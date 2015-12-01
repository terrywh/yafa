#include "../core.h"
#include "ext/standard/info.h"
namespace php {

    module* module::p_self = nullptr;

    int module::mstartup(INIT_FUNC_ARGS) {
        for(auto i=module::p_self->m_classes.begin(); i!= module::p_self->m_classes.end(); ++i) {
            i->second->mstartup();
        }
        return SUCCESS;
    }
    int module::mshutdown(SHUTDOWN_FUNC_ARGS) {
        for(auto i=module::p_self->m_classes.begin(); i!= module::p_self->m_classes.end(); ++i) {
            i->second->mshutdown();
        }
        return SUCCESS;
    }
    int module::rstartup(INIT_FUNC_ARGS) {
        for(auto i=module::p_self->m_classes.begin(); i!= module::p_self->m_classes.end(); ++i) {
            i->second->rstartup();
        }
        return SUCCESS;
    }
    int module::rshutdown(SHUTDOWN_FUNC_ARGS) {
        for(auto i=module::p_self->m_classes.begin(); i!= module::p_self->m_classes.end(); ++i) {
            i->second->rshutdown();
        }
        return SUCCESS;
    }
    void module::info(ZEND_MODULE_INFO_FUNC_ARGS) {
        php_info_print_table_start();
	php_info_print_table_header(2, "yafa support", "enabled");
	php_info_print_table_row(2, "Version", _module_entry.version);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
    }

    zend_module_entry module::_module_entry = {
        STANDARD_MODULE_HEADER,
        NULL, // name
        NULL, // _zend_function_entry
        module::mstartup,
        module::mshutdown,
        module::rstartup,
        module::rshutdown,
        module::info,
        NULL, // version
        STANDARD_MODULE_PROPERTIES
    };
    
    module::module(const char* name, const char* version):_name(name), _vers(version) {
        _module_entry.name    = _name;
        _module_entry.version = _vers;
        module::p_self = this;
    }
    
    module::~module() {
        m_classes.clear();
        m_functions.clear();
    }
    // 自动将当前实例转换为 zend_module_entry* 类型返回
    module::operator zend_module_entry*() {
        _function_entry.push_back(ZEND_FE_END);
        // 随着添加 vector 内部空间会动态调整
        _module_entry.functions = _function_entry.data();
        return &_module_entry;
    }

    ZEND_BEGIN_ARG_INFO(function_argv, 0)
    ZEND_ARG_VARIADIC_INFO(0, param)
    ZEND_END_ARG_INFO();

    module& module::add(const char* cname, function_t fn) {
        zend_string* name = zend_string_init(cname, strlen(cname), 1);
        module::m_functions[std::string(name->val, name->len)] = fn;
        _function_entry.push_back({name->val, module::__call, function_argv, 2, 0});
        return *this;
    }

    void module::__call(INTERNAL_FUNCTION_PARAMETERS) {
        std::string  name(execute_data->func->internal_function.function_name->val, execute_data->func->internal_function.function_name->len);
        zval* z_argv = nullptr;
        int   z_size = 0;
        if(zend_parse_parameters(ZEND_NUM_ARGS(), "*", &z_argv, &z_size) == FAILURE) {
            zend_throw_error(NULL, "failed to __call");
            return;
        }
        auto func = module::p_self->m_functions[name];
        if(func != nullptr) {
            parameter param(z_argv, z_size);
            value r = func(param);
            RETURN_ZVAL(&r.val, 1, 0);
        }else{
            // NOT SUPPOSE TO BE HERE
            zend_throw_error(nullptr, "function \"%s\" not exist", execute_data->func->internal_function.function_name->val);
        }
    }
}

