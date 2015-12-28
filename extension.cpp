/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "core.h"
#include "database_mysql.h"
#include "database_redis.h"
#include "database_ssdb.h"

#ifdef DEBUG
#include "test.cpp"
#endif

extern "C" {
    CORE_EXPORT zend_module_entry* get_module() {
        static php::module module("yafa", "0.3.5");
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
        
        static php::class_declare<database_redis> c2("yafa_database_redis");
        c2
            .method("init"      , database_redis::init      )
            .method("get_master", database_redis::get_master)
            .method("get_slave" , database_redis::get_slave )
            .property("_conf", ZEND_ACC_PROTECTED | ZEND_ACC_STATIC)
            .property("_cache", ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
        module.add(c2);
        
        static php::class_declare<database_ssdb> c3("yafa_database_ssdb");
        c3
            .method("init"      , database_ssdb::init      )
            .method("get_master", database_ssdb::get_master)
            .method("get_slave" , database_ssdb::get_slave )
            .property("_conf", ZEND_ACC_PROTECTED | ZEND_ACC_STATIC)
            .property("_cache", ZEND_ACC_PROTECTED | ZEND_ACC_STATIC)
            .property("prefix", ZEND_ACC_PUBLIC);
        module.add(c3);
        
        return module;
    }
};

