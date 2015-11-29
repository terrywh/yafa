/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   php.h
 * Author: wuhao
 *
 * Created on 2015年11月29日, 下午1:31
 */

#ifndef CORE_H
#define CORE_H

#define CORE_EXPORT __attribute__ ((visibility ("default")))
#define CORE_HIDDEN __attribute__ ((visibility ("hidden")))

#include <string>
#include <vector>
#include <map>

#include "main/php.h"
#include "Zend/zend_interfaces.h"


#include "php/value_iterator.h"
#include "php/value.h"

#include "php/parameter.h"
#include "php/class.h"
#include "php/property.h"
#include "php/module.h"

#include "php/invoke.h"

namespace php {
    
}

#endif /* CORE_H */

