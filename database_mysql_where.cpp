/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   database_mysql_where.cpp
 * Author: wuhao
 * 
 * Created on 2015年11月22日, 下午2:11
 */
#include "core.h"
#include "database_mysql_where.h"

database_mysql_where::database_mysql_where(php::value& mysqli, std::string& sql):_mysqli(mysqli), _sql(sql) {
}

database_mysql_where::~database_mysql_where() {
}

void database_mysql_where::build(php::value& c) {
    _sql.append(" WHERE ");
    if(c.is_type(IS_STRING)) {
        _sql.append((std::string)c);
        return;
    }else if(!c.is_type(IS_ARRAY)) {
        zend_throw_error(nullptr, "conditions must be string or array");
        return;
    }
    size_t len = c.length();
    for(auto i=c.begin(); i!= c.end(); ++i) {
        std::string key(ZSTR_VAL(i->key), ZSTR_LEN(i->key));
        php::value  val(i->val, false);
        append_item(key, val);
        if(--len != 0) { // 最顶层默认按照 AND 处理
            _sql.append(" AND ");
        }
    }
}

void database_mysql_where::append_item(const std::string& key, php::value& val) {
    if(key[key.length()-1] == ']') {
        size_t pos = key.find_first_of('[');
        std::string op = key.substr(pos+1, key.length()-pos-2);
        _sql.push_back('`');
        _sql.append(key.substr(0, pos));
        _sql.push_back('`');
        if(op == "!") {
            if(val.is_type(IS_NULL)) {
                _sql.append(" IS NOT ");
            }else if(val.is_type(IS_ARRAY)) {
                _sql.append(" NOT IN ");
            }else{
                _sql.append("!=");
            }
        }else if(op == ">" || op == "<" || op == ">=" || op == "<=") {
            _sql.append(op);
        }else if(op == "<>") {
            _sql.append(" BETWEEN ");
            php::value v0 = val[0];
            php::value v1 = val[1];
            append_val(v0);
            _sql.append(" AND ");
            append_val(v1);
            return;
        }else if(op == "><") {
            _sql.append(" NOT BETWEEN ");
            php::value v0 = val[0];
            php::value v1 = val[1];
            append_val(v0);
            _sql.append(" AND ");
            append_val(v1);
            return;
        }else if(op == "~") {
            _sql.append(" LIKE ");
        }else if(op == "!~") {
            _sql.append(" NOT LIKE ");
        }else{
            zend_throw_error(nullptr,"operation \"%s\" not suppored", op.c_str());
            return;
        }
        append_val(val);
    }else if(key == "AND") {
        _sql.push_back('(');
        unsigned long length = val.length();
        for(auto p=val.begin(); p!= val.end(); ++p) {
            std::string key( ZSTR_VAL(p->key), ZSTR_LEN(p->key) );
            php::value  val(p->val, false);
            append_item(key, val);
            if(--length != 0) {
                _sql.append(" AND ");
            }
        }
        _sql.push_back(')');
    }else if(key == "OR") {
        _sql.push_back('(');
        unsigned long length = val.length();
        for(auto p=val.begin(); p!= val.end(); ++p) {
            std::string key( ZSTR_VAL(p->key), ZSTR_LEN(p->key) );
            php::value  val(p->val, false);
            append_item(key, val);
            if(--length != 0) {
                _sql.append(" OR ");
            }
        }
        _sql.push_back(')');
    }else{ // `key`
        _sql.push_back('`');
        _sql.append(key);
        _sql.push_back('`');
        if(val.is_type(IS_NULL)) { // IS NULL
            _sql.append(" IS ");
        }else if(val.is_type(IS_ARRAY)) {
            _sql.append(" IN ");
        }else{
            _sql.push_back('=');
        }
        append_val(val);
    }
}

void database_mysql_where::append_val(php::value& val) {
    if(val.is_type(IS_NULL)) { // IS NULL
        _sql.append("NULL");
    }else if(val.is_type(IS_ARRAY)) { // ('1','2','3')
        _sql.push_back('(');
        unsigned int length = val.length();
        for(auto i=val.begin(); i!= val.end(); ++i) {
            php::value val(i->val, false);
            append_val(val);
            if(--length != 0) {
                _sql.push_back(',');
            }
        }
        _sql.push_back(')');
    }else if(val.is_type(IS_TRUE)) {
        _sql.append("true");
    }else if(val.is_type(IS_FALSE)) {
        _sql.append("false");
    }else if(val.is_type(IS_LONG)) {
        _sql.append(std::to_string((zend_long)val));
    }else{ // 'val'
        _sql.push_back('\'');
        _sql.append((std::string)php::call_method_1(&_mysqli, "escape_string", val));
        _sql.push_back('\'');
    }
}

