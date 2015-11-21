/* 
 * File:   value.hpp
 * Author: wuhao
 *
 * Created on 2015年11月18日, 上午11:56
 */

#ifndef PHP_VALUE_HPP
#define PHP_VALUE_HPP

namespace easy {
    class argument_t;
    class value_t {
    private:
        zval _value;
    public:
        value_t(zval* val) {
            if(!val) {
                ZVAL_NULL(&_value);
            }else if(Z_REFCOUNTED_P(val)) {
                ZVAL_COPY(&_value, val);
            }else {
                memcpy(&_value, val, sizeof(zval));
            }
        }
        // 默认构造，直接初始化为 IS_NULL
        value_t() {
            // _value = (zval*)emalloc(sizeof(zval));
            ZVAL_NULL(&_value);
        }
        value_t(const value_t& val) {
            if(Z_REFCOUNTED(val._value)) {
                ZVAL_COPY(&_value, &val._value);
            }else{
                memcpy(&_value, &val._value, sizeof(zval));
            }
        }
        value_t& operator=(const value_t& val) {
            if(Z_REFCOUNTED(val._value)) {
                ZVAL_COPY(&_value, &val._value);
            }else{
                memcpy(&_value, &val._value, sizeof(zval));
            }
            return *this;
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
            if(Z_REFCOUNTED(_value)) {
                zval_ptr_dtor(&_value);
            }
        }
        // 类型判定
        inline bool is_string() const {
            return Z_TYPE(_value) == IS_STRING;
        }
        inline bool is_array() const {
            return Z_TYPE(_value) == IS_ARRAY;
        }
        inline bool is_null() const  {
            return Z_TYPE(_value) == IS_NULL;
        }
        inline bool is_long() const {
            return Z_TYPE(_value) == IS_LONG;
        }
        inline bool is_object() const {
            return Z_TYPE(_value) == IS_OBJECT;
        }
        // 辅助
        inline bool empty() const {
            return Z_TYPE(_value) != IS_NULL || 
                    (Z_TYPE(_value) == IS_LONG && Z_LVAL(_value) == 0l) ||
                    (Z_TYPE(_value) == IS_STRING && Z_STRLEN(_value) == 0) ||
                    (Z_TYPE(_value) == IS_ARRAY && zend_hash_num_elements(Z_ARR(_value)));
        }
        // 类型转换
        operator double();
        operator zend_long();
        operator bool();
        operator std::string();
        
        inline zval* get() {
            return &_value;
        };
        // 数组
        value_t operator [](zend_ulong index);
        value_t operator [](const char*);
        value_t operator [](const std::string& key);
        bool has(zend_ulong index) const;
        bool has(const std::string& key) const;
        inline zend_ulong length() const;
        void set(zend_ulong index, const value_t& val);
        void set(const std::string& key, const value_t& val);
        
        value_t clone() const;
        
        static value_t create_array();
        
        friend class class_abstract;
        friend class class_base;
        friend class module_t;
        friend class argument_t;
        friend value_t call_method(const value_t& obj, const std::string& method, const argument_t& param);
        friend value_t call_method_1(const value_t& obj, const std::string& method, const value_t& a1);
        friend value_t call_method_2(const value_t& obj, const std::string& method, const value_t& a1, const value_t& a2);
        friend value_t property(const value_t& obj, const std::string& name);
    };
    static const value_t VALUE_NULL;
    static const std::string STRING_EMPTY;
}

#endif /* VALUE_HPP */

