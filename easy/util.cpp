#include "../easy.h"
namespace easy{
value_t call_method(const value_t* obj, const std::string& method, const argument_t& param) {
    value_t ret;
    zend_fcall_info fci;
    // zend_fcall_info_cache fcc;
    
    fci.size = sizeof (fci);
    fci.no_separation = 1;
    fci.symbol_table = NULL;
    ZVAL_STRINGL(&fci.function_name, method.c_str(), method.length());
    fci.object = Z_OBJ(obj->_value);
    fci.function_table = NULL;
    fci.params = const_cast<zval*>(param._args.data());
    fci.param_count = param._args.size();
    fci.retval = &ret._value;
    
//    fcc.initialized = 1;
//    fcc.function_handler = (zend_function*)zend_hash_find_ptr(&Z_OBJCE(obj._value)->function_table, Z_STR(fci.function_name));
//    fcc.calling_scope = Z_OBJCE(obj._value);
//    fcc.called_scope  = Z_OBJCE(obj._value);
//    fcc.object = Z_OBJ(obj._value);
    int result = zend_call_function(&fci, nullptr);
    zval_ptr_dtor(&fci.function_name);
    if(result != SUCCESS) {
        zend_throw_error(nullptr, "Couldn't execute method \"%s::%s\"", Z_OBJCE(obj->_value)->name->val, method.c_str());
    }
    return ret;
}

value_t call_method_1(const value_t* obj, const std::string& method, const value_t& a1) {
    value_t retval;
    if(obj == nullptr) {
        zend_call_method(nullptr, nullptr, nullptr, method.c_str(), method.length(), const_cast<zval*>(&retval._value), 1, const_cast<zval*>(&a1._value), nullptr);
    }else{
        zend_call_method(const_cast<zval*>(&obj->_value), Z_OBJCE(obj->_value), nullptr, method.c_str(), method.length(), const_cast<zval*>(&retval._value), 1, const_cast<zval*>(&a1._value), nullptr);
    }
    return retval;
}

value_t call_method_2(const value_t* obj, const std::string& method, const value_t& a1, const value_t& a2) {
    value_t retval;
    if(obj == nullptr) {
        zend_call_method(nullptr, nullptr, nullptr, method.c_str(), method.length(), const_cast<zval*>(&retval._value), 1, const_cast<zval*>(&a1._value), const_cast<zval*>(&a2._value));
    }else{
        zend_call_method(const_cast<zval*>(&obj->_value), Z_OBJCE(obj->_value), nullptr, method.c_str(), method.length(), const_cast<zval*>(&retval._value), 2, const_cast<zval*>(&a1._value), const_cast<zval*>(&a2._value));
    }
    return retval;
}

value_t property(const value_t& obj, const std::string& name) {
    zval rv;
    return zend_read_property(Z_OBJCE(obj._value), const_cast<zval*>(&obj._value), name.c_str(), name.length(), false, &rv);
};

value_t property_exists(const value_t& obj, const std::string& name) {
    zval rv;
    if( Z_TYPE_P(Z_OBJCE(obj._value)->default_properties_table) == IS_ARRAY ) {
        return zend_hash_str_exists(Z_ARR_P(Z_OBJCE(obj._value)->default_properties_table), name.c_str(), name.length());
    }
    return false;
};

value_t create_object(const char* class_name, const param_t& param) {
    zend_string* name = zend_string_init(class_name, sizeof (class_name) - 1, 0);
    zend_class_entry* ce = zend_lookup_class(name);
    zend_string_release(name);
    if (ce == nullptr) {
        zend_throw_error(nullptr, "class \"%s\" not found", class_name);
        return VALUE_NULL;
    }
    zval obj;
    object_init_ex(&obj, ce);
}

}