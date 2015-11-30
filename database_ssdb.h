/* 
 * File:   database_ssdb.h
 * Author: wuhao
 *
 * Created on 2015年11月30日, 下午2:39
 */

#ifndef DATABASE_SSDB_H
#define DATABASE_SSDB_H

#include "deps/SSDB_client.h"


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
//    static php::value parse(redisReply* reply, bool with_key = false);
    static std::map<std::string, bool> without_prefix;
    ssdb::Client* pssdb = nullptr;
    std::string   prefix;
};

#endif /* DATABASE_SSDB_H */

