
php::value testFunction(const php::parameter& argv) {
    // return php::call_method_1(nullptr, "json_encode", std::string("abc"));
    php::value v("std");
    return v;
}

class testClass : public php::class_base {
public:
    static std::string class_name;
    static zend_class_entry* class_entry;
    php::value test(const php::parameter& argv);
};

php::value testClass::test(const php::parameter& param) {
    php::property prop(this);
    // prop.sset("save", "world");
    php::value v0 = prop.sget("save");
    
    v0["key1"] = "111";
    v0["key2"] = 222l;
    v0["key3"]["key4"] = "444";

    prop.sset("save", v0);
    return v0;
}

std::string       testClass::class_name;
zend_class_entry* testClass::class_entry;