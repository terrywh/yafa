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

#include "php.h"

#include "easy/exception.h"
#include "easy/value.h"
#include "easy/value_array.h"
#include "easy/param.h"
#include "easy/class.h"
#include "easy/module.h"

namespace easy {
    value_t call_method(value_t obj, const char* method, const param_t& param);
    value_t create_object(const char* class_name, const param_t& param);
}

#endif /* EASY_HPP */

