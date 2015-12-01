#include "boost/algorithm/string.hpp"
#include "core.h"
#include "database_ssdb.h"

std::string       database_ssdb::class_name;
zend_class_entry* database_ssdb::class_entry;

std::map<std::string, int> database_ssdb::cmd_flag = {
    // ----------------------------------------------------------------------------------
    {"auth"        , 0 | FLAG_PREFIX_NONE                       | FLAG_RETURN_BOOL } ,
    {"flushdb"     , 0 | FLAG_PREFIX_NONE                                          } ,
    {"info"        , 0 | FLAG_PREFIX_NONE                                          } ,
    {"dbsize"      , 0 |                       FLAG_RETURN_LONG                    } ,
    {"ping"        , 0 |                       FLAG_RETURN_LONG                    } ,
    // -----------------------------------------------            ----------------------- 
    {"get"         , 0                                                              },
    {"set"         , 0 |                                          FLAG_RETURN_LONG  },
    {"setx"        , 0 |                                          FLAG_RETURN_LONG  },
    {"setnx"       , 0 |                                          FLAG_RETURN_LONG  },
    {"expire"      , 0 |                                          FLAG_RETURN_LONG  },
    {"ttl"         , 0 |                                          FLAG_RETURN_LONG  },
    {"del"         , 0 |                                          FLAG_RETURN_LONG  },
    {"incr"        , 0 |                                          FLAG_RETURN_LONG  },
    {"decr"        , 0 |                                          FLAG_RETURN_LONG  },
    {"exists"      , 0 |                                          FLAG_RETURN_BOOL  },
    {"getbit"      , 0 |                                          FLAG_RETURN_LONG  },
    {"setbit"      , 0 |                                          FLAG_RETURN_LONG  },
    {"countbit"    , 0 |                                          FLAG_RETURN_LONG  },
    {"bitcount"    , 0 |                                          FLAG_RETURN_LONG  },
    {"strlen"      , 0 |                                          FLAG_RETURN_LONG  },
    {"keys"        , 0 | FLAG_PREFIX_NONE                                           },
    {"rkeys"       , 0 | FLAG_PREFIX_NONE                                           },
    {"scan"        , 0 | FLAG_PREFIX_NONE                       | FLAG_RETURN_KV    },
    {"rscan"       , 0 | FLAG_PREFIX_NONE                       | FLAG_RETURN_KV    },
    {"multi_exists", 0 | FLAG_PREFIX_MULTI_ALL | FLAG_RETURN_KV | FLAG_RETURN_BOOL  },
    {"multi_set"   , 0 | FLAG_PREFIX_MULTI_JUMP                 | FLAG_RETURN_LONG  },
    {"multi_get"   , 0 | FLAG_PREFIX_MULTI_ALL                  | FLAG_RETURN_KV    },
    {"multi_del"   , 0 | FLAG_PREFIX_MULTI_ALL                  | FLAG_RETURN_LONG  },
    // --------------------------------------------------------------------------------
    {"hset"        , 0 |                                          FLAG_RETURN_LONG  },
    {"hget"        , 0                                                              },
    {"hdel"        , 0 |                                          FLAG_RETURN_LONG  },
    {"hincr"       , 0 |                                          FLAG_RETURN_LONG  },
    {"hdecr"       , 0 |                                          FLAG_RETURN_LONG  },
    {"hexists"     , 0 |                                          FLAG_RETURN_BOOL  },
    {"hsize"       , 0 |                                          FLAG_RETURN_LONG  },
    {"hlist"       , 0 | FLAG_PREFIX_NONE                                           },
    {"hrlist"      , 0 | FLAG_PREFIX_NONE                                           },
    {"hkeys"       , 0                                                              },
    {"hgetall"     , 0 |                                          FLAG_RETURN_KV    },
    {"hscan"       , 0 | FLAG_PREFIX_NONE                       | FLAG_RETURN_KV    },
    {"hrscan"      , 0 | FLAG_PREFIX_NONE                       | FLAG_RETURN_KV    },
    {"hclear"      , 0 |                                          FLAG_RETURN_LONG  },
    {"multi_hexists",0 |                         FLAG_RETURN_KV | FLAG_RETURN_BOOL  },
    {"multi_hset"  , 0 |                                          FLAG_RETURN_LONG  },
    {"multi_hget"  , 0 |                                          FLAG_RETURN_KV    },
    {"multi_hdel"  , 0 |                                          FLAG_RETURN_LONG  },
    {"multi_hsize" , 0 |                                          FLAG_RETURN_KV    },
    // -------------------------------------------------------------------------------- 
    {"zset"        , 0 |                                          FLAG_RETURN_LONG  },
    {"zget"        , 0 |                                          FLAG_RETURN_LONG  },
    {"zdel"        , 0 |                                          FLAG_RETURN_LONG  },
    {"zincr"       , 0 |                                          FLAG_RETURN_LONG  },
    {"zdecr"       , 0 |                                          FLAG_RETURN_LONG  },
    {"zexists"     , 0 |                                          FLAG_RETURN_BOOL  },
    {"zsize"       , 0 |                                          FLAG_RETURN_LONG  },
    {"zlist"       , 0 | FLAG_PREFIX_NONE                                           },
    {"zrlist"      , 0 | FLAG_PREFIX_NONE                                           },
    {"zkeys"       , 0                                                              },
    {"zscan"       , 0 |                          FLAG_RETURN_KV | FLAG_RETURN_LONG },
    {"zrscan"      , 0 |                          FLAG_RETURN_KV | FLAG_RETURN_LONG },
    {"zrank"       , 0 |                                          FLAG_RETURN_LONG  },
    {"zrrank"      , 0 |                                          FLAG_RETURN_LONG  },
    {"zrange"      , 0 |                          FLAG_RETURN_KV | FLAG_RETURN_LONG },
    {"zrrange"     , 0 |                          FLAG_RETURN_KV | FLAG_RETURN_LONG },
    {"zclear"      , 0 |                                          FLAG_RETURN_LONG  },
    {"zcount"      , 0 |                                          FLAG_RETURN_LONG  },
    {"zsum"        , 0 |                                          FLAG_RETURN_LONG  },
    {"zavg"        , 0 |                                          FLAG_RETURN_DBL   },
    {"zremrangebyrank"  ,                                         FLAG_RETURN_LONG  },
    {"zremrangebyscore" ,                                         FLAG_RETURN_LONG  },
    {"zpop_front"  , 0 |                          FLAG_RETURN_KV | FLAG_RETURN_LONG },
    {"zpop_back"   , 0 |                          FLAG_RETURN_KV | FLAG_RETURN_LONG },
    {"multi_zexists",0 |                          FLAG_RETURN_KV | FLAG_RETURN_BOOL },
    {"multi_zset"  , 0 |                                          FLAG_RETURN_LONG  },
    {"multi_zget"  , 0 |                                          FLAG_RETURN_KV    },
    {"multi_zdel"  , 0 |                                          FLAG_RETURN_LONG  },
    {"multi_zsize" , 0 |                                          FLAG_RETURN_KV    },
    // ------------------------------------------------------------------------------- 
    {"qpush_front" , 0                                                              },
    {"qpush_back"  , 0                                                              },
    {"qpop_front"  , 0 |                                          FLAG_RETURN_LONG  },
    {"qpop_back"   , 0 |                                          FLAG_RETURN_LONG  },
    {"qpush"       , 0 |                                          FLAG_RETURN_LONG  },
    {"qpop"        , 0                                                              },
    {"qfront"      , 0                                                              },
    {"qback"       , 0                                                              },
    {"qsize"       , 0 |                                          FLAG_RETURN_LONG  },
    {"qclear"      , 0 |                                          FLAG_RETURN_LONG  },
    {"qget"        , 0                                                              },
    {"qset"        , 0 |                                          FLAG_RETURN_LONG  },
    {"qrange"      , 0                                                              },
    {"qslice"      , 0                                                              },
    {"qtrim_front" , 0 |                                          FLAG_RETURN_LONG  },
    {"qtrim_back"  , 0 |                                          FLAG_RETURN_LONG  },
    {"qlist"       , 0 | FLAG_PREFIX_NONE                                           },
    {"qrlist"      , 0 | FLAG_PREFIX_NONE                                           },
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
    int timeout = 2000;
    if(conf.__isset("timeout")) {
        timeout = (long)conf["timeout"];
    }
    pssdb = ssdb::Client::connect(host, port, timeout);
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
    // 找到命令
    auto f = cmd_flag.find(uname);
    if(f == cmd_flag.end()) { // 没有找到对应的方法
        zend_throw_error(nullptr, "redis command \"%s\" not found", uname.c_str());
        return php::value();
    }
    argv.push_back(uname);
    // 前缀处理
    int flag = f->second;
    if(prefix.empty() || flag & FLAG_PREFIX_NONE) { // 不处理
        for(size_t i=0;i<param.size;++i) {
            zval* v = param.get(i);
            convert_to_string(v);
            argv.push_back(std::string(Z_STRVAL_P(v), Z_STRLEN_P(v)));
        }
    }else if(flag & FLAG_PREFIX_MULTI_ALL) { // 所有参数都处理
        for(size_t i=0;i<param.size;i++) {
            zval* v = param.get(i);
            convert_to_string(v);
            std::string key = prefix;
            key.append(std::string(Z_STRVAL_P(v), Z_STRLEN_P(v)));
            argv.push_back(key);
        }
    }else if(flag & FLAG_PREFIX_MULTI_JUMP) { // 间隔处理
        for(size_t i=0;i<param.size;i++) {
            zval* v = param.get(i);
            convert_to_string(v);
            if(i%2 == 1) {
                argv.push_back(std::string(Z_STRVAL_P(v), Z_STRLEN_P(v)));
            }else{
                std::string key = prefix;
                key.append(std::string(Z_STRVAL_P(v), Z_STRLEN_P(v)));
                argv.push_back(key);
            }
        }
    }else{ // 首个参数处理
        zval* v = param.get(0);
        convert_to_string(v);
        std::string key = prefix;
        key.append(std::string(Z_STRVAL_P(v), Z_STRLEN_P(v)));
        argv.push_back(key);
        for(size_t i=1;i<param.size;++i) {
            zval* v = param.get(i);
            convert_to_string(v);
            argv.push_back(std::string(Z_STRVAL_P(v), Z_STRLEN_P(v)));
        }
    }
    const std::vector<std::string>* reply = pssdb->request(argv);
    if(reply == nullptr || reply->size() < 1) {
        zend_throw_error(nullptr, "ssdb command \"%s\" failed #1", uname.c_str());
        return php::value();
    }
    ssdb::Status status(reply);
    if(status.ok()) {
        // 返回值处理
        return database_ssdb::parse(reply, flag);
    }else if(status.not_found()) {
        return php::value();
    }else if(reply->size() > 1){
        zend_throw_error(nullptr, "ssdb command \"%s\" faild #2: %s", uname.c_str(), reply->at(1).c_str());
        return php::value();
    }else{
        zend_throw_error(nullptr, "ssdb command \"%s\" faild #3", uname.c_str());
        return php::value();
    }
}


php::value database_ssdb::parse(const std::vector<std::string>* reply, int flag) {
    php::value v;
    if(flag & FLAG_RETURN_KV) {
        for(int i=1;i<reply->size();i+=2) {
            v[reply->at(i)] = database_ssdb::parse_item(reply->at(i+1), flag);
        }
    }else{
        int k = 0;
        for(int i=1;i<reply->size();i++) {
            v[k] = database_ssdb::parse_item(reply->at(i), flag);
        }
    }
    return std::move(v);
}

php::value database_ssdb::parse_item(const std::string& r, int flag) {
    php::value v;
    if(flag & FLAG_RETURN_BOOL) {
        v = r.empty() || r == "0" ? false : true;
    }else if(flag & FLAG_RETURN_LONG) {
        v = std::stol(r);
    }else if(flag & FLAG_RETURN_DBL) {
        v = std::stod(r);
    }else{ // 按文本处理
        v = r;
    }
    return std::move(v);
}