/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "core.h"
#include "database_mysql.h"

php::value testFunction(const php::parameter& argv) {
    // return php::call_method_1(nullptr, "json_encode", std::string("abc"));
    php::value v("std");
    return v;
}

class testClass : public php::class_base {
public:
    static std::string class_name;
    static zend_class_entry* class_entry;
    php::value test(const php::parameter& argv);
};

php::value testClass::test(const php::parameter& param) {
    php::property prop(this);
    // prop.sset("save", "world");
    php::value v0 = prop.sget("save");
    
    v0["key1"] = "111";
    v0["key2"] = 222l;
    v0["key3"]["key4"] = "444";

    prop.sset("save", v0);
    return v0;
}

std::string       testClass::class_name;
zend_class_entry* testClass::class_entry;

extern "C" {
    CORE_EXPORT zend_module_entry* get_module() {
        static php::module module("yafa", "0.1.0");
//        // 加入函数
//        module.add("testFunction", testFunction);
//        // 加入类
//        static php::class_declare<testClass> c1("testClass");
//        // 类成员方法
//        c1.method("test", &testClass::test)
//            .property("prop1", ZEND_ACC_PUBLIC)
//            .property("prop2", ZEND_ACC_PUBLIC)
//            .property("save", ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);  // 属性
//        module.add(c1);
        static php::class_declare<database_mysql> c1("yafa_database_mysql");
        c1
            .method("init", database_mysql::init)
            .method("get_master", database_mysql::get_master)
            .method("get_slave" , database_mysql::get_slave)
            .method("format"    , &database_mysql::format)
            .method("format_query", &database_mysql::format_query)
            .method("insert"    , &database_mysql::insert)
            .method("remove"    , &database_mysql::remove)
            .method("update"    , &database_mysql::update)
            .method("select"    , &database_mysql::select)
            .method("one"       , &database_mysql::one)
            .property("_conf", ZEND_ACC_PROTECTED | ZEND_ACC_STATIC)
            .property("_cache", ZEND_ACC_PROTECTED | ZEND_ACC_STATIC)
            .property("_mysqli", ZEND_ACC_PROTECTED);
        
        module.add(c1);
        return module;
    }
};

