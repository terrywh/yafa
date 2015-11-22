/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "easy.h"
#include "database_mysql.h"

easy::value_t testFunction(easy::param_t& argv) {
    return easy::call_method_1(nullptr, "json_encode", std::string("abc"));
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
        static easy::module_t module("wuf", "0.1.0");
//        // 加入函数
//        module.function("testFunction", testFunction);
//        // 加入类
//        static easy::class_t<testClass> c1("testClass");
//        // 类成员方法
//        c1.method("test", &testClass::test)
//            .property("save", easy::value_t("abcdefg"), ZEND_ACC_PUBLIC);  // 静态属性
//        module.add(c1);
        
        static easy::class_t<database_mysql> c2("wuf_database_mysql");
        c2
            .method("init", database_mysql::init)
            .method("get_slave", database_mysql::get_slave)
            .method("get_master", database_mysql::get_master)
            .method("format", &database_mysql::format)
            .method("format_query", &database_mysql::format_query)
            .method("insert", &database_mysql::insert)
            .method("remove", &database_mysql::remove)
            .method("update", &database_mysql::update)
            .method("select", &database_mysql::select)
            .method("one", &database_mysql::one)
            .property("_cache", easy::value_t::create_array(), ZEND_ACC_PRIVATE)
            .property("_conf",  easy::VALUE_NULL, ZEND_ACC_PRIVATE);
        
        module.add(c2);
        
        return module;
    }
};

