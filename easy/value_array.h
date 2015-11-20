/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   value_array.h
 * Author: wuhao
 *
 * Created on 2015年11月20日, 下午1:31
 */

#ifndef EASY_VALUE_ARRAY_H
#define EASY_VALUE_ARRAY_H

#include "value.h"


namespace easy {
    class value_array {
    private:
        zval _value;
    public:
        value_array() {
            // ZVAL_NEW_ARR(_val);
            _array_init(&_value, 8);
        }
//        value_array(zval* val) {
//            ZVAL_COPY(&_val, val);
//        }
        value_array(const value_t& val) {
            ZVAL_COPY(&_value, &val._value);
        }
        value_array(const value_array& a) {
            ZVAL_COPY(&_value, &a._value);
        }
        ~value_array() {
            zval_ptr_dtor(&_value);
        }
        value_t operator [](zend_ulong index) {
            return zend_hash_index_find(Z_ARR(_value), index);
        }
        value_t operator [](const std::string& key) {
            return zend_hash_str_find(Z_ARR(_value), key.c_str(), key.length());
        }
        bool has(const std::string& key) const;
        bool has(zend_ulong index) const;
        zend_ulong length() const {
            return zend_hash_num_elements(Z_ARR(_value));
        };
        friend class param_t;
    };
}
#endif /* EASY_VALUE_ARRAY_H */


