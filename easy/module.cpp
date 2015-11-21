#include "../easy.h"

namespace easy {

module_t* module_t::_inst;

int module_t::module_startup(INIT_FUNC_ARGS) {
    for(auto i=module_t::_inst->_classes_map.begin(); i!= module_t::_inst->_classes_map.end(); ++i) {
        i->second->module_startup();
    }
    return SUCCESS;
}
int module_t::module_shutdown(SHUTDOWN_FUNC_ARGS) {
    for(auto i=module_t::_inst->_classes_map.begin(); i!= module_t::_inst->_classes_map.end(); ++i) {
        i->second->module_shutdown();
    }
    return SUCCESS;
}
int module_t::request_startup(INIT_FUNC_ARGS) {
    for(auto i=module_t::_inst->_classes_map.begin(); i!= module_t::_inst->_classes_map.end(); ++i) {
        i->second->request_startup();
    }
    return SUCCESS;
}
int module_t::request_shutdown(SHUTDOWN_FUNC_ARGS) {
    for(auto i=module_t::_inst->_classes_map.begin(); i!= module_t::_inst->_classes_map.end(); ++i) {
        i->second->request_shutdown();
    }
    return SUCCESS;
}
void module_t::info(ZEND_MODULE_INFO_FUNC_ARGS) {

}


ZEND_BEGIN_ARG_INFO(function_argv, 0)
ZEND_ARG_VARIADIC_INFO(0, param)
ZEND_END_ARG_INFO();

void module_t::add(const char* cname, function_t fn) {
    zend_string* name = zend_string_init(cname, strlen(cname), 1);
    module_t::_function_map[std::string(name->val, name->len)] = fn;
    _function_entry.push_back({"testFunction", module_t::__call, function_argv, 2, 0});
}

void module_t::__call(INTERNAL_FUNCTION_PARAMETERS) {
    std::string  name(execute_data->func->internal_function.function_name->val, execute_data->func->internal_function.function_name->len);
    zval* z_argv = nullptr;
    int   z_size = 0;
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "*", &z_argv, &z_size) == FAILURE) {
        zend_throw_error(NULL, "failed to __call");
        return;
    }
    
    auto func = module_t::_inst->_function_map[name];
    if(func != nullptr) {
        param_t p(z_argv, z_size);
        value_t r = func(p);
        ZVAL_ZVAL(return_value, &r._value, 1, 0);
    }else{
        // NOT SUPPOSE TO BE HERE
        zend_throw_error(nullptr, "function \"%s\" not exist", execute_data->func->internal_function.function_name->val);
    }
}
}
