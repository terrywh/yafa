/* 
 * File:   database_mysql.cpp
 * Author: wuhao
 * 
 * Created on 2015年11月19日, 上午10:41
 */
#include "core.h"

#include "database_mysql.h"
#include "database_mysql_where.h"
#include "php/parameter.h"

std::string       database_mysql::class_name;
zend_class_entry* database_mysql::class_entry;

std::string database_mysql::MASTER     = "master";
std::string database_mysql::SLAVE      = "slave";

database_mysql::database_mysql() {
}
void database_mysql::__construct(const php::parameter& param) {
    php::property prop(this);
    prop.oset("_mysqli", param[0]);
}

database_mysql::~database_mysql() {
    
}

php::value database_mysql::init(const php::parameter& param) {
    if(param.size < 1 || !param[0].is_type(IS_ARRAY)) {
        zend_throw_error(NULL, "mysql config of type array is required");
        return false;
    }
    php::value conf = param[0];
    if(!conf.__isset("master") && !conf.__isset("slave")) {
        zend_throw_error(NULL, "illegal mysql config #1");
        return false;
    }
    php::property prop(database_mysql::class_entry);
    prop.sset("_conf", conf);
    return true;
}

php::value database_mysql::create(php::value& config) {
    std::string host = config["host"],
            port = config["port"],
            user = config["user"],
            pass = config["pass"],
            db   = config["db"];
    if(host.empty() || port.empty() || user.empty()) {
        zend_throw_error(NULL, "illegal mysql config #3");
        return php::null;
    }
    std::string s;
    s.append("new mysqli(\"")
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
        .append(")");
    php::value mysqli, object;
    zend_eval_string(const_cast<char*>(s.c_str()), mysqli.intern(), const_cast<char*>("yafa_database_mysql::create_mysqli"));
    php::property prop(mysqli);
    if(!prop.oget("connect_error").is_empty()) {
        zend_throw_error(nullptr, "failed to create mysqli");
        return php::null;
    }
    
    object_init_ex(object.intern(), database_mysql::class_entry);
    php::_store * p_store = php::get_store(object);
    php::parameter param(mysqli.intern(), 1);
    p_store->self->__construct(param);
    return std::move(object);
}

php::value database_mysql::get_master(const php::parameter& param) {
    zend_long  idx = 0;
    std::string key = "master_";
    if(param.size == 1) {
        idx = param[0];
    }
    key.append(std::to_string(idx));
    php::property prop(database_mysql::class_entry);
    php::value cache = prop.sget("_cache");
    php::value db    = cache[key];
    if(!db.is_type(IS_OBJECT)) {
        php::value conf = prop.sget("_conf");
        conf = conf["master"][idx];
        if(!conf.is_type(IS_ARRAY)) {
            zend_throw_error(NULL, "illegal mysql config #2");
            return php::null;
        }
        db = database_mysql::create(conf);
        if(db.is_type(IS_OBJECT)) {
            cache[key] = db;
        }
    }
    return db;
}
php::value database_mysql::get_slave(const php::parameter& param) {
    zend_long  idx = 0;
    std::string key = "slave_";
    if(param.size == 1) {
        idx = param[0];
    }
    key.append(std::to_string(idx));
    php::property prop(database_mysql::class_entry);
    php::value cache = prop.sget("_cache");
    php::value db    = cache[key];
    if(!db.is_type(IS_OBJECT)) {
        php::value conf = prop.sget("_conf");
        conf = conf["slave"][idx];
        if(!conf.is_type(IS_ARRAY)) {
            zend_throw_error(NULL, "illegal mysql config #2");
            return php::null;
        }
        db = database_mysql::create(conf);
        cache[key] = db;
    }
    return db;
}

