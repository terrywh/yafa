/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   database_mysql.h
 * Author: wuhao
 *
 * Created on 2015年11月19日, 上午10:41
 */

#ifndef DATABASE_MYSQL_H
#define DATABASE_MYSQL_H

class database_mysql:public easy::class_base {
    private:
        easy::value_t        _mysqli;
        static std::string    MASTER;
        static std::string    SLAVE;
    public:
        // static std::string CLASS_NAME;
        static easy::value_t init(easy::param_t& param);
        static easy::value_t get_master(easy::param_t& param);
        static easy::value_t get_slave(easy::param_t& param);
        database_mysql();
        void __construct(easy::param_t& param) override;
        ~database_mysql();
        easy::value_t format(easy::param_t& param);
        easy::value_t format_query(easy::param_t& param);
        easy::value_t __call(const std::string& name, easy::param_t& param) override;
        bool __isset(const std::string& name) override;
        easy::value_t __get(const std::string& name) override;

    private:
        static easy::value_t create(easy::value_t& config);
        
};

#endif /* DATABASE_MYSQL_H */

