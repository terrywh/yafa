/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   property.h
 * Author: wuhao
 *
 * Created on 2015年11月29日, 下午5:22
 */

#ifndef PHP_PROPERTY_H
#define PHP_PROPERTY_H

#include "value.h"


namespace php{

    class property {
    public:
        property(const std::string& class_name) {
            zend_string* z_class_name = zend_string_init(class_name.c_str(), class_name.length(), 0);
            p_ce = zend_lookup_class(z_class_name);
            zend_string_release(z_class_name);
        }
        property(zend_class_entry* p):p_ce(p) {
            
        }
        template <class T>
        property(T* ins) {
            p_obj = &ins->self;
            p_ce  = T::class_entry;
        }
        property(value val) {
            if(Z_TYPE(val.val) == IS_OBJECT) {
                p_ce = Z_OBJCE(val.val);
                p_obj = &val.val;
            }else{
                zend_throw_error(nullptr, "read property of non-object");
            }
        }
        property(zval* val) {
            if(Z_TYPE_P(val) == IS_OBJECT) {
                p_ce = Z_OBJCE_P(val);
                p_obj = val;
            }else{
                zend_throw_error(nullptr, "read property of non-object");
            }
        }
        value sget(const std::string& key);
        value oget(const std::string& key);
        property& sset(const std::string& key, const value& val);
        property& oset(const std::string& key, const value& val);
        bool __isset(const std::string& key);
    private:
        zval*             p_obj = nullptr;
        zend_class_entry* p_ce  = nullptr;
        zval              temp;
    };
}
#endif /* PHP_PROPERTY_H */