php::value database_mysql::format(const php::parameter& param) {
    if(param.size < 1 || !param[0].is_type(IS_STRING)) {
        zend_throw_error(NULL, "format string is required");
        return php::null;
    }
    if(param.size == 1) return param[0];
    std::string before = param[0];
    std::string after;
    size_t i = 0, m = 1;
    php::property prop(this);
    php::value    _mysqli = prop.oget("_mysqli");
    while(i < before.length() && m <= param.size) {
        if(before[i]!='?') {
            after.push_back(before[i]);
        }else{
            if(m == param.size) {
                zend_throw_error(nullptr, "format arguments is not enough");
                return after;
            }
            
            if(param[m].is_type(IS_LONG)) {
                after.append(std::to_string((zend_long)param[m]));
            }else{
                after.push_back('\'');
                after.append((std::string)php::call_method_1(&_mysqli, "escape_string", param[m]));
                after.push_back('\'');
            }
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

php::value database_mysql::format_query(const php::parameter& param) {
    php::value sql = format(param);
    php::property prop(this);
    php::value _mysqli = prop.oget("_mysqli");
    return php::call_method_1(&_mysqli, "query", sql);
}

php::value database_mysql::insert(const php::parameter& param) {
    std::string table  = param[0];
    php::value data = param[1];
    if(table.empty() || data.length() == 0) {
        zend_throw_error(nullptr, "table name and data is required");
        return php::null;
    }
    std::string sql = "INSERT INTO `" + table + "`(";
    size_t length = data.length();
    for(auto i=data.begin(); i!=data.end(); ++i) {
        sql.push_back('`');
        sql.append(std::string(ZSTR_VAL(i->key), ZSTR_LEN(i->key)));
        sql.push_back('`');
        if(--length != 0) {
            sql.push_back(',');
        }
    }
    sql.append(") VALUES(");
    length = data.length();
    php::property prop(this);
    php::value    _mysqli = prop.oget("_mysqli");
    for(auto i=data.begin(); i!=data.end(); ++i) {
        sql.push_back('\'');
        php::value val(i->val, false);
        if(val.is_type(IS_ARRAY)) {
            val = php::call_method_2(nullptr, "json_encode", php::value(i->val), php::value(1<<8)); // PHP_JSON_UNESCAPED_UNICODE
        }
        sql.append((std::string)php::call_method_1(&_mysqli, "escape_string", val));
        sql.push_back('\'');
        --length;
        if(length != 0) {
            sql.push_back(',');
        }
    }
    sql.push_back(')');
    return php::call_method_1(&_mysqli, "query", sql);
}

php::value database_mysql::remove(const php::parameter& param) {
    std::string table  = param[0];
    php::value data = param[1];
    if(table.empty() || data.length() == 0) {
        zend_throw_error(nullptr, "table name and conditions is required");
        return php::null;
    }
    std::string sql = "DELETE FROM `"+table+"`";
    php::property prop(this);
    php::value    _mysqli = prop.oget("_mysqli");
    database_mysql_where where(_mysqli, sql);
    where.build(data);
    return php::call_method_1(&_mysqli, "query", sql);
}

php::value database_mysql::update(const php::parameter& param) {
    std::string table = param[0];
    php::value  data  = param[1];
    if(table.empty() || !data.is_type(IS_ARRAY) || data.length() == 0) {
        zend_throw_error(nullptr, "table name and data is required");
        return php::null;
    }
    std::string sql = "UPDATE `" + table + "` SET ";
    unsigned long len = data.length();
    php::property prop(this);
    php::value    _mysqli = prop.oget("_mysqli");
    for(auto i=data.begin(); i!=data.end(); ++i) {
        sql.push_back('`');
        sql.append(std::string(ZSTR_VAL(i->key), ZSTR_LEN(i->key)));
        sql.push_back('`');
        sql.push_back('=');
        sql.push_back('\'');
        php::value val = i->val;
        if(val.is_type(IS_ARRAY)) {
            val = php::call_method_2(nullptr, "json_encode", php::value(i->val), php::value(1<<8)); // PHP_JSON_UNESCAPED_UNICODE
        }
        sql.append((std::string)php::call_method_1(&_mysqli, "escape_string", val));
        sql.push_back('\'');
        if(--len != 0) {
            sql.push_back(',');
        }
    }
    if(param.size >= 3) {
        php::value cond = param[2];
        database_mysql_where where(_mysqli, sql);
        where.build(cond);
    }else{
        zend_error(E_NOTICE, "database update without conditions");
    }
    return php::call_method_1(&_mysqli, "query", sql);
}

php::value database_mysql::select(const php::parameter& param) {
    std::string   table  = param[0];
    php::value column = param[1];
    if(table.empty() || !column.is_type(IS_STRING) && !column.is_type(IS_ARRAY) || column.length() < 1) {
        zend_throw_error(nullptr, "table name and columns is required");
        return php::null;
    }
    // 以下参数可选
    php::value cond   = param[2];
    php::value group  = param[3];
    php::value order  = param[4];
    php::value limit  = param[5];
    
    php::property prop(this);
    php::value    _mysqli = prop.oget("_mysqli");
        
    std::string sql = "SELECT ";
    if(column.is_type(IS_STRING)) {
        sql.append((std::string)column);
    }else if(column.is_type(IS_ARRAY)) {
        int len = column.length();
        for(auto i=column.begin(); i!=column.end(); ++i) {
            sql.push_back('`');
            sql.append((std::string)php::value(i->val));
            sql.push_back('`');
            if(--len !=0) {
                sql.push_back(',');
            }
        }
    }
    sql.append(" FROM `");
    sql.append(table);
    sql.push_back('`');
    if(!cond.is_empty()) {
        database_mysql_where where(_mysqli, sql);
        where.build(cond);
    }
    if(!group.is_empty()) {
        sql.append(" GROUP BY ");
        if(group.is_type(IS_STRING)) {
            sql.append((std::string)group);
        }else if(group.is_type(IS_ARRAY)) {
            int len = group.length();
            for(auto i=group.begin(); i!=group.end(); ++i) {
                sql.push_back('`');
                sql.append((std::string)php::value(i->val));
                sql.push_back('`');
                if(--len !=0) {
                    sql.push_back(',');
                }
            }
        }
    }
    build_order(sql, order);
    if(!limit.is_empty()) {
        sql.append(" LIMIT ");
        if(limit.is_type(IS_STRING)) {
            sql.append((std::string)limit);
        }else if(limit.is_type(IS_LONG)) {
            sql.append(std::to_string((zend_long)limit));
        }else if(limit.is_type(IS_ARRAY)) {
            int len = limit.length();
            for(auto i=limit.begin(); i!=limit.end(); ++i) {
                sql.append(std::to_string((zend_long)php::value(i->val)));
                if(--len !=0) {
                    sql.push_back(',');
                }
            }
        }
    }
    return php::call_method_1(&_mysqli, "query", sql);
}

void database_mysql::build_order(std::string& sql, php::value& order) {
    if(order.is_empty()) {
        return;
    }
    sql.append(" ORDER BY ");
    if(order.is_type(IS_STRING)) {
        sql.append((std::string)order);
        return;
    }else if(!order.is_type(IS_ARRAY)) {
        zend_throw_error(nullptr, "order must be array with either bool or \"ASC\"/\"DESC\" value]");
        return;
    }
    int len = order.length();
    for(auto i=order.begin(); i!=order.end(); ++i) {
        sql.push_back('`');
        sql.append(std::string(ZSTR_VAL(i->key), ZSTR_LEN(i->key)));
        sql.push_back('`');
        php::value osc = php::value(i->val);
        if(osc.is_empty()) { // bool 变量转换
            sql.append(" DESC");
        }else if(osc.is_type(IS_STRING)) {
            sql.push_back(' ');
            sql.append((std::string)osc);
        }else{
            sql.append(" ASC");
        }
        if(--len !=0) {
            sql.push_back(',');
        }
    }
}

php::value database_mysql::one(const php::parameter& param) {
    std::string   table  = param[0];
    php::value cond   = param[1];
    php::value order  = param[2];
    if(table.empty()) {
        zend_throw_error(nullptr, "table name is required");
        return php::null;
    }
    std::string sql = "SELECT * FROM `"+table+"`";
    php::property prop(this);
    php::value    _mysqli = prop.oget("_mysqli");
    if(!cond.is_empty()) {
        database_mysql_where where(_mysqli, sql);
        where.build(cond);
    }
    build_order(sql, order);
    sql.append(" LIMIT 1");
    php::value rs = php::call_method_1(&_mysqli, "query", sql);
    if(rs.is_type(IS_OBJECT)) {
        return php::call_method_0(&rs, "fetch_assoc");
    }else{
        return php::null;
    }
}

// 将其他所有调用转接到 mysqli
php::value database_mysql::__call(const std::string& name, const php::parameter& param) {
    std::vector<zval> params;
    for(size_t i=0;i<param.size;++i) {
        params.push_back(*param.get(i));
    }
    php::property prop(this);
    php::value    _mysqli = prop.oget("_mysqli");
    return php::call_method(&_mysqli, name, params);
}
bool database_mysql::__isset(const std::string& name) {
    php::property self(this);
    php::value    _mysqli = self.oget("_mysqli");
    php::property prop(_mysqli);
    bool isset = prop.__isset(name);
    return prop.__isset(name);
}
php::value database_mysql::__get(const std::string& name) {
    php::property self(this);
    php::value    _mysqli = self.oget("_mysqli");
    php::property prop(_mysqli);
    return prop.oget(name);
}


