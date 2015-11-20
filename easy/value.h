/* 
 * File:   value.hpp
 * Author: wuhao
 *
 * Created on 2015年11月18日, 上午11:56
 */

#ifndef PHP_VALUE_HPP
#define PHP_VALUE_HPP

namespace easy {
    class value_t {
    public:
        zval _value;
        // 默认构造，直接初始化为 IS_NULL
        value_t() {
            // _value = (zval*)emalloc(sizeof(zval));
            ZVAL_NULL(&_value);
        }
        value_t(zval* val) {
            ZVAL_COPY(&_value, val);
        }
        value_t(const value_t& val) {
            // _value = (zval*)emalloc(sizeof(zval));
            ZVAL_COPY(&_value, &val._value);
        }
        value_t(const char* str) {
            // _value = (zval*)emalloc(sizeof(zval));
            auto zstr = zend_string_init(str, strlen(str), 0);
            ZVAL_STR(&_value, zstr);
        }
        value_t(const std::string& str) {
            // _value = (zval*)emalloc(sizeof(zval));
            auto zstr = zend_string_init(str.c_str(), str.length(), 0);
            ZVAL_STR(&_value, zstr);
        }
        value_t(const bool& b) {
            // _value = (zval*)emalloc(sizeof(zval));
            ZVAL_BOOL(&_value, false);
        }
        ~value_t() {
            zval_ptr_dtor(&_value);
        }
        // 类型判定
        bool is_string() {
            return Z_TYPE(_value) == IS_STRING;
        }
        bool is_array() {
            return Z_TYPE(_value) == IS_ARRAY;
        }
        bool is_null() {
            return Z_TYPE(_value) == IS_NULL;
        }
        // 类型转换
        operator double() {
            if(Z_TYPE(_value) != IS_DOUBLE) {
                convert_to_double(&_value);
            }
            return _value.value.dval;
            return 0;
        }
        operator zend_long() {
            if(Z_TYPE(_value) != IS_LONG) {
                convert_to_long(&_value);
            }
            return _value.value.lval;
        }
        operator bool() {
            convert_to_boolean(&_value);
            return Z_TYPE(_value) == IS_TRUE;
        }
        operator std::string() {
            if(zval_get_type(&_value) != IS_STRING) {
                convert_to_string(&_value);
            }
            return std::string(Z_STRVAL(_value), Z_STRLEN(_value));
        }
        operator zval*() {
            return &_value;
        }
    };
}

#endif /* VALUE_HPP */

