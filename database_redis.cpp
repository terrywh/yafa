/* 
 * File:   database_redis.cpp
 * Author: wuhao
 * 
 * Created on 2015年11月27日, 下午2:12
 */
#include "boost/algorithm/string.hpp"
#include "core.h"
#include "database_redis.h"

std::string       database_redis::class_name;
zend_class_entry* database_redis::class_entry;


php::value database_redis::init(const php::parameter& param) {
    if(param.size < 1 || !param[0].is_type(IS_ARRAY)) {
        zend_throw_error(NULL, "redis config of type array is required");
        return false;
    }
    php::value conf = param[0];
    if(!conf.__isset("master") && !conf.__isset("slave")) {
        zend_throw_error(NULL, "illegal redis config #1");
        return false;
    }
    php::property prop(database_redis::class_entry);
    prop.sset("_conf", conf);
    return true;
}
php::value database_redis::get_master(const php::parameter& param) {
    zend_long  idx = 0;
    std::string key = "master_";
    if(param.size == 1) {
        idx = param[0];
    }
    key.append(std::to_string(idx));
    php::property      prop(database_redis::class_entry);
    php::value cache = prop.sget("_cache");
    php::value db    = cache[key];
    if(!db.is_type(IS_OBJECT)) {
        php::value conf = prop.sget("_conf");
        conf = conf["master"][idx];
        if(!conf.is_type(IS_ARRAY)) {
            zend_throw_error(NULL, "illegal redis config #2");
            return php::value();
        }
        db = database_redis::create(conf);
        cache[key] = db;
    }
    return db;
}
php::value database_redis::get_slave(const php::parameter& param) {
    zend_long  idx = 0;
    std::string key = "slave_";
    if(param.size == 1) {
        idx = param[0];
    }
    key.append(std::to_string(idx));
    php::property      prop(database_redis::class_entry);
    php::value cache = prop.sget("_cache");
    php::value db    = cache[key];
    if(!db.is_type(IS_OBJECT)) {
        php::value conf = prop.sget("_conf");
        conf = conf["slave"][idx];
        if(!conf.is_type(IS_ARRAY)) {
            zend_throw_error(NULL, "illegal redis config #2");
            return php::value();
        }
        db = database_redis::create(conf);
        cache[key] = db;
    }
    return db;
}

php::value database_redis::create(php::value& config) {
    std::string create_redis = "new Redis()";
    php::value  redis;
    zend_eval_stringl(const_cast<char*>(create_redis.c_str()), create_redis.length(), redis.intern(), "database_redis::create");
    if(!redis.is_type(IS_OBJECT)) {
        zend_throw_error(nullptr, "failed to create Redis client");
        return php::value();
    }
    // connect
    std::vector<zval> connect_param;
    connect_param.push_back(*config["host"].intern());
    connect_param.push_back(*config["port"].intern());
    if(config.__isset("timeout")) {
        connect_param.push_back(*config["timeout"].intern());
    }else{
        php::value timeout = 2.0;
        connect_param.push_back(*timeout.intern());
    }
    php::call_method(&redis, "connect", connect_param);
    // setOption
    if(config.__isset("prefix")) {
        php::value redis_opt_prefix = 2;
        php::call_method_2(&redis, "setoption", redis_opt_prefix, config["prefix"]);
    }
    // Auth
    if(config.__isset("auth")) {
        php::call_method_1(&redis, "auth", config["auth"]);
    }
    return std::move(redis);
}
