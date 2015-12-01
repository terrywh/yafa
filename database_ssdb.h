/* 
 * File:   database_ssdb.h
 * Author: wuhao
 *
 * Created on 2015年11月30日, 下午2:39
 */

#ifndef DATABASE_SSDB_H
#define DATABASE_SSDB_H

#include "deps/SSDB_client.h"

enum database_ssdb_flags: int {
    FLAG_PREFIX_NONE        = 1,
    FLAG_PREFIX_MULTI_JUMP  = 2, // key val key val 间隔形式
    FLAG_PREFIX_MULTI_ALL   = 4,
    FLAG_RETURN_LONG        = 8,
    FLAG_RETURN_DBL         = 16,
    FLAG_RETURN_BOOL        = 32,
    FLAG_RETURN_KV          = 64,
};

class database_ssdb: public php::class_base {
public:
    static zend_class_entry* class_entry;
    static std::string       class_name;
    
    static php::value init(const php::parameter& param);
    static php::value get_master(const php::parameter& param);
    static php::value get_slave(const php::parameter& param);
    
    database_ssdb();
    virtual void __construct(const php::parameter& param) override;
    virtual void __destruct() override;
    virtual php::value __call(const std::string& name, const php::parameter& param) override;
    virtual ~database_ssdb();
private:
    static php::value create(php::value& config);
    static php::value parse(const std::vector<std::string>* reply, int flag);
    static php::value parse_item(const std::string& r, int flag);
    static std::map<std::string, int> cmd_flag;
    ssdb::Client* pssdb = nullptr;
    std::string   prefix;
};

#endif /* DATABASE_SSDB_H */

