/*
 * File:   exception.h
 * Author: wuhao
 *
 * Created on 2015年11月18日, 下午6:09
 */

#ifndef EASY_EXCEPTION_H
#define EASY_EXCEPTION_H


namespace easy {
    class exception_t {
    public:
        std::string message;
        exception_t(const std::string& msg):message(msg) {
            
        };
    };
}

#endif /* EXCEPTION_H */

