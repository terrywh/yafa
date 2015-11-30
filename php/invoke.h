/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   invoke.h
 * Author: wuhao
 *
 * Created on 2015年11月30日, 上午1:33
 */

#ifndef PHP_INVOKE_H
#define PHP_INVOKE_H

namespace php {
    value call_method(value* obj, const std::string& method, const std::vector<zval>& params);
    value call_method_0(value* obj, const std::string& method);
    value call_method_1(value* obj, const std::string& method, const value& a1);
    value call_method_2(value* obj, const std::string& method, const value& a1, const value& a2);
    
    // value create_object(zend_class_entry* ce);
}

#endif /* PHP_INVOKE_H */

