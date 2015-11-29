/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   parameter.h
 * Author: wuhao
 *
 * Created on 2015年11月29日, 下午2:33
 */

#ifndef PHP_PARAMETER_H
#define PHP_PARAMETER_H

namespace php {
class parameter {
public:
    parameter(zval* p, size_t s);
    parameter(zval* a);
    // -------------------------------------------------------------------------
    value operator[](size_t idx) const;
    zval* get(size_t idx) const;
    size_t size;
private:
    zval*  p_val;
    bool   b_array;
};
}

#endif /* PARAMETER_H */

