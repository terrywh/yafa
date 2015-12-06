/* 
 * File:   class.hpp
 * Author: wuhao
 *
 * Created on 2015年11月17日, 下午8:50
 */

#ifndef EASY_CLASS_HPP
#define EASY_CLASS_HPP

#include "value.h"


namespace php {
    class property;
    // 实现继承
    class class_base {
    public:
        class_base() {}
        virtual void __construct(const parameter& param) {}
        virtual void __destruct() {}
        virtual value __call(const std::string& name, const parameter& param);
        // static  value __callStatic(const std::string& name, param_t& param);
        virtual value __get(const std::string& name);
        virtual bool __isset(const std::string& name);
        virtual void __set(const std::string& name, const value& value);
        
    protected:
        zval   self;
        friend class class_wrap;
        friend class property;
    };
    
    typedef value(class_base::*object_method_t)(const parameter& argv);
    typedef value(*static_method_t)(const parameter& argv);
    
    class module;
    class class_wrap {
    public:
        static void __construct(INTERNAL_FUNCTION_PARAMETERS);
        static void __destruct(INTERNAL_FUNCTION_PARAMETERS);
        static void __call(INTERNAL_FUNCTION_PARAMETERS);
        static void __callStatic(INTERNAL_FUNCTION_PARAMETERS);
        static void __get(INTERNAL_FUNCTION_PARAMETERS);
        static void __set(INTERNAL_FUNCTION_PARAMETERS);
        static void __isset(INTERNAL_FUNCTION_PARAMETERS);
        static zend_object* create_object(zend_class_entry* ce);
    private:    
        std::map<std::string, object_method_t> m_object_method;
        std::map<std::string, static_method_t> m_static_method;
        std::map<std::string, int>       m_property;
        
        void mstartup();
        void rstartup();
        void mshutdown();
        void rshutdown();
    public:
        class_wrap(const std::string& name);
        virtual class_base* create() = 0;
        virtual void set_class_entry(zend_class_entry* ce) = 0;
        // 添加属性
        class_wrap& property(const std::string& name, int flags);
        
    protected:
        std::string class_name;
        void add_object_method(const std::string& name, object_method_t method);
        void add_static_method(const std::string& name, static_method_t method);
        friend class module;
    };
    
    typedef struct {
        zend_class_entry* entry;
        class_base*       self;
        class_wrap*       wrap;
        zend_object       obj;
    } _store;
    
    inline _store* get_store(zval* v) {
        return (_store*)((char*)(Z_OBJ_P(v)) - offsetof(_store, obj));
    }
    inline _store* get_store(value v) {
        return get_store(v.intern());
    }

    // 反射类
    template <class T>
    class class_declare: public class_wrap {
    public:
        class_declare(const std::string name):class_wrap(name) {
            T::class_name = name;
        };
        // 添加成员函数
        class_declare<T>& method(const std::string& name, value(T::*method)(const parameter& argv)) {
            this->add_object_method(name, static_cast<object_method_t>(method));
            return *this;
        }
        // 添加静态函数
        class_declare<T>& method(const std::string& name, value(*method)(const parameter& argv)) {
            this->add_static_method(name, static_cast<static_method_t>(method));
            return *this;
        }
        virtual class_base* create() override {
            return new T();
        }
        virtual void set_class_entry(zend_class_entry* ce) override {
            T::class_entry = ce;
        }
    };
};

#endif /* PHP_CLASS_HPP */

