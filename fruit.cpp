/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "easy.h"
#include "database_mysql.h"

easy::value_t testFunction(easy::param_t& argv) {
    return easy::value_t("hello");
}

class testClass : public easy::class_base {
public:
    easy::value_t test(easy::param_t& argv);
};

easy::value_t testClass::test(easy::param_t& param) {
    sprop("testClass", "save", param[0]);
    return std::string("abc");
}


extern "C" {
    EASY_EXPORT zend_module_entry* get_module() {
        static easy::module_t module("fruit", "0.1.0");
        // 加入方法
        module.add("testFunction", testFunction);
        // 加入类
        static easy::class_t<testClass> c1("testClass");
        // 类成员函数
        c1.method("test", &testClass::test);
        // 静态属性
        c1.property("save", easy::value_t("abcdefg"), ZEND_ACC_PUBLIC);
        module.add(c1);
        
        static easy::class_t<database_mysql> c2("wuf_database_mysql");
        c2.method("init", database_mysql::init);
        // c2.add("get_slave", database_mysql::get_slave);
        c2.method("get_master", database_mysql::get_master);
        c2.method("format", &database_mysql::format);
        c2.property("_cache", easy::value_t::create_array(), ZEND_ACC_PRIVATE);
        c2.property("_conf",  easy::VALUE_NULL, ZEND_ACC_PRIVATE);
        c2.method("format_query", &database_mysql::format_query);
        module.add(c2);
        
        return module;
    }
};

