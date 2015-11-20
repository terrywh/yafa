/* 
 * File:   param.h
 * Author: wuhao
 *
 * Created on 2015年11月18日, 下午6:14
 */

#ifndef PARAM_H
#define PARAM_H

#include "value_array.h"


namespace easy {
    class param_t {
        bool is_array;
        bool need_free = false;
    public:
        zval* _params;
        unsigned int size;
        param_t(zval* p, unsigned int s):_params(p),size(s) {
            if(size > 0) {
                if(Z_TYPE_P(_params) == IS_ARRAY) {
                    is_array = true;
                }else{
                    is_array = false;
                }
            }
        }
        ~param_t() {
            if(need_free) efree(_params);
        }
        
        value_t operator [](zend_ulong index) {
            if(size > 0 && is_array) { // 兼容数组类型的变量
                zval* v = zend_hash_index_find(Z_ARR_P(_params), index);
                if(v != nullptr) {
                    return value_t(v);
                }else{
                    zend_throw_error(NULL, "undefined offset \"%ld\"", index);
                    return std::move(value_t());
                }
            }else if(index < size) {
                return value_t(&_params[index]);
            }else{
                zend_throw_error(NULL, "index \"%d\" out of range", index);
                return std::move(value_t());
            }
        };
    };
}

#endif /* PARAM_H */

