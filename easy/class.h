/* 
 * File:   class.hpp
 * Author: wuhao
 *
 * Created on 2015年11月17日, 下午8:50
 */

#ifndef EASY_CLASS_HPP
#define EASY_CLASS_HPP

namespace easy {
    // 实现继承
    class class_base {
    public:
        class_base() {};
        virtual void __construct(param_t& param) {};
        virtual void __destruct() {}
        virtual value_t __call(const std::string& name, param_t& param) {
            zend_throw_error(nullptr, "call to undefined object method \"%s\"", name.c_str());
            return std::move(value_t());
        };
        static value_t __callStatic(const std::string& name, param_t& param) {
            zend_throw_error(nullptr, "call to undefined static method \"%s\"", name.c_str());
            return std::move(value_t());
        };
        virtual value_t __get(const std::string& name) {
            zend_throw_error(nullptr, "undefined property \"%s\"", name.c_str());
            return std::move(value_t());
        };
        virtual bool __isset(const std::string& name) {
            return false;
        };
    };
    
    class class_abstract;
     // 向 zend_object 中保存关联的
    class class_store {
    public:
        class_base*       _instance;
        class_abstract*   _abstract;
        static inline class_store* get_store(zval* v) {
            return (class_store*)((char*)(Z_OBJ_P(v)) - offsetof(class_store, _object));
        };
        // 注意，此元素必须放在最后
        zend_object       _object;
      
    };
    
    typedef value_t(class_base::*object_method_t)(param_t& argv);
    typedef value_t(*static_method_t)(param_t& argv);
    // 方便保存指针
    class class_abstract {
    public:
        class_abstract(const std::string& name):_name(name) {
            php_printf("class_abstract [%s]\n",name.c_str());
        };
        class_abstract(const class_abstract& self):class_abstract(self._name) {
            
        };
        std::string _name;
        void bootstrap();
        virtual class_base* create_instance() = 0;
        virtual value_t proxyCallStatic(const std::string& name, param_t& param) = 0;
        static zend_object* create_object(zend_class_entry* ce);
        static void __construct(INTERNAL_FUNCTION_PARAMETERS);
        static void __destruct(INTERNAL_FUNCTION_PARAMETERS);
        static void __call(INTERNAL_FUNCTION_PARAMETERS);
        static void __callStatic(INTERNAL_FUNCTION_PARAMETERS);
        static void __get(INTERNAL_FUNCTION_PARAMETERS);
        static void __isset(INTERNAL_FUNCTION_PARAMETERS);
        
        zend_class_entry*                                                   _entry;
        std::map<const std::string, value_t(class_base::*)(param_t& argv) > _object_method_map;
        std::map<const std::string, value_t(*)(param_t& argv) >             _static_method_map;
        
        void add_object_method(const std::string& name, object_method_t method);
        void add_static_method(const std::string& name, static_method_t method);
    };    
   
    // 反射类
    template <class T>
    class class_t: public class_abstract {
    public:
        class_t():class_abstract(T::CLASS_NAME) {};
        void add(const std::string& name, value_t(T::*method)(param_t& argv)) {
            this->add_object_method(name, static_cast<object_method_t>(method));
        };
        void add(const std::string& name, value_t(*method)(param_t& argv)) {
            this->add_static_method(name, static_cast<static_method_t>(method));
        };
        
        virtual class_base* create_instance() override {
            return new T();
        }
        virtual value_t proxyCallStatic(const std::string& name, param_t& param) {
            return T::__callStatic(name, param);
        }
    };
};

#endif /* PHP_CLASS_HPP */

