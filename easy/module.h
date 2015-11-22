/**
 * 
 */
#ifndef EASY_MODULE_HPP
#define EASY_MODULE_HPP

#include "class.h"


namespace easy {
    typedef value_t(*function_t)(param_t& argv);
    class module_t {
    public:
        static int module_startup(INIT_FUNC_ARGS);
        static int module_shutdown(SHUTDOWN_FUNC_ARGS);
        static int request_startup(INIT_FUNC_ARGS);
        static int request_shutdown(SHUTDOWN_FUNC_ARGS);
        static void info(ZEND_MODULE_INFO_FUNC_ARGS);
        // 模块名称
        const char* _name;
        // 模块版本
        const char* _vers;
        // 导出函数
        zend_module_entry _entry = {
            STANDARD_MODULE_HEADER,
            NULL, // name
            NULL, // _zend_function_entry
            module_t::module_startup,
            module_t::module_shutdown,
            module_t::request_startup,
            module_t::request_shutdown,
            module_t::info,
            NULL, // version
            STANDARD_MODULE_PROPERTIES
        };
        static module_t* _inst;
        module_t(const char* name, const char* version):_name(name), _vers(version) {
            _entry.name = _name;
            _entry.version = _vers;
//            // _entry.globals_ptr = this;
            module_t::_inst = this;
        };
        ~module_t() {
            // TODO 清理 _classes_map;
        };
        // 自动将当前实例转换为 zend_module_entry* 类型返回
        operator zend_module_entry*() {
            _function_entry.push_back(ZEND_FE_END);
            // 随着添加 vector 内部空间会动态调整
            _entry.functions = _function_entry.data();
            return &_entry;
        };
        template <class T>
        module_t& add(class_t<T>& c) { // class<T> 的实例不能被回收
            _classes_map[c._name] = &c;
            return *this;
        };
        
        // 函数表
        std::vector<zend_function_entry>        _function_entry;
        std::map<std::string, class_abstract*>  _classes_map;
        std::map<std::string, function_t>       _function_map;
        
        // 函数处理
        static void __call(INTERNAL_FUNCTION_PARAMETERS);
//        static zend_internal_arg_info _function_argv[];
        module_t& function(const char* cname, function_t fn);
    };
    
    
};


#endif /* EASY_MODULE_HPP */

