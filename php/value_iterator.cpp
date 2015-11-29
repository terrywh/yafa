/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   value_iterator.cpp
 * Author: wuhao
 * 
 * Created on 2015年11月30日, 上午12:41
 */

#include "../core.h"
namespace php{
    value_iterator::value_iterator(zend_array* hash, bool start):p_hash(hash) {
        if(start) reset();
        else end();
    }
    value_iterator& value_iterator::operator++() {
        if(zend_hash_move_forward_ex(p_hash, &pos) == FAILURE) {
            pos = -1;
        }
        return *this;
    }
    value_iterator  value_iterator::operator++(int) {
        if(zend_hash_move_forward_ex(p_hash, &pos) == FAILURE) {
            pos = -1;
        }
        return *this;
    }
    
    bool value_iterator::operator ==(const value_iterator& that) const {
        return p_hash == that.p_hash && pos == that.pos;
    }
    bool value_iterator::operator !=(const value_iterator& that) const {
        return p_hash != that.p_hash || pos != that.pos;
    }
    
    
    void value_iterator::reset() {
        zend_hash_internal_pointer_reset_ex(p_hash, &pos);
    }
    void value_iterator::end() {
        pos = -1;
    }
    value_item* value_iterator::operator ->() {
        zend_hash_get_current_key_ex(p_hash, &item.key, &item.idx, &pos);
        item.val = zend_hash_get_current_data_ex(p_hash, &pos);
        return &item;
    }
}
