/* 
 * File:   param.h
 * Author: wuhao
 *
 * Created on 2015年11月18日, 下午6:14
 */

#ifndef PARAM_H
#define PARAM_H


namespace easy {
    class param_t {
        bool is_array;
    public:
        zval* _params;
        unsigned int size;
        param_t(zval* p, unsigned int s):_params(p),size(s),is_array(false) {
        }
        param_t(zval* p, unsigned int s, bool is_array):_params(p),size(s),is_array(is_array) {
        }
        ~param_t() {
        }
        
        value_t operator [](zend_ulong index) {
            if(size > 0 && is_array) { // 兼容数组类型的变量
                zval* v = zend_hash_index_find(Z_ARR_P(_params), index);
                if(v != nullptr) {
                    return v;
                }
            }else if(index < size) {
                return _params + index;
            }
            return VALUE_NULL;
        };
    };
}

#endif /* PARAM_H */

