#include "../easy.h"
namespace easy{
value_t call_method(value_t obj, const char* method, const param_t& param) {
    zend_fcall_info fci;
    value_t ret;
    //zend_fcall_info_cache fcic;
    // 
    fci.size = sizeof (fci);
    fci.no_separation = 1;
    fci.symbol_table = NULL;

    ZVAL_STRINGL(&fci.function_name, method, sizeof (method) - 1);

    fci.object = Z_OBJ(obj._value);
    fci.function_table = NULL;
    fci.params = param._params;
    fci.param_count = param.size;
    fci.retval = ret;
    zend_call_function(&fci, NULL);

    zval_ptr_dtor(&fci.function_name);
    return std::move(ret);
}

value_t create_object(const char* class_name, const param_t& param) {
    zend_string* name = zend_string_init(class_name, sizeof (class_name) - 1, 0);
    zend_class_entry* ce = zend_lookup_class(name);
    zend_string_release(name);
    if (ce == nullptr) {
        zend_throw_error(nullptr, "class \"%s\" not found", class_name);
        return value_t();
    }
    zval obj;
    object_init_ex(&obj, ce);
}

}