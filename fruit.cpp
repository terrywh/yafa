/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "easy.h"
#include "database_mysql.h"

easy::value_t testFunction(easy::param_t& argv) {
    return easy::value_t("hello");
};

class testClass : public easy::class_base {
public:
    static std::string CLASS_NAME;
    easy::value_t test(easy::param_t& argv);
};

std::string testClass::CLASS_NAME="testClass";
easy::value_t testClass::test(easy::param_t& argv) {
    return std::string("abc");
}


extern "C" {
    EASY_EXPORT zend_module_entry* get_module() {
        static easy::module_t module("fruit", "0.1.0");
        // 加入方法
        module.add("testFunction", testFunction);
        // 加入类
        static easy::class_t<testClass> c1;
        // 类成员函数
        c1.add("test", &testClass::test);
        module.add(c1);
        
        static easy::class_t<database_mysql> c2;
        c2.add("init", database_mysql::init);
        c2.add("get_slave", database_mysql::get_slave);
        c2.add("get_master", database_mysql::get_master);
        c2.add("format", &database_mysql::format);
        c2.add("query_format", &database_mysql::query_format);
        module.add(c2);
        
        return module;
    }
};
