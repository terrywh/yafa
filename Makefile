PHP_CONFIG?=/data/server/php/bin/php-config
PHP_INCLUES=$(shell $(PHP_CONFIG) --includes)
PHP_EXTENSION_DIR=$(shell $(PHP_CONFIG) --extension-dir)
CONF?=Release

CORE_OBJECTS=php/invoke.o php/value_iterator.o php/value.o php/parameter.o php/class.o php/property.o php/module.o
YAFA_OBJECTS=database_mysql_where.o database_mysql.o extension.o
TARGET=yafa.so

CXXFLAGS+= -std=c++11 -m64 -fPIC

ifeq ($(CONF),Debug)
	CXXFLAGS+= -O0 -g
else
	CXXFLAGS+= -O2
endif

.PHONY:all clean install test environ
all:$(TARGET) install
	@echo done.

test:
	@echo $(PHP_INCLUES)
	@echo $(PHP_EXTENSION_DIR)
clean:
	rm -f $(CORE_OBJECTS)
	rm -f $(YAFA_OBJECTS)
	rm -f $(TARGET)

$(CORE_OBJECTS):%.o:%.cpp
	g++ $(CXXFLAGS) $(PHP_INCLUES) -c $^ -o $@
$(YAFA_OBJECTS):%.o:%.cpp
	g++ $(CXXFLAGS) $(PHP_INCLUES) -c $^ -o $@
$(TARGET):$(CORE_OBJECTS) $(YAFA_OBJECTS)
	g++ $(CXXFLAGS) -shared $^ -static-libstdc++ -o $@
install:$(TARGET)
	rm -f $(PHP_EXTENSION_DIR)/$(TARGET)
	cp $(TARGET) $(PHP_EXTENSION_DIR)/
	@echo please add $(TARGET) to php.ini.

