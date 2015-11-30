#include "boost/algorithm/string.hpp"
#include "core.h"
#include "database_ssdb.h"

std::string       database_ssdb::class_name;
zend_class_entry* database_ssdb::class_entry;

std::map<std::string, bool> database_ssdb::without_prefix = {
    {"auth",true}
};

php::value database_ssdb::init(const php::parameter& param) {
    if(param.size < 1 || !param[0].is_type(IS_ARRAY)) {
        zend_throw_error(NULL, "ssdb config of type array is required");
        return false;
    }
    php::value conf = param[0];
    if(!conf.__isset("master") && !conf.__isset("slave")) {
        zend_throw_error(NULL, "illegal ssdb config #1");
        return false;
    }
    php::property prop(database_ssdb::class_entry);
    prop.sset("_conf", conf);
    return true;
}
php::value database_ssdb::get_master(const php::parameter& param) {
    zend_long  idx = 0;
    std::string key = "master_";
    if(param.size == 1) {
        idx = param[0];
    }
    key.append(std::to_string(idx));
    php::property      prop(database_ssdb::class_entry);
    php::value cache = prop.sget("_cache");
    php::value db    = cache[key];
    if(!db.is_type(IS_OBJECT)) {
        php::value conf = prop.sget("_conf");
        conf = conf["master"][idx];
        if(!conf.is_type(IS_ARRAY)) {
            zend_throw_error(NULL, "illegal ssdb config #2");
            return php::value();
        }
        db = database_ssdb::create(conf);
        cache[key] = db;
    }
    return db;
}
php::value database_ssdb::get_slave(const php::parameter& param) {
    zend_long  idx = 0;
    std::string key = "slave_";
    if(param.size == 1) {
        idx = param[0];
    }
    key.append(std::to_string(idx));
    php::property      prop(database_ssdb::class_entry);
    php::value cache = prop.sget("_cache");
    php::value db    = cache[key];
    if(!db.is_type(IS_OBJECT)) {
        php::value conf = prop.sget("_conf");
        conf = conf["slave"][idx];
        if(!conf.is_type(IS_ARRAY)) {
            zend_throw_error(NULL, "illegal ssdb config #2");
            return php::value();
        }
        db = database_ssdb::create(conf);
        cache[key] = db;
    }
    return db;
}
    
database_ssdb::database_ssdb() {
    
}

    
database_ssdb::~database_ssdb() {
    
}

php::value database_ssdb::create(php::value& config) {
    std::string host = config["host"];
    zend_long   port = config["port"];
    
    zval obj;
    object_init_ex(&obj, database_ssdb::class_entry);
    php::_store * p_store = php::get_store(&obj);
    php::parameter param(config.intern(), 1);
    p_store->self->__construct(param);
    return php::value(&obj);
}


void database_ssdb::__construct(const php::parameter& param) {
    php::value conf = param[0];
    std::string host = conf["host"];
    zend_long   port = conf["port"];
    if(host.empty() || port <= 0 || port > 65535) {
        zend_throw_error(nullptr, "illegal ssdb config #3");
        return;
    }
//    pssdb = redisConnect(host.c_str(), port);
//    if(pssdb == nullptr) {
//        zend_throw_error(nullptr, "redis failed to connect");
//        return;
//    }else if(pssdb->err) {
//        zend_throw_error(nullptr, "redis failed to connect [%d] %s", pssdb->err, pssdb->errstr);
//        __destruct();
//        return;
//    }
    pssdb = ssdb::Client::connect(host, port);
    if(pssdb == nullptr) {
        zend_throw_error(nullptr, "ssd failed to connect ssdb");
        return;
    }
    if(conf.__isset("auth")) {
        php::parameter auth(conf["auth"].intern(), 1);
        __call("auth", auth);
    }
    prefix = (std::string)conf["prefix"];
    // 保存一个属性，方便客户端调用
    php::property prop(this);
    prop.oset("prefix", prefix);
}

void database_ssdb::__destruct() {
    if(pssdb != nullptr) {
        delete pssdb;
    }
}


php::value database_ssdb::__call(const std::string& name, const php::parameter& param) {
    std::vector<std::string>  argv;
    // COMMAND
    std::string uname = boost::algorithm::to_lower_copy(name);
    argv.push_back(uname);
    int i = 0;
    // FIRST ARG
    if(param.size > 0 && database_ssdb::without_prefix.find(uname) == database_ssdb::without_prefix.end()) {
        std::string key = prefix;
        key.append((std::string)param[i]);
        argv.push_back(key);
        ++i;
    }
    for(;i<param.size;++i) {
        zval* pv = param.get(i);
        if(Z_TYPE_P(pv) != IS_STRING) {
            convert_to_string(pv);
        }
        if(Z_STRLEN_P(pv) == 0) {
            argv.push_back("");
        }else{
            argv.push_back(std::string(Z_STRVAL_P(pv), Z_STRLEN_P(pv)));
        }
    }
    php_printf("%s => %s\n", argv[0].c_str(), argv[1].c_str());
    const std::vector<std::string>* reply = pssdb->request(argv);
    if(reply == nullptr) {
        zend_throw_error(nullptr, "redis command \"%s\" failed #1", uname.c_str());
        return php::value();
    }

    if(reply->front().compare("ok") != 0) {
        zend_throw_error(nullptr, "redis command \"%s\" failed #2: %s %s", uname.c_str(), reply->front().c_str(), reply->back().c_str());
        return php::value();
    }
    
    if(uname.compare("hgetall") == 0) {
        return php::value();
    }else if(reply->size() == 2) {
        php::value r = reply->at(1);
        return std::move(r);
    }else{
        php::value r;
        for(int i=0;i<reply->size();++i) {
            r[i] = reply->at(i);
        }
        return std::move(r);
    }
}


//php::value database_ssdb::parse(redisReply* reply, bool with_key) {
//    php::value r;
//    if(reply->type == REDIS_REPLY_STATUS) {
//        r = reply->str[0] == '+';
//    }else if(reply->type == REDIS_REPLY_INTEGER) {
//        r = reply->integer;
//    }else if(reply->type == REDIS_REPLY_NIL) {
//        
//    }else if(reply->type == REDIS_REPLY_STRING) {
//        r = std::string(reply->str, reply->len);
//    }else if(reply->type == REDIS_REPLY_ARRAY) {
//        std::string key;
//        for(size_t i=0;i<reply->elements;i++) {
//            if(with_key) { // 特殊情况需要设置键值
//                key = (std::string)parse(reply->element[i]);
//                ++i;
//                r[key] = parse(reply->element[i]);
//            }else{
//                r[i] =  parse(reply->element[i]);
//            }
//        }
//    }else {
//        zend_throw_error(nullptr, "ssdb command failed #3");
//    }
//    return std::move(r);
//}