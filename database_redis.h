/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef DATABASE_REDIS_H
#define DATABASE_REDIS_H

class database_redis: public php::class_base {
public:
    static zend_class_entry* class_entry;
    static std::string       class_name;
    static php::value init(const php::parameter& param);
    static php::value get_master(const php::parameter& param);
    static php::value get_slave(const php::parameter& param);

    
private:
    static php::value create(php::value& config);
    bool              transaction = false;
};

#endif /* DATABASE_REDIS_H */

