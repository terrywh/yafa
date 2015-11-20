#include "../easy.h"

namespace easy {
    bool value_array::has(const std::string& key) const {
        return zend_hash_str_exists(Z_ARR(_value), key.c_str(), key.length()) != 0;
    }
    bool value_array::has(zend_ulong index) const {
        return zend_hash_index_exists(Z_ARR(_value), index) != 0;
    }
}