#include "../easy.h"

namespace easy {
void class_abstract::__construct(INTERNAL_FUNCTION_PARAMETERS) {
    zval *argv;
    int size;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &argv, &size) == FAILURE) {
        zend_throw_error(NULL, "failed to construct object");
        return;
    }
    param_t param(argv, size);
    class_store* c = class_store::get_store(getThis());
    c->_instance->__construct(param);
}
    
void class_abstract::__destruct(INTERNAL_FUNCTION_PARAMETERS) {
    class_store* c = class_store::get_store(getThis());
    try {
        c->_instance->__destruct();
    } catch (exception_t ex) {
        zend_throw_error(NULL, ex.message.c_str());
        return;
    }
}
    
void class_abstract::__call(INTERNAL_FUNCTION_PARAMETERS) {
    zend_string* z_name;
    zval* z_argv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &z_name, &z_argv) == FAILURE) {
        zend_throw_error(NULL, "failed to __call");
        return;
    }
    class_store* c = class_store::get_store(getThis());
    param_t     param(z_argv, zend_hash_num_elements(Z_ARRVAL_P(z_argv)));
    std::string name(ZSTR_VAL(z_name), ZSTR_LEN(z_name));
    auto pfunc = c->_abstract->_object_method_map[name];
    if (pfunc != nullptr) {
        value_t retval = (c->_instance->*pfunc)(param);
        ZVAL_ZVAL(return_value, (zval*) retval, 1, 0);
    } else {
        // 默认实现是 “找不到对象方法”
        value_t retval = c->_instance->__call(name,param);
        ZVAL_ZVAL(return_value, (zval*) retval, 1, 0);
    }
}
    
void class_abstract::__get(INTERNAL_FUNCTION_PARAMETERS) {
    zend_string* name;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &name) == FAILURE) {
        zend_throw_error(NULL, "failed to __call");
        return;
    }
    class_store* c = class_store::get_store(getThis());
    value_t retval = c->_instance->__get(std::string(name->val, name->len));
    RETURN_ZVAL(&retval._value, 1, 0);
}
    
void class_abstract::__isset(INTERNAL_FUNCTION_PARAMETERS) {
    zend_string* name;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &name) == FAILURE) {
        zend_throw_error(NULL, "failed to __isset");
        return;
    }
    class_store* c = class_store::get_store(getThis());
    RETURN_BOOL(c->_instance->__isset(std::string(name->val, name->len)));
    // return_value->u1.type_info = IS_NULL;
}
    
void class_abstract::__callStatic(INTERNAL_FUNCTION_PARAMETERS) {
    zend_string* z_name;
    zval* z_argv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &z_name, &z_argv) == FAILURE) {
        zend_throw_error(NULL, "failed to __callStatic");
        return;
    }
    class_abstract* ca = *((class_abstract**)(execute_data->called_scope->info.user.doc_comment->val + 1));
    param_t      param(z_argv, zend_hash_num_elements(Z_ARRVAL_P(z_argv)));
    std::string  name(ZSTR_VAL(z_name), ZSTR_LEN(z_name));
    
    auto func = ca->_static_method_map[name];
    if(func != nullptr) {
        value_t retval = func(param);
        ZVAL_ZVAL(return_value, (zval*)retval, 1, 0);
    }else{
        value_t retval = ca->proxyCallStatic(name, param);
        ZVAL_ZVAL(return_value, (zval*)retval, 1, 0);
    }
}

ZEND_BEGIN_ARG_INFO(constructor_argv,0)
ZEND_ARG_VARIADIC_INFO(0, argv)
ZEND_END_ARG_INFO()

static zend_internal_arg_info destructor_argv[] = {
    { (const char*) (zend_uintptr_t) (0), NULL, 0, 0, 0, 0},
};
static zend_internal_arg_info call_argv[] = {
    { (const char*) (zend_uintptr_t) (2), NULL, 0, 0, 0, 0},
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_ARRAY_INFO(0, argv, 0)
};
static zend_internal_arg_info get_argv[] = {
    { (const char*) (zend_uintptr_t) (2), NULL, 0, 0, 0, 0},
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
};
// TODO 继续支持其他魔术方法？
static zend_function_entry method_entry[] = {
    { "__construct", class_abstract::__construct, constructor_argv, 1, 0},
//   { "__destruct" , class_abstract::__destruct , destructor_argv , 0, ZEND_ACC_PUBLIC},
    { "__call"      , class_abstract::__call      , call_argv       , 2, ZEND_ACC_PUBLIC},
    { "__callStatic", class_abstract::__callStatic, call_argv       , 2, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC},
//    { "__get"      , class_abstract::__get      , get_argv        , 1, ZEND_ACC_PUBLIC},
//    { "__isset"    , class_abstract::__isset    , get_argv        , 1, ZEND_ACC_PUBLIC},
    ZEND_FE_END
};

void class_abstract::bootstrap() {
    zend_class_entry ce;
    class_abstract* _self = this;
    INIT_OVERLOADED_CLASS_ENTRY_EX(ce, _name.c_str(), _name.length(), method_entry, nullptr, nullptr, nullptr, nullptr, nullptr);
    _entry = zend_register_internal_class_ex(&ce, nullptr);
    // 利用注释保存当前对象指针
    _entry->info.user.doc_comment = zend_string_init("\0", 2 + sizeof(class_abstract*), 1);
    memcpy(_entry->info.user.doc_comment->val + 1, &_self, sizeof(class_abstract*));
    _entry->create_object = create_object;
}

zend_object* class_abstract::create_object(zend_class_entry* ce) {
    class_store *_store;

    _store = (class_store*)ecalloc(1, sizeof(class_store) + zend_object_properties_size(ce));
    zend_object_std_init(&_store->_object, ce);
    object_properties_init(&_store->_object, ce);
    _store->_object.handlers = &std_object_handlers;
    // 从 doc_comment 中恢复对象指针
    _store->_abstract = *((class_abstract**)(ce->info.user.doc_comment->val + 1));
    _store->_instance = _store->_abstract->create_instance();
    
    return &_store->_object;
}


void class_abstract::add_object_method(const std::string& name, value_t(class_base::*method)(param_t& argv)) {
    class_abstract* _self = this;
    this->_object_method_map[name] = method;
}

void class_abstract::add_static_method(const std::string& name, value_t(*method)(param_t& argv)) {
    this->_static_method_map[name] = method;
}
}
