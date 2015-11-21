#include "../easy.h"

namespace easy {
    
    value_t class_base::__call(const std::string& name, param_t& param) {
        zend_throw_error(nullptr, "call to undefined object method \"%s\"", name.c_str());
        return VALUE_NULL;
    }
    value_t class_base::__callStatic(const std::string& name, param_t& param) {
        return VALUE_NULL;
    }
    value_t class_base::__get(const std::string& name) {
        zend_throw_error(nullptr, "undefined property \"%s\"", name.c_str());
        return VALUE_NULL;
    }
    bool class_base::__isset(const std::string& name) {
        return false;
    }
    void class_base::__set(const std::string& name, const value_t& value) {
    }
    value_t class_base::sprop(const std::string& cn, const std::string& name) {
        zend_string* cname = zend_string_init(cn.c_str(), cn.length(), 0);
        zend_class_entry* ce = zend_lookup_class(cname);
        zend_string_release(cname);
        if(ce == nullptr) {
            zend_throw_error(nullptr, "static property of undefined class \"%s\"", cn.c_str());
            return VALUE_NULL;
        }
        return zend_read_static_property(ce, name.c_str(), name.length(), false);
    }
    value_t class_base::oprop(const std::string& name) {
        zval obj, rv;
        ZVAL_OBJ(&obj, &_store->_object);
        return zend_read_property(Z_OBJCE(obj), &obj, name.c_str(), name.length(), false, &rv);
    }
    void class_base::sprop(const std::string& cn, const std::string& name, const value_t& value) {
        zend_string* cname = zend_string_init(cn.c_str(), cn.length(), 0);
        zend_class_entry* ce = zend_lookup_class(cname);
        zend_string_release(cname);
        if(ce == nullptr) {
            zend_throw_error(nullptr, "static property of undefined class \"%s\"", cn.c_str());
        }else{
            zend_update_static_property(ce, name.c_str(), name.length(), const_cast<zval*>(&value._value));
        }
    }
    void class_base::oprop(const std::string& name, const value_t& value) {
        zval obj;
        ZVAL_OBJ(&obj, &_store->_object);
        zend_update_property(Z_OBJCE(obj), &obj, name.c_str(), name.length(), const_cast<zval*>(&value._value));
    }
    
    
    
    
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
    c->_instance->__destruct();
}
    
void class_abstract::__call(INTERNAL_FUNCTION_PARAMETERS) {
    zend_string* z_name;
    zval* z_argv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &z_name, &z_argv) == FAILURE) {
        zend_throw_error(NULL, "failed to __call");
        return;
    }
    class_store* c = class_store::get_store(getThis());
    param_t     param(z_argv, zend_hash_num_elements(Z_ARRVAL_P(z_argv)), true);
    std::string name(ZSTR_VAL(z_name), ZSTR_LEN(z_name));
    auto p = c->_abstract->_object_method_map.find(name);
    if (p != c->_abstract->_object_method_map.end()) {
        value_t retval = (c->_instance->*(p->second))(param);
        ZVAL_ZVAL(return_value, &retval._value, 1, 0);
    } else {
        // 默认实现是 “找不到对象方法”
        value_t retval = c->_instance->__call(name,param);
        ZVAL_ZVAL(return_value, &retval._value , 1, 0);
    }
}
    
void class_abstract::__get(INTERNAL_FUNCTION_PARAMETERS) {
    zend_string* z_name;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &z_name) == FAILURE) {
        zend_throw_error(NULL, "failed to __get");
        return;
    }
    class_store* c = class_store::get_store(getThis());
    std::string name(z_name->val, z_name->len);
    auto p = c->_abstract->_object_property.find(name);
    if(p != c->_abstract->_object_property.end()) {
        zval rv;
        RETURN_ZVAL(zend_read_property(c->_abstract->_entry, getThis(), p->first.c_str(), p->first.length(), false, &rv), 1, 0);
    }else{
        value_t retval = c->_instance->__get(name);
        RETURN_ZVAL(&retval._value, 1, 0);
    }
}

