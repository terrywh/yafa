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
    char* value_t::c_str() const {
        if(zval_get_type(&_value) == IS_NULL) {
            return nullptr;
        }
        if(zval_get_type(&_value) != IS_STRING) {
            convert_to_string(const_cast<zval*>(&_value));
        }
        return Z_STRVAL(_value);
    }
    
    value_t value_t::operator [](int index) {
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
    bool value_t::has(int index) const {
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
    
    void value_t::set(int index, const value_t& val) {
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
    
    value_t::iterator value_t::begin() const {
        value_t::iterator i;
        if(!is_array()) {
            zend_throw_error(nullptr, "for use by Array value only");
            i.idx = -1;
            return i;
        }
        zend_hash_internal_pointer_reset_ex(Z_ARR(_value), &i.pos);
        return i;
    }
    bool value_t::has_more(value_t::iterator& i) const {
        if(i.idx == -1) return false;
        i.val = zend_hash_get_current_data_ex(Z_ARR(_value), &i.pos);
        zend_hash_get_current_key_ex(Z_ARR(_value), &i.key, &i.idx, &i.pos);
        return i.val != nullptr;
    }
    void value_t::next(value_t::iterator& i) const {
        zend_hash_move_forward_ex(Z_ARR(_value), &i.pos);
    }
}