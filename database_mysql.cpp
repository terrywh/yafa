/* 
 * File:   database_mysql.cpp
 * Author: wuhao
 * 
 * Created on 2015年11月19日, 上午10:41
 */
#include "easy.h"
#include "database_mysql.h"
#include "database_mysql_where.h"

// std::string database_mysql::CLASS_NAME = "wue_database_mysql";
std::string database_mysql::MASTER     = "master";
std::string database_mysql::SLAVE      = "slave";

database_mysql::database_mysql() {
}
void database_mysql::__construct(easy::param_t& param) {
    _mysqli = param[0];
}

database_mysql::~database_mysql() {
    
}

easy::value_t database_mysql::init(easy::param_t& param) {
    if(param.size < 1 || !param[0].is_array()) {
        zend_throw_error(NULL, "mysql config of type array is required");
        return false;
    }
    easy::value_t conf = param[0];
    if(!conf.has("master") && !conf.has("slave")) {
        zend_throw_error(NULL, "illegal mysql config #1");
        return false;
    }
    sprop("wuf_database_mysql", "_conf", conf);
    return true;
}

easy::value_t database_mysql::create(easy::value_t& config) {
    std::string host = config["host"],
            port = config["port"],
            user = config["user"],
            pass = config["pass"],
            db   = config["db"];
    if(host.empty() || port.empty() || user.empty() || pass.empty()) {
        zend_throw_error(NULL, "illegal mysql config #3");
        return easy::VALUE_NULL;
    }
    std::string s;
    s.append("new wuf_database_mysql(new mysqli(\"")
        .append(host)
        .append("\",\"")
        .append(user)
        .append("\",\"")
        .append(pass)
        .append("\",\"");
    if(db.empty()) {
         s.append("\",");
    }else{
        s.append(db)
            .append("\",");
    }
    s.append(port)
        .append("))");
    zval retval;
    zend_eval_string(const_cast<char*>(s.c_str()), &retval, const_cast<char*>("wuf_database_mysql::create"));
    return &retval;
}

easy::value_t database_mysql::get_master(easy::param_t& param) {
    zend_long  idx = 0;
    std::string key = "master_";
    if(param.size == 1) {
        idx = param[0];
    }
    key.append(std::to_string(idx));
    easy::value_t cache = sprop("wuf_database_mysql", "_cache");
    easy::value_t db = cache[key];
    if(!db.is_object()) {
        easy::value_t conf = sprop("wuf_database_mysql", "_conf");
        conf = conf["master"][idx];
        if(!conf.is_array()) {
            zend_throw_error(NULL, "illegal mysql config #2");
            return easy::VALUE_NULL;
        }
        db = database_mysql::create(conf);
        cache.set(key, db);
    }
    return db;
}
easy::value_t database_mysql::get_slave(easy::param_t& param) {
   zend_long  idx = 0;
    std::string key = "slave_";
    if(param.size == 1) {
        idx = param[0];
    }
    key.append(std::to_string(idx));
    easy::value_t cache = sprop("wuf_database_mysql", "_cache");
    easy::value_t db = cache[key];
    if(!db.is_object()) {
        easy::value_t conf = sprop("wuf_database_mysql", "_conf");
        conf = conf["slave"][idx];
        if(!conf.is_array()) {
            zend_throw_error(NULL, "illegal mysql config #2");
            return easy::VALUE_NULL;
        }
        db = database_mysql::create(conf);
        cache.set(key, db);
    }
    return db;
}

easy::value_t database_mysql::format(easy::param_t& param) {
    if(param.size < 1 || !param[0].is_string()) {
        zend_throw_error(NULL, "format string is required");
        return easy::VALUE_NULL;
    }
    if(param.size == 1) return param[0];
    std::string before = param[0];
    std::string after;
    size_t i = 0, m = 1;
    while(i < before.length() && m <= param.size) {
        if(before[i]!='?') {
            after.push_back(before[i]);
        }else{
            if(m == param.size) {
                zend_throw_error(nullptr, "format arguments is not enough");
                return after;
            }
            after.push_back('\'');
            after.append((std::string)easy::call_method_1(&_mysqli, "escape_string", param[m]));
            after.push_back('\'');
            ++m;
        }
        ++i;
    }
    if(m < param.size) {
        zend_throw_error(nullptr, "too many format arguments");
        return after;
    }
    return after;
}

