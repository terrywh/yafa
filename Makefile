PHP_CONFIG?=/data/server/php/bin/php-config
PHP_INCLUDE=$(shell $(PHP_CONFIG) --includes)
BOOST_INCLUDE?= -I/data/htdocs/boost
PHP_EXTENSION_DIR=$(shell $(PHP_CONFIG) --extension-dir)
CONF?=Release

CORE_OBJECTS= php/invoke.o php/value_iterator.o php/value.o php/parameter.o php/class.o php/property.o php/module.o
DATABASE_OBJECTS= database_ssdb.o database_redis.o database_mysql_where.o database_mysql.o
EXTENSION_OBJECTS= extension.o
DEPS_STATIC_LIBRARY= deps/libssdb-client.a
TARGET=yafa.so

LDFLAGS+= -static-libstdc++
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
	rm -f $(DATABASE_OBJECTS)
	rm -f $(TARGET)

$(CORE_OBJECTS):%.o:%.cpp
	g++ $(CXXFLAGS) $(BOOST_INCLUDE) $(PHP_INCLUDE) -c $^ -o $@
$(DATABASE_OBJECTS):%.o:%.cpp
	g++ $(CXXFLAGS) $(BOOST_INCLUDE) $(PHP_INCLUDE) -c $^ -o $@
$(EXTENSION_OBJECTS):%.o:%.cpp
	g++ $(CXXFLAGS) $(BOOST_INCLUDE) $(PHP_INCLUDE) -c $^ -o $@

$(TARGET):$(DEPS_STATIC_LIBRARY) $(CORE_OBJECTS) $(DATABASE_OBJECTS) $(EXTENSION_OBJECTS)
	g++ $(CXXFLAGS) $(LDFLAGS) -shared $^ -o $@
install:$(TARGET)
	rm -f $(PHP_EXTENSION_DIR)/$(TARGET)
	cp $(TARGET) $(PHP_EXTENSION_DIR)/
	@echo please add $(TARGET) to php.ini.

