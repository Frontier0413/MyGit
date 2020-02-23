#pragma once
#include <initializer_list>
#include "../Contant/stl_tree.h"

namespace hxl
{
template <class Key, class Value, class Compare = less<Key>, class Alloc = alloc>
class map
{
public:
    using key_type = Key;
    using maped_type = Value;
    using value_type = pair<Key, Value>;

private:
    using rep_type = RB_tree<key_type, value_type, select1st<key_type, value_type>, Compare, Alloc>;
    rep_type t;

public:
    using iterator = typename rep_type::iterator;
    using const_iterator = typename rep_type::const_iterator;
    using reference = maped_type&;
    using size_type = typename rep_type::size_type;
    using pointer = maped_type*;

public:
    map(const Compare& comp = Compare()) : t(comp) { }

    template <class InputIterator>
    map(InputIterator first, InputIterator last) : t()
    {
        while (first != last)
        {
            t.insert_unique(*first);
            ++first;
        }
    }

    template <class T>
    map(std::initializer_list<T> l) : map(l.begin(), l.end()) { }

    ~map()
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
        return t.insert_unique(x);
    }

    iterator erase(iterator pos)
    {
        return t.erase(pos);
    }

    iterator find(const key_type& x)
    {
        return t.find(x);
    }

    reference operator[](key_type x)
    {
        iterator result = t.find(x);
        if (result == t.end())
            result = t.insert_unique(make_pair<key_type, maped_type>(x, maped_type())).first;
        return result->second;
    }   
};
}