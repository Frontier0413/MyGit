#pragma once 
#include "stl_tree.h"
#include <initializer_list>

namespace hxl
{

template <class Key, class Compare = less<Key>, class Alloc = alloc>
class multi_set
{
public:
    using key_type      = Key;
    using value_type    = Key;
    using key_compare   = Compare;

private:
    using rep_type = RB_tree<key_type, value_type, identity<key_type, value_type>, Compare, Alloc>;
    rep_type t;

public:
    using iterator = typename rep_type::iterator;
    using const_iterator = typename rep_type::const_iterator;

    multi_set(const Compare& comp = Compare()) : t(comp) { }

    template <class T>
    multi_set(std::initializer_list<T> l) : set(l.begin, l.end()) { }

    template <class InputIterator>
    multi_set(InputIterator first, InputIterator last) : t()
    {
        while(first != last)
        {
            t.insert_equal(*first);
            ++first;
        }
    }

    ~multi_set()
    {
        clear();
    }

    void clear()
    {
        t.clear();
    }

    iterator begin()
    {
        return t.begin();
    }

    iterator end()
    {
        return t.end();
    }

    void erase(const iterator& pos)
    {
        t.erase(pos);
    }

    pair<iterator, bool> insert(const value_type& x)
    {
        return t.insert_equal(x);
    }
    
};
}