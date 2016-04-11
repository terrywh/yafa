/* 
 * File:   database_mysql.h
 * Author: wuhao
 *
 * Created on 2015年11月19日, 上午10:41
 */

#ifndef DATABASE_MYSQL_H
#define DATABASE_MYSQL_H

class database_mysql:public php::class_base {
    private:
        static std::string    MASTER;
        static std::string    SLAVE;
        std::string           last_query;
    public:
        static std::string       class_name;
        static zend_class_entry* class_entry;
        
        static php::value init(const php::parameter& param);
        static php::value get_master(const php::parameter& param);
        static php::value get_slave(const php::parameter& param);
        database_mysql();
        void __construct(const php::parameter& param) override;
        ~database_mysql();
        php::value format(const php::parameter& param);
        php::value format_query(const php::parameter& param);
        php::value insert(const php::parameter& param);
        void insert_keys(php::value& _mysqli, std::string& sql, const php::value& item);
        void insert_vals(php::value& _mysqli, std::string& sql, const php::value& item);
        php::value remove(const php::parameter& param);
        php::value update(const php::parameter& param);
        php::value select(const php::parameter& param);
        php::value one(const php::parameter& param);
        
        php::value __call(const std::string& name, const php::parameter& param) override;
        bool __isset(const std::string& name) override;
        php::value __get(const std::string& name) override;

    private:
        static php::value create(php::value& config);
        static void build_order(std::string& sql, php::value& order);
        
};

#endif /* DATABASE_MYSQL_H */

