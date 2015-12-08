/* 
 * File:   property.cpp
 * Author: wuhao
 * 
 * Created on 2015年11月29日, 下午5:22
 */

#include "../core.h"

namespace php {
    value property::sget(const std::string& key) {
        if(p_ce == nullptr) {
            zend_throw_error(nullptr, "get property of non-class");
            return null;
        }
        zval* v = zend_read_static_property(p_ce, key.c_str(), key.length(), 0);
        return value(v, true);
    }
    value property::oget(const std::string& key) {
        if(p_ce == nullptr || p_obj == nullptr) {
            zend_throw_error(nullptr, "get property of non-object");
            return null;
        }
        zval rv, *v;
        v = zend_read_property(p_ce, p_obj, key.c_str(), key.length(), 0, &rv);
        return value(v, true);
    }
    property& property::sset(const std::string& key, const value& r) {
        if(p_ce == nullptr) {
            zend_throw_error(nullptr, "set property of non-class");
            return *this;
        }
        zend_update_static_property(p_ce, key.c_str(), key.length(), const_cast<zval*>(&r.val));
        // r.add_ref();
        return *this;
    }
    property& property::oset(const std::string& key, const value& r) {
        if(p_ce == nullptr || p_obj == nullptr) {
            zend_throw_error(nullptr, "set property of non-object");
            return *this;
        }
        zend_update_property(p_ce, p_obj, key.c_str(), key.length(), const_cast<zval*>(&r.val));
        // r.add_ref();
        return *this;
    }
    bool property::__isset(const std::string& key) {
        if(p_ce != nullptr) {
            zend_string* name = zend_string_init(key.c_str(), key.length(), 0);
            zend_property_info* info = zend_get_property_info(p_ce, name, 1);
            zend_string_release(name);
            if(info == nullptr) {
                return false;
            }else{
                return true;
            }
        }
        return false;
    }
}