/* 
 * File:   database_mysql.cpp
 * Author: wuhao
 * 
 * Created on 2015年11月19日, 上午10:41
 */
#include "easy.h"
#include "database_mysql.h"

std::string database_mysql::CLASS_NAME = "wue_database_mysql";
std::string database_mysql::MASTER     = "master";
std::string database_mysql::SLAVE      = "slave";

database_mysql::database_mysql() {
    php_printf("test 1");
}
void database_mysql::__construct(easy::param_t& param) {
    _mysqli = param[0];
}

database_mysql::~database_mysql() {
    
}
easy::value_array                    database_mysql::_config;
std::map<std::string, easy::value_t> database_mysql::_cache;

easy::value_t database_mysql::init(easy::param_t& param) {
    if(param.size < 1 || !param[0].is_array()) {
        zend_throw_error(NULL, "mysql config of type array is required");
        // throw easy::exception_t("config array is required");
        return false;
    }
    _config = param[0];
    if((!_config.has("master") || !_config["master"].is_array()) && (!_config.has("slave") || !_config["slave"].is_array())) {
        zend_throw_error(NULL, "illegal mysql config #1");
        return false;
    }
    return true;
}

easy::value_t database_mysql::create(easy::value_array& config) {
    if(!config.has("host") || !config.has("port") || !config.has("user") || !config.has("pass")) {
        zend_throw_error(NULL, "illegal mysql config #3");
        return easy::value_t();
    }
    std::ostringstream c;
    c << "new " << database_mysql::CLASS_NAME << "("  << "new mysqli(\""
            << (std::string)config["host"] << "\",\""
            << (std::string)config["user"] << "\",\""
            << (std::string)config["pass"] << "\",\"";
    if(config.has("db")) {
        c << (std::string)config["db"] << "\",";
    }else{
        c << "\",";
    }
    c << (zend_long)config["port"] << "));";
    std::string s = c.str();
    php_printf("object create eval [%s]\n", s.c_str());
    zval retval;
    if(zend_eval_stringl(const_cast<char*>(s.c_str()), s.length(), &retval, "database_mysql::create") == SUCCESS) {
        return easy::value_t(&retval);
    }else{
        return easy::value_t();
    }
};

easy::value_t database_mysql::get_master(easy::param_t& param) {
    zend_long  idx = 0;
    std::string key = "master_";
    if(param.size == 1) {
        idx = param[0];
    }
    key.append(std::to_string(idx));
    easy::value_t cache = _cache[key];
    if(cache.is_null()) {
        easy::value_array configs = database_mysql::_config["master"];
        if(!configs[idx].is_array()) {
            zend_throw_error(NULL, "illegal mysql config #2");
            return easy::value_t();
        }
        easy::value_array config = configs[idx];
        cache = database_mysql::create(config);
        _cache[key] = cache;
    }
    return cache;
}
easy::value_t database_mysql::get_slave(easy::param_t& param) {
    return std::string("slave");
}

easy::value_t database_mysql::format(easy::param_t& param) {

}

easy::value_t database_mysql::query_format(easy::param_t& param) {

}

easy::value_t database_mysql::__call(const std::string& name, easy::param_t& param) {

}
bool database_mysql::__isset(const std::string& name) {
    
}
easy::value_t database_mysql::__get(const std::string& name) {

}


