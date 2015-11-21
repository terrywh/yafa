#include "../easy.h"

namespace easy {
    // 类型转换
    value_t::operator double() {
        if(Z_TYPE(_value) != IS_DOUBLE) {
            convert_to_double(&_value);
        }
        return _value.value.dval;
        return 0;
    }
    value_t::operator zend_long() {
        if(Z_TYPE(_value) != IS_LONG) {
            convert_to_long(&_value);
        }
        return _value.value.lval;
    }
    value_t::operator bool() {
        convert_to_boolean(&_value);
        return Z_TYPE(_value) == IS_TRUE;
    }
    value_t::operator std::string() {
        if(zval_get_type(&_value) == IS_NULL) {
            return STRING_EMPTY;
        }
        
        if(zval_get_type(&_value) != IS_STRING) {
            convert_to_string(&_value);
        }
        return std::string(Z_STRVAL(_value), Z_STRLEN(_value));
    }
    value_t value_t::operator [](zend_ulong index) {
        if(!is_array()) {
            return VALUE_NULL;
        }
        return zend_hash_index_find(Z_ARR(_value), index);
    }
    value_t value_t::operator [](const char* key) {
        if(!is_array()) {
            return VALUE_NULL;
        }
        return zend_hash_str_find(Z_ARR(_value), key, strlen(key));
    }
    value_t value_t::operator [](const std::string& key) {
        if(!is_array()) {
            return VALUE_NULL;
        }
        return zend_hash_str_find(Z_ARR(_value), key.c_str(), key.length());
    }
    bool value_t::has(zend_ulong index) const {
        if(!is_array()) {
            return false;
        }
        return zend_hash_index_exists(Z_ARR(_value), index);
    }
    bool value_t::has(const std::string& key) const {
        if(!is_array()) {
            return false;
        }
        return zend_hash_str_exists(Z_ARR(_value), key.c_str(), key.length());
    }
    
    void value_t::set(zend_ulong index, const value_t& val) {
        if(!is_array()) {
            return;
        }
        zend_hash_index_update(Z_ARR(_value), index, const_cast<zval*>(&val._value));
    }
    void value_t::set(const std::string& key, const value_t& val) {
        if(!is_array()) {
            return;
        }
        zend_hash_str_update(Z_ARR(_value), key.c_str(), key.length(), const_cast<zval*>(&val._value));
    }
    
    
    value_t value_t::clone() const {
        value_t d;
        ZVAL_DUP(&d._value, &_value);
        return d;
    }
    
    value_t value_t::create_array() {
        value_t retval;
        zend_array* ALLOC_HASHTABLE(val);
        zend_hash_init(val, 8, nullptr, nullptr, 0);
        ZVAL_ARR(&retval._value, val);
        return val;
    }
}