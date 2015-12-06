/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   value.h
 * Author: wuhao
 *
 * Created on 2015年11月29日, 下午1:30
 */

#ifndef PHP_VALUE_H
#define PHP_VALUE_H

namespace php {
    class property;
    class parameter;
    class value {
    public:
        value();
        value(zval* p, bool own = false);
        value(const value& orig);
        value(value&& orig);
        virtual ~value();
        inline bool is_refcounted() const {
            return Z_REFCOUNTED(val);
        }
        inline bool is_type(int type) const {
            return Z_TYPE(val) == type;
        }
        inline bool is_empty() const {
            return Z_TYPE(val) == IS_NULL || Z_TYPE(val) == IS_FALSE ||
                (Z_TYPE(val) == IS_LONG && Z_LVAL(val) == 0l) ||
                (Z_TYPE(val) == IS_STRING && Z_STRLEN(val) == 0) ||
                (Z_TYPE(val) == IS_ARRAY && zend_hash_num_elements(Z_ARR(val)) == 0);
        }
        inline void clear() {
            if(own) {
                zval_ptr_dtor(&val);
            }
            ZVAL_NULL(&val);
        }
        // ---------------------------------------------------------------------
        // 从各种值构造 value
        value(const char* str);
        value(const std::string& str);
        value(const long& l);
        value(const int& i);
        value(const bool& b);
        value(const double& d);
        // ---------------------------------------------------------------------
        // 赋值
        value& operator=(const value&);
        value& operator=(const char* str);
        value& operator=(const std::string& str);
        value& operator=(const long& l);
        value& operator=(const long long& l);
        value& operator=(const bool& b);
        value& operator=(const zval* v);
        value& operator=(const double& d);
        void update_parent();
        // ---------------------------------------------------------------------
        // 数组
        value operator[](int index);
        value operator[](const char* key);
        value operator[](const std::string& key);
        bool __isset(int idx) const;
        bool __isset(const std::string& key) const;
        size_t length() const;
        
        value_iterator begin() const;
        value_iterator end() const;
        
        // ---------------------------------------------------------------------
        // 转换
        operator std::string();
        operator zend_long();
        inline zval* intern() {
            return &val;
        }
    protected:
        // 以下用于层次结构
        std::string  key_str;
        zend_ulong   key_idx;
        value*       parent   = nullptr;
        bool         key_type = false;
        
        // ---------------
        
        bool    own;
        zval    val;
        
        
        
        friend class class_wrap;
        friend class module;
        friend class property;
        friend class parameter;
        
        friend value call_method(value* obj, const std::string& method, const std::vector<zval>& params);
        friend value call_method_0(value* obj, const std::string& method);
        friend value call_method_1(value* obj, const std::string& method, const value& a1);
        friend value call_method_2(value* obj, const std::string& method, const value& a1, const value& a2);
        // friend value create_object(zend_class_entry* ce);
    };
    
    static value null;
}
#endif /* PHP_VALUE_H */

