#pragma once
#include <initializer_list>
#include "../Contant/stl_tree.h"

namespace hxl
{
template <class Key, class Value, class Compare = less<Key>, class Alloc = alloc>
class multi_map
{
public:
    using key_type = Key;
    using value_type = pair<Key, Value>;

private:
    using rep_type = RB_tree<key_type, value_type, select1st<key_type, value_type>, Compare, Alloc>;
    rep_type t;

public:
    using iterator = typename rep_type::iterator;
    using const_iterator = typename rep_type::const_iterator;

public:
    multi_map(const Compare& comp = Compare()) : t(comp) { }

    template <class InputIterator>
    multi_map(InputIterator first, InputIterator last) : t()
    {
        while (first != last)
        {
            t.insert_equal(*first);
            ++first;
        }
    }

    template <class T>
    multi_map(std::initializer_list<T> l) : map(l.begin(), l.end()) { }

    ~multi_map()
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

    pair<iterator, bool> insert(const value_type& x)
    {
        return t.insert_equal(x);
    }

    iterator erase(iterator pos)
    {
        return t.erase(pos);
    }

    iterator find(const value_type& x)
    {
        return t.find(x);
    }
};
}