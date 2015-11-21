#include "../easy.h"

namespace easy {
    void argument_t::push(const value_t& v) {
        _args.push_back(v._value);
    }
    void argument_t::pop() {
        _args.pop_back();
    }
    void argument_t::insert(zend_ulong index, const value_t& v) {
        _args.insert(_args.begin() + index, v._value);
    }
    void argument_t::remove(zend_ulong index) {
        _args.erase(_args.begin() + index);
    }
}