void class_abstract::__set(INTERNAL_FUNCTION_PARAMETERS) {
    zend_string* z_name;
    zval*        z_value;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &z_name, &z_value) == FAILURE) {
        zend_throw_error(NULL, "failed to __set");
        return;
    }
    class_store* c = class_store::get_store(getThis());
    std::string name(z_name->val, z_name->len);
    auto p = c->_abstract->_static_property.find(name);
    if(p != c->_abstract->_static_property.end() && p->second.second & ZEND_ACC_STATIC == 0) {
        zend_update_property(c->_abstract->_entry, getThis(), name.c_str(), name.length(), z_value);
    }else{
        c->_instance->__set(name, z_value);
    }
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
    param_t      param(z_argv, zend_hash_num_elements(Z_ARRVAL_P(z_argv)), true);
    std::string  name(ZSTR_VAL(z_name), ZSTR_LEN(z_name));
    
    auto func = ca->_static_method_map[name];
    if(func != nullptr) {
        value_t retval = func(param);
        ZVAL_ZVAL(return_value, &retval._value, 1, 0);
    }else{
        zend_throw_error(nullptr, "call to undefined static method \"%s\"", name.c_str());
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
    { (const char*) (zend_uintptr_t) (1), NULL, 0, 0, 0, 0},
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
};
static zend_internal_arg_info set_argv[] = {
    { (const char*) (zend_uintptr_t) (2), NULL, 0, 0, 0, 0},
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_INFO(0, value)
};
// TODO 继续支持其他魔术方法？
static zend_function_entry method_entry[] = {
    { "__construct", class_abstract::__construct, constructor_argv, 1, 0},
//   { "__destruct" , class_abstract::__destruct , destructor_argv , 0, ZEND_ACC_PUBLIC},
    { "__call"      , class_abstract::__call      , call_argv       , 2, ZEND_ACC_PUBLIC},
    { "__callStatic", class_abstract::__callStatic, call_argv       , 2, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC},
    { "__get"      , class_abstract::__get      , get_argv        , 1, ZEND_ACC_PUBLIC},
    { "__isset"    , class_abstract::__isset    , get_argv        , 1, ZEND_ACC_PUBLIC},
//    { "__set"    , class_abstract::_set    , set_argv        , 1, ZEND_ACC_PUBLIC},
    ZEND_FE_END
};

void class_abstract::module_startup() {
    zend_class_entry ce;
    class_abstract* _self = this;
    INIT_OVERLOADED_CLASS_ENTRY_EX(ce, _name.c_str(), _name.length(), method_entry, nullptr, nullptr, nullptr, nullptr, nullptr);
    _entry = zend_register_internal_class_ex(&ce, nullptr);
    for(auto i=_static_property.begin(); i!=_static_property.end(); ++i) {
        zend_declare_property(_entry, i->first.c_str(), i->first.length(), &i->second.first._value, i->second.second);
    }
    for(auto i=_object_property.begin(); i!=_object_property.end(); ++i) {
        zend_declare_property(_entry, i->first.c_str(), i->first.length(), const_cast<zval*>(&VALUE_NULL._value), i->second.second);
    }
    // 利用注释保存当前对象指针
    _entry->info.user.doc_comment = zend_string_init("\0", 2 + sizeof(class_abstract*), 1);
    memcpy(_entry->info.user.doc_comment->val + 1, &_self, sizeof(class_abstract*));
    _entry->create_object = create_object;
}

void class_abstract::module_shutdown() {
    _object_method_map.clear();
    _static_method_map.clear();
    _static_property.clear(); // 清理，静态属性初值也被清理掉了，所以不会引起 exit(1) 的问题
    _object_property.clear();
}

void class_abstract::request_startup() {
//    for(auto i=_static_property_map.begin();i!=_static_property_map.end();++i) {
//        *(i->second.first) = i->second.second;
//    }
}
void class_abstract::request_shutdown() {
//    // 静态属性在每次请求结束后需要还原
//    for(auto i=_property_map.begin();i!=_property_map.end();++i) {
//        i->second.first = std::get<1>(i->second);
//    }
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
    _store->_instance->_store = _store;
    return &_store->_object;
}


void class_abstract::add_object_method(const std::string& name, value_t(class_base::*method)(param_t& argv)) {
    class_abstract* _self = this;
    this->_object_method_map[name] = method;
}

void class_abstract::add_static_method(const std::string& name, value_t(*method)(param_t& argv)) {
    this->_static_method_map[name] = method;
}

void class_abstract::property(const std::string& name, const value_t& init, int flags) {
    // 属性的地址和初始值
    this->_static_property[name] = std::make_pair(init, flags|ZEND_ACC_STATIC);
}

void class_abstract::property(const std::string& name, object_property_t p, int flags) {
    // 属性的地址和初始值
    this->_object_property[name] = std::make_pair(p, flags);
}

}
