#include "../core.h"

namespace php {
    value class_base::__call(const std::string& name, const parameter& param) {
        zend_throw_error(nullptr, "undefined method \"%s\"", name.c_str());
        return value();
    }
        // static  value __callStatic(const std::string& name, param_t& param);
    value class_base::__get(const std::string& name) {
        zend_throw_error(nullptr, "undefined property \"%s\"", name.c_str());
        return value();
    }
    bool class_base::__isset(const std::string& name) {
        return false;
    }
    void class_base::__set(const std::string& name, const value& value) {
        zend_throw_error(nullptr, "undefined property \"%s\"", name.c_str());
    }
    
    class_wrap::class_wrap(const std::string& name):class_name(name) {
    }
    // 方法调用
    void class_wrap::__construct(INTERNAL_FUNCTION_PARAMETERS) {
        zval *z_argv;
        int z_size;
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &z_argv, &z_size) == FAILURE) {
            zend_throw_error(NULL, "failed to call __construct");
            return;
        }
        _store* p_store = get_store(getThis());
        parameter param(z_argv, z_size);
        p_store->self->__construct(param);
    }
    
    void class_wrap::__destruct(INTERNAL_FUNCTION_PARAMETERS) {
        _store* p_store = get_store(getThis());
        p_store->self->__destruct();
    }
    
    void class_wrap::__call(INTERNAL_FUNCTION_PARAMETERS) {
        zend_string* z_name;
        zval*        z_argv;
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &z_name, &z_argv) == FAILURE) {
            zend_throw_error(NULL, "failed to __call");
            return;
        }
        _store*     p_store = get_store(getThis());
        parameter   param(z_argv);
        std::string name(ZSTR_VAL(z_name), ZSTR_LEN(z_name));
        auto        p_method = p_store->wrap->m_object_method.find(name);
        if (p_method ==  p_store->wrap->m_object_method.end()) {
            value r = p_store->self->__call(name,param);   // 默认实现是 “未定义方法”
            RETURN_ZVAL(&r.val, 1, 0);
        } else {
            value r = (p_store->self->*(p_method->second))(param);
            RETURN_ZVAL(&r.val, 1, 0);
        }
    }
    
    void class_wrap::__get(INTERNAL_FUNCTION_PARAMETERS) {
        zend_string* z_name;
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &z_name) == FAILURE) {
            zend_throw_error(NULL, "failed to __get");
            return;
        }

        zend_property_info* info = zend_get_property_info(Z_OBJCE_P(getThis()), z_name, 1);
        if(info == nullptr) {
            _store* p_store = get_store(getThis());
            std::string name(ZSTR_VAL(z_name), ZSTR_LEN(z_name));
            value r = p_store->self->__get(name); // 默认实现是 ”未定义属性“
            RETURN_ZVAL(&r.val, 1, 0);
        }else{
            zval  rv, *v;
            v = zend_read_property(Z_OBJCE_P(getThis()), getThis(), ZSTR_VAL(z_name), ZSTR_LEN(z_name), 1, &rv);
            RETURN_ZVAL(v, 1, 0);
        }
    }

    void class_wrap::__set(INTERNAL_FUNCTION_PARAMETERS) {
        zend_string* z_name;
        zval*        z_val;
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &z_name, &z_val) == FAILURE) {
            zend_throw_error(nullptr, "failed to __set");
            return;
        }
        zend_property_info* info = zend_get_property_info(Z_OBJCE_P(getThis()), z_name, 1);
        if(info == nullptr) {
            _store* p_store = get_store(getThis());
            p_store->self->__set(std::string(z_name->val, z_name->len), value(z_val, false)); // 默认实现 “未定义属性”
            return;
        }
        if(info->flags & ZEND_ACC_PUBLIC == 0) {
            zend_throw_error(nullptr, "property \"%s\" is not public", z_name->val);
            return;
        }
        if(info->flags & ZEND_ACC_STATIC == 0) {
            zend_update_property_ex(Z_OBJCE_P(getThis()), getThis(), z_name, z_val);
        }else{
            zend_update_static_property(Z_OBJCE_P(getThis()), z_name->val, z_name->len, z_val);
        }
    }
    
    void class_wrap::__isset(INTERNAL_FUNCTION_PARAMETERS) {
        zend_string* z_name;
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &z_name) == FAILURE) {
            zend_throw_error(NULL, "failed to __isset");
            return;
        }
        zend_property_info* info = zend_get_property_info(Z_OBJCE_P(getThis()), z_name, 1);
        if(info == nullptr) {
            _store* p_store = get_store(getThis());
            RETURN_BOOL(p_store->self->__isset(std::string(z_name->val, z_name->len)));
        }
        RETURN_BOOL(true);
    }
    
    void class_wrap::__callStatic(INTERNAL_FUNCTION_PARAMETERS) {
        zend_string* z_name;
        zval*        z_argv;
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &z_name, &z_argv) == FAILURE) {
            zend_throw_error(NULL, "failed to __callStatic");
            return;
        }
        class_wrap*  p_wrap = *((class_wrap**)(execute_data->called_scope->info.user.doc_comment->val + 1));
        parameter    param(z_argv);
        std::string  name(ZSTR_VAL(z_name), ZSTR_LEN(z_name));
        
        auto p_method = p_wrap->m_static_method.find(name);
        if(p_method == p_wrap->m_static_method.end()) {
            zend_throw_error(nullptr, "undefined method \"%s\"", name.c_str());
            RETURN_NULL();
        }else{
            value r = p_method->second(param);
            RETURN_ZVAL(&r.val, 1, 0);
        }
        
    }
    // 添加属性
    class_wrap& class_wrap::property(const std::string& name, int flags) {
        m_property[name] = flags;
        return *this;
    }
   
    void class_wrap::add_object_method(const std::string& name, object_method_t method) {
        m_object_method[name] = method;
    }
    void class_wrap::add_static_method(const std::string& name, static_method_t method) {
        m_static_method[name] = method;
    }

    // -------------------------------------------------------------------------
    // 魔术方法声明
    ZEND_BEGIN_ARG_INFO(constructor_argv, 0)
    ZEND_ARG_VARIADIC_INFO(0, argv)
    ZEND_END_ARG_INFO()

    ZEND_BEGIN_ARG_INFO(destructor_argv, 0)
    ZEND_END_ARG_INFO()
    
    ZEND_BEGIN_ARG_INFO_EX(call_argv, 0, 0, 2)
        ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
        ZEND_ARG_ARRAY_INFO(0, argv, 0)
    ZEND_END_ARG_INFO()
    
    ZEND_BEGIN_ARG_INFO_EX(get_argv, 0, 0, 1)
        ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_END_ARG_INFO()
        
    ZEND_BEGIN_ARG_INFO_EX(set_argv, 0, 0 , 2)
        ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
        ZEND_ARG_INFO(0, value)
    ZEND_END_ARG_INFO()
    
    // TODO 继续支持其他魔术方法？
    static zend_function_entry method_entry[] = {
        { "__construct" , class_wrap::__construct , constructor_argv, 1, ZEND_ACC_PUBLIC},
        { "__destruct"  , class_wrap::__destruct  , destructor_argv , 0, ZEND_ACC_PUBLIC},
        { "__call"      , class_wrap::__call      , call_argv       , 2, ZEND_ACC_PUBLIC},
        { "__callStatic", class_wrap::__callStatic, call_argv       , 2, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC},
        { "__get"       , class_wrap::__get       , get_argv        , 1, ZEND_ACC_PUBLIC},
        { "__isset"     , class_wrap::__isset     , get_argv        , 1, ZEND_ACC_PUBLIC},
        { "__set"       , class_wrap::__set       , set_argv        , 2, ZEND_ACC_PUBLIC},
        ZEND_FE_END
    };
    void class_wrap::mstartup() {
        zend_class_entry ce, *p_ce;
        class_wrap*      p_wrap = this;
        INIT_OVERLOADED_CLASS_ENTRY_EX(ce, class_name.c_str(), class_name.length(), method_entry, nullptr, nullptr, nullptr, nullptr, nullptr);
        p_ce = zend_register_internal_class_ex(&ce, nullptr);
        for(auto i=m_property.begin(); i!=m_property.end(); ++i) {
            zend_declare_property_null(p_ce, i->first.c_str(), i->first.length(), i->second);
        }
        p_wrap->set_class_entry(p_ce);
        
        // 利用注释保存当前对象指针
        p_ce->info.user.doc_comment = zend_string_init("\0", 2 + sizeof(class_wrap*), 1);
        memcpy(p_ce->info.user.doc_comment->val + 1, &p_wrap, sizeof(class_wrap*));
        p_ce->create_object = create_object;
    }

    void class_wrap::mshutdown() {
        m_object_method.clear();
        m_static_method.clear();
        m_property.clear();
    }

    void class_wrap::rstartup() {}
    void class_wrap::rshutdown() {}

    zend_object* class_wrap::create_object(zend_class_entry* ce) {
        _store* p_store;
        p_store = (_store*)ecalloc(1, sizeof(_store) + zend_object_properties_size(ce));
        zend_object_std_init(&p_store->obj, ce);
        object_properties_init(&p_store->obj, ce);
        p_store->obj.handlers = &std_object_handlers;
        // 从 doc_comment 中恢复对象指针
        p_store->entry = ce;
        p_store->wrap = *((class_wrap**)(ce->info.user.doc_comment->val + 1));
        p_store->self = p_store->wrap->create();
        ZVAL_OBJ(&p_store->self->self, &p_store->obj);
        return &p_store->obj;
    }
}

