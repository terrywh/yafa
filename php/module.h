/**
 * 
 */
#ifndef PHP_MODULE_HPP
#define PHP_MODULE_HPP

#include "../core.h"


namespace php {
    typedef value(*function_t)(const parameter& argv);
    class module {
    public:
        static int mstartup(INIT_FUNC_ARGS);
        static int mshutdown(SHUTDOWN_FUNC_ARGS);
        static int rstartup(INIT_FUNC_ARGS);
        static int rshutdown(SHUTDOWN_FUNC_ARGS);
        static void info(ZEND_MODULE_INFO_FUNC_ARGS);
        // 模块名称
        const char* _name;
        // 模块版本
        const char* _vers;
       
        static module* p_self;
        module(const char* name, const char* version);
        ~module();
        // 自动将当前实例转换为 zend_module_entry* 类型返回
        operator zend_module_entry*();
        // 函数表
         // 导出函数
        static zend_module_entry            _module_entry;
        std::vector<zend_function_entry>    _function_entry;
        std::map<std::string, class_wrap*>  m_classes;
        std::map<std::string, function_t>   m_functions;
        // 函数处理
        static void __call(INTERNAL_FUNCTION_PARAMETERS);
        module& add(const char* cname, function_t fn);
        template <class T>
        module& add(class_declare<T>&  c)  { // class<T> 的实例不能被回收
            m_classes[c.class_name] = &c;
            return *this;
        };
    };
    
    
};


#endif /* EASY_MODULE_HPP */

