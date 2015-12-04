PHP_CONFIG?=/data/server/php/bin/php-config
PHP_INCLUDE=$(shell $(PHP_CONFIG) --includes | sed 's/-I/-isystem /g')
BOOST_INCLUDE?= -I/data/htdocs/boost
PHP_EXTENSION_DIR=$(shell $(PHP_CONFIG) --extension-dir)
CONF?=Release

CORE_OBJECTS= php/invoke.o php/value_iterator.o php/value.o php/parameter.o php/class.o php/property.o php/module.o
DATABASE_OBJECTS= database_ssdb.o database_redis.o database_mysql_where.o database_mysql.o extension.o
DEPS_LIBRARY= deps/libssdb-client.a deps/libhiredis.a
TARGET=yafa.so

CXXFLAGS+= -std=c++11 -m64 -fPIC

ifeq ($(CONF),Debug)
	CXXFLAGS+= -DDEBUG=1 -O0 -g
else
	CXXFLAGS+= -O2
endif

.PHONY:all clean install test environ
all:$(TARGET) install
	@echo done.

test:
	@echo $(PHP_INCLUDE)
	@echo $(PHP_EXTENSION_DIR)
clean:
	rm -f $(CORE_OBJECTS)
	rm -f $(YAFA_OBJECTS)
	rm -f $(TARGET)

$(CORE_OBJECTS):%.o:%.cpp
	clang++ $(CXXFLAGS) $(BOOST_INCLUDE) $(PHP_INCLUDE) -c $^ -o $@
$(DATABASE_OBJECTS):%.o:%.cpp
	clang++ $(CXXFLAGS) $(BOOST_INCLUDE) $(PHP_INCLUDE) -c $^ -o $@
$(TARGET):$(CORE_OBJECTS) $(DATABASE_OBJECTS) $(DEPS_LIBRARY) 
	clang++ $(CXXFLAGS) -shared $^ -static-libstdc++ -o $@
install:$(TARGET)
	rm -f $(PHP_EXTENSION_DIR)/$(TARGET)
	cp $(TARGET) $(PHP_EXTENSION_DIR)/
	@echo please add $(TARGET) to php.ini.