easy::value_t database_mysql::format_query(easy::param_t& param) {
    easy::value_t sql = format(param);
    return easy::call_method_1(&_mysqli, "query", sql);
}

easy::value_t database_mysql::insert(easy::param_t& param) {
    std::string table  = param[0];
    easy::value_t data = param[1];
    if(table.empty() || data.length() == 0) {
        zend_throw_error(nullptr, "table name and data is required");
        return easy::VALUE_NULL;
    }
    std::string sql = "INSERT INTO `" + table + "`(";
    size_t length = data.length();
    for(auto i=data.begin();data.has_more(i);data.next(i)) {
        sql.push_back('`');
        sql.append((std::string)easy::value_t(i.key));
        sql.push_back('`');
        if(--length != 0) {
            sql.push_back(',');
        }
    }
    sql.append(") VALUES(");
    length = data.length();
    for(auto i=data.begin();data.has_more(i);data.next(i)) {
        sql.push_back('\'');
        easy::value_t value = i.val;
        if(value.is_array()) {
            value = easy::call_method_1(nullptr, "json_encode", easy::value_t(i.val));
        }
        sql.append((std::string)easy::call_method_1(&_mysqli, "escape_string", value));
        sql.push_back('\'');
        --length;
        if(length != 0) {
            sql.push_back(',');
        }
    }
    sql.push_back(')');
    php_printf("SQL: [%s]\n", sql.c_str());
    return easy::call_method_1(&_mysqli, "query", sql);
}

easy::value_t database_mysql::remove(easy::param_t& param) {
    
}

easy::value_t database_mysql::update(easy::param_t& param) {
    std::string   table  = param[0];
    easy::value_t data = param[1];
    if(table.empty() || data.length() == 0) {
        zend_throw_error(nullptr, "table name and data is required");
        return easy::VALUE_NULL;
    }
    std::string sql = "UPDATE `" + table + "` SET ";
    unsigned long len = data.length();
    for(auto i=data.begin(); data.has_more(i); data.next(i)) {
        sql.push_back('`');
        sql.append((std::string)easy::value_t(i.key));
        sql.push_back('`');
        sql.push_back('=');
        sql.push_back('\'');
        easy::value_t value = i.val;
        if(value.is_array()) {
            value = easy::call_method_1(nullptr, "json_encode", easy::value_t(i.val));
        }
        sql.append((std::string)easy::call_method_1(&_mysqli, "escape_string", value));
        sql.push_back('\'');
        if(--len != 0) {
            sql.push_back(',');
        }
    }
    
    if(param.size >= 3) {
        easy::value_t cond = param[2];
        database_mysql_where where(_mysqli, sql);
        where.build(cond);
    }else{
        zend_error(E_NOTICE, "database update without conditions");
    }
    php_printf("SQL: [%s]\n", sql.c_str());
    return easy::call_method_1(&_mysqli, "query", sql);
}

easy::value_t database_mysql::select(easy::param_t& param) {
    
}

easy::value_t database_mysql::one(easy::param_t& param) {
}

// 将其他所有调用转接到 mysqli
easy::value_t database_mysql::__call(const std::string& name, easy::param_t& param) {
    easy::argument_t args;
    for(unsigned int i=0;i<param.size;++i) {
        args.push(param[i]);
    }
    return easy::call_method(&_mysqli, name, args);
}
bool database_mysql::__isset(const std::string& name) {
    easy::property_exists(_mysqli, name);
    return true;
}
easy::value_t database_mysql::__get(const std::string& name) {
    return easy::property(_mysqli, name);
}


