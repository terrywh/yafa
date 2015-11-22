/* 
 * File:   php.hpp
 * Author: wuhao
 *
 * Created on 2015年11月18日, 上午10:15
 */

#ifndef EASY_HPP
#define EASY_HPP

#define EASY_EXPORT __attribute__ ((visibility ("default")))
#define EASY_HIDDEN __attribute__ ((visibility ("hidden")))

#include <utility>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <tuple>

#include "php.h"
#include "zend_interfaces.h"

#include "easy/exception.h"
#include "easy/value.h"
#include "easy/argument.h"
#include "easy/param.h"
#include "easy/class.h"
#include "easy/module.h"

namespace easy {
    value_t call_method(const value_t* obj, const std::string& method, const argument_t& param);
    value_t call_method_0(const value_t* obj, const std::string& method);
    value_t call_method_1(const value_t* obj, const std::string& method, const value_t& a1);
    value_t call_method_2(const value_t* obj, const std::string& method, const value_t& a1, const value_t& a2);
    value_t property(const value_t& obj, const std::string& name);
    value_t property_exists(const value_t& obj, const std::string& name);
    value_t create_object(const char* class_name, const param_t& param);
}

#endif /* EASY_HPP */

