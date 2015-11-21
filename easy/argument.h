/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   value_array.h
 * Author: wuhao
 *
 * Created on 2015年11月20日, 下午1:31
 */

#ifndef EASY_VALUE_ARRAY_H
#define EASY_VALUE_ARRAY_H

namespace easy {
    class argument_t {
    private:
        std::vector<zval> _args;
    public:
        argument_t() {}
        void push(const value_t& v);
        void pop();
        void insert(zend_ulong index, const value_t& v);
        void remove(zend_ulong index);
        inline zend_ulong length() const {
            return _args.size();
        };
        friend value_t call_method(const value_t& obj, const std::string& method, const argument_t& param);
    };
}
#endif /* EASY_VALUE_ARRAY_H */


