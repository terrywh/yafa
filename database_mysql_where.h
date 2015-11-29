/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   database_mysql_where.h
 * Author: wuhao
 *
 * Created on 2015年11月22日, 下午2:11
 */

#ifndef DATABASE_MYSQL_WHERE_H
#define DATABASE_MYSQL_WHERE_H

class database_mysql_where {
public:
    database_mysql_where(php::value& mysqli, std::string& sql);
    virtual ~database_mysql_where();
    
    void build(php::value& c);
private:
    void append_item(const std::string& key, php::value& val);
    void append_val(php::value& val);
    php::value& _mysqli;
    std::string& _sql;
};

#endif /* DATABASE_MYSQL_WHERE_H */

