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
#include "easy.h"
#include "database_mysql_where.h"

database_mysql_where::database_mysql_where(const easy::value_t& mysqli, std::string& sql):_mysqli(mysqli), _sql(sql) {
}

database_mysql_where::~database_mysql_where() {
}

void database_mysql_where::build(easy::value_t& c) {
    _sql.append(" WHERE ");
    if(c.is_string()) {
        _sql.append((std::string)c);
        return;
    }else if(!c.is_array()) {
        zend_throw_error(nullptr, "only string and array conditions is supported");
        return;
    }
    unsigned long len = c.length();
    for(auto i=c.begin();c.has_more(i);c.next(i)) {
        std::string   key = easy::value_t(i.key);
        easy::value_t val = i.val;
        append_item(key, val);
        if(--len != 0) { // 最顶层默认按照 AND 处理
            _sql.append(" AND ");
        }
    }
}

void database_mysql_where::append_item(const std::string& key, easy::value_t& val) {
    if(key[key.length()-1] == ']') {
        size_t pos = key.find_first_of('[');
        std::string op = key.substr(pos+1, key.length()-pos-2);
        _sql.push_back('`');
        _sql.append(key.substr(0, pos));
        _sql.push_back('`');
        if(op == "!") {
            if(val.is_null()) {
                _sql.append(" IS NOT ");
            }else if(val.is_array()) {
                _sql.append(" NOT IN ");
            }else{
                _sql.append("!=");
            }
        }else if(op == ">" || op == "<" || op == ">=" || op == "<=") {
            _sql.append(op);
        }else if(op == "<>") {
            _sql.append(" BETWEEN ");
            append_val(val[0]);
            _sql.append(" AND ");
            append_val(val[1]);
            return;
        }else if(op == "><") {
            _sql.append(" NOT BETWEEN ");
            append_val(val[0]);
            _sql.append(" AND ");
            append_val(val[1]);
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
        for(auto p=val.begin();val.has_more(p);val.next(p)) {
            std::string   key(p.key->val, p.key->val);
            easy::value_t val = p.val;
            append_item(key, val);
            if(--length != 0) {
                _sql.append(" AND ");
            }
        }
        _sql.push_back(')');
    }else if(key == "OR") {
        _sql.push_back('(');
        unsigned long length = val.length();
        for(auto p=val.begin();val.has_more(p);val.next(p)) {
            std::string   key(p.key->val, p.key->len);
            easy::value_t val = p.val;
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
        if(val.is_null()) { // IS NULL
            _sql.append(" IS ");
        }else if(val.is_array()) {
            _sql.append(" IN ");
        }else{
            _sql.push_back('=');
        }
        append_val(val);
    }
}

void database_mysql_where::append_val(const easy::value_t& val) {
    if(val.is_null()) { // IS NULL
        _sql.append("NULL");
    }else if(val.is_array()) { // ('1','2','3')
        _sql.push_back('(');
        unsigned int length = val.length();
        for(auto i=val.begin();val.has_more(i);val.next(i)) {
            append_val(i.val);
            if(--length != 0) {
                _sql.push_back(',');
            }
        }
        _sql.push_back(')');
    }else if(val.is_true()) {
        _sql.append("true");
    }else if(val.is_false()) {
        _sql.append("false");
    }else if(val.is_long()) {
        _sql.append(val.c_str());
    }else{ // 'val'
        _sql.push_back('\'');
        _sql.append((std::string)easy::call_method_1(&_mysqli, "escape_string", val));
        _sql.push_back('\'');
    }
}

