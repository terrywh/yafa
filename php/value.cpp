/* 
 * File:   value.cpp
 * Author: wuhao
 * 
 * Created on 2015年11月29日, 下午1:30
 */
#include "../core.h"

namespace php {
    // -------------------------------------------------------------------------
    // 静态常量
    // -------------------------------------------------------------------------
    // 基本构造
    value::value():own(true) {
        ZVAL_NULL(&val);
    }
    
    value::value(zval* p, bool o):own(o) {
        if(own) {
            if(Z_REFCOUNTED_P(p)) {
                ZVAL_COPY(&val, p);
            }else{
                val = *p;
            }
        }else{
            val = *p;
        }
    }
    value::value(const value& orig):own(true) {
        if(Z_REFCOUNTED(orig.val)) {
            ZVAL_COPY(&val, &orig.val);
        }else{
            val = orig.val;
        }
        
    }
    
    value::value(value&& orig) {
        val = orig.val;
        own = orig.own;
        ZVAL_NULL(&orig.val);
        orig.own = false;
        
        parent = orig.parent;
        key_type = orig.key_type;
        if(key_type) {
            key_idx = orig.key_idx;
        }else{
            key_str = orig.key_str;
        }
        orig.parent = nullptr;
    }

    value::~value() {
        if(own) {
            zval_ptr_dtor(&val);
        }
    }
    // -------------------------------------------------------------------------
    value::value(const char* str) {
        own = true;
        ZVAL_STR(&val, zend_string_init(str, strlen(str), 0));
    }
    value::value(const std::string& str) {
        own = true;
        ZVAL_STR(&val, zend_string_init(str.c_str(), str.length(), 0));
    }
    value::value(const long& l) {
        own = true;
        ZVAL_LONG(&val, l);
    }
    value::value(const int& i) {
        own = true;
        ZVAL_LONG(&val, i);
    }
    value::value(const bool& b) {
        own = true;
        ZVAL_BOOL(&val, b);
    }
    value::value(const double& d) {
        own = true;
        ZVAL_DOUBLE(&val, d);
    }
    // -------------------------------------------------------------------------
    // 赋值
    value& value::operator=(const value& orig) {
        clear();
        own = true;
        if(Z_REFCOUNTED(orig.val)) {
            ZVAL_COPY(&val, &orig.val);
        }else{
            val = orig.val;
        }
        update_parent();
        return *this;
    }
    value& value::operator=(const char* str) {
        clear();
        own = true;
        ZVAL_STR(&val, zend_string_init(str, strlen(str), 0));
        update_parent();
        return *this;
    }
    value& value::operator=(const std::string& str) {
        clear();
        own = true;
        ZVAL_STR(&val, zend_string_init(str.c_str(), str.length(), 0));
        update_parent();
        return *this;
    }
    value& value::operator=(const long& l) {
        clear();
        own = true;
        ZVAL_LONG(&val, l);
        update_parent();
        return *this;
    }
    value& value::operator=(const long long& l) {
        clear();
        own = true;
        ZVAL_LONG(&val, l);
        update_parent();
        return *this;
    }
    value& value::operator=(const bool& b) {
        clear();
        own = true;
        ZVAL_BOOL(&val, b);
        update_parent();
        return *this;
    }
    value& value::operator=(const zval* v) {
        clear();
        own = true;
        ZVAL_ZVAL(&val, const_cast<zval*>(v), 1, 0);
        return *this;
    }
    value& value::operator=(const double& d) {
        clear();
        own = true;
        ZVAL_DOUBLE(&val, d);
        return *this;
    }
    // ---------------------------------------------------------------------
    // 数组
    value value::operator[](int idx) {
        if(!is_type(IS_ARRAY)){
            value v;
            v.key_idx = idx;
            v.key_type = true;
            v.parent = this;
            return v;
        }
        zval* p = zend_hash_index_find(Z_ARR(val), idx);
        if(p == nullptr) { // 不存在的元素
            value v;
            v.key_idx = idx;
            v.key_type = true;
            v.parent = this;
            return v;
        }
        return value(p, false); // 仅处理，不做内存
    }
    value value::operator[](const char* key) {
        if(!is_type(IS_ARRAY)){
            value v;
            v.key_str = key;
            v.key_type = false;
            v.parent = this;
            return v;
        }
        zval* p = zend_hash_str_find(Z_ARR(val), key, strlen(key));
        if(p == nullptr) { // 存在的元素
            value v;
            v.key_str  = key;
            v.key_type = false;
            v.parent   = this;
            return v;
        }
        return value(p, false); // 仅处理，不做内存
    }
    value value::operator[](const std::string& key) {
        if(!is_type(IS_ARRAY)){
            value v;
            v.key_str = key;
            v.key_type = false;
            v.parent = this;
            return v;
        }
        zval* p = zend_hash_str_find(Z_ARR(val), key.c_str(), key.length());
        if(p == nullptr) { // 存在的元素
            value v;
            v.key_str = key;
            v.key_type = false;
            v.parent = this;
            return v;
        }
        return value(p, false); // 仅处理，不做内存
    }
    
    void value::update_parent() {
        if(parent == nullptr) {
            return;
        }
        if(!parent->is_type(IS_ARRAY)) {
            ZVAL_NEW_ARR(&parent->val);
            zend_hash_init(Z_ARR(parent->val), 4, nullptr, nullptr, 0);
            parent->update_parent();
        }
        if(Z_REFCOUNTED(val)) {
            Z_ADDREF(val);
        }
        if(key_type) {
            zend_hash_index_update(Z_ARR(parent->val), key_idx, &val);
        }else{
            zend_hash_str_update(Z_ARR(parent->val), key_str.c_str(), key_str.length(), &val);
        }
        
    }
    
    bool value::__isset(int idx) const {
        if(!is_type(IS_ARRAY)){
            return false;
        }
        return zend_hash_index_exists(Z_ARR(val), idx);
    }
    
    bool value::__isset(const std::string& key) const {
        if(!is_type(IS_ARRAY)){
            return false;
        }
        return zend_hash_str_exists(Z_ARR(val), key.c_str(), key.length());
    }
    size_t value::length() const {
        if(is_type(IS_ARRAY)){
            return zend_hash_num_elements(Z_ARR(val));
        }else if(is_type(IS_STRING)) {
            return Z_STRLEN(val);
        }
        return 0;
    }
    value_iterator value::begin() const {
        return value_iterator(Z_ARR(val), true);
    }
    value_iterator value::end() const {
        return value_iterator(Z_ARR(val), false);
    }
    // -------------------------------------------------------------------------
    // 类型转换
    value::operator std::string() {
        if(!is_type(IS_STRING)) {
            convert_to_string(const_cast<zval*>(&val));
        }
        return std::string(Z_STRVAL(val), Z_STRLEN(val));
    }
    value::operator zend_long() {
        if(!is_type(IS_LONG)) {
            convert_to_long(const_cast<zval*>(&val));
        }
        return Z_LVAL(val);
    }
}
