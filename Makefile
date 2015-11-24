PHP_CONFIG?=/data/server/php/bin/php-config
PHP_INCLUES=$(shell $(PHP_CONFIG) --includes)
PHP_EXTENSION_DIR=$(shell $(PHP_CONFIG) --extension-dir)
CONF?=Release

EASY_OBJECTS=easy/util.o easy/value.o easy/argument.o easy/class.o easy/module.o
YAFA_OBJECTS=database_mysql.o database_mysql_where.o extension.o
TARGET=yafa.so

CXXFLAGS+= -std=c++11 -m64 -fPIC

ifeq ($(CONF),'Debug')
    CXXFLAGS+= -O0 -g
endif
ifeq ($(CONF),'Release')
    CXXFLAGS+= -O2
endif

.PHONY:all clean install test
all:$(TARGET) install
	@echo done.
test:
	@echo $(PHP_INCLUES)
	@echo $(PHP_EXTENSION_DIR)
clean:
	rm -f $(EASY_OBJECTS)
	rm -f $(YAFA_OBJECTS)
	rm -f $(TARGET)
$(EASY_OBJECTS):%.o:%.cpp
	g++ $(CXXFLAGS) $(PHP_INCLUES) -c $^ -o $@
$(YAFA_OBJECTS):%.o:%.cpp
	g++ $(CXXFLAGS) $(PHP_INCLUES) -c $^ -o $@
$(TARGET):$(EASY_OBJECTS) $(YAFA_OBJECTS)
	g++ $(CXXFLAGS) -fPIC -shared $^ -o $@
install:$(TARGET)
	cp $(TARGET) $(PHP_EXTENSION_DIR)/
	@echo please add $(TARGET) to php.ini.

