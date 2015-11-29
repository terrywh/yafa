/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   value_iterator.h
 * Author: wuhao
 *
 * Created on 2015年11月30日, 上午12:41
 */

#ifndef VALUE_ITERATOR_H
#define VALUE_ITERATOR_H

namespace php {
   typedef struct {
        zend_string* key;
        zend_ulong   idx;
        zval*        val;
    } value_item;
class value_iterator {
public:
    value_iterator(zend_array* hash, bool start);
    
    value_iterator& operator++();
    value_iterator  operator++(int);
    
    bool operator ==(const value_iterator& that) const;
    bool operator !=(const value_iterator& that) const;
    
    value_item* operator ->();
    
    void reset();
    void end();
private:
    HashPosition pos;
    zend_array*  p_hash;
    value_item   item;
};
}

#endif /* VALUE_ITERATOR_H */

