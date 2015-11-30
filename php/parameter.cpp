/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   parameter.cpp
 * Author: wuhao
 * 
 * Created on 2015年11月29日, 下午2:33
 */

#include "../core.h"

namespace php {
parameter::parameter(zval* p, size_t s):p_zval(p),size(s),b_array(false) {
}
parameter::parameter(zval* p):p_zval(p),b_array(true) {
    if(Z_TYPE_P(p) == IS_ARRAY) {
        size = zend_hash_num_elements(Z_ARR_P(p));
    }else{
        p_zval = nullptr;
        size   = 0;
        zend_throw_error(nullptr, "parameter must be of type array");
    }
}

value parameter::operator [](size_t idx) const {
    if(b_array && p_zval != nullptr) {
        zval* v = zend_hash_index_find(Z_ARR_P(p_zval), idx);
        if(v == nullptr) {
            return value();
        }else{
            return value(v, false);
        }
    }

    if(p_zval != nullptr && idx < size) {
        value v(p_zval + idx, false); // value 对象并不持有对应的 zval*
        return std::move(v);
    }
    
    return value();
}

zval* parameter::get(size_t idx) const {
    if(b_array) {
        return zend_hash_index_find(Z_ARR_P(p_zval), idx);
    }
    if(p_zval != nullptr && idx < size) {
        return p_zval + idx;
    }
    
    return nullptr;
}


}
