#include "stl_hash_table.h"
#include "../Algorithm/stl_hash.h"
#include "../Algorithm/stl_function.h"

namespace hxl
{

template <class Key, class Val, class HashFcn = hash<Key>, class KeyEqual = KeyEqual<Key> , class ExtractKey = select1st<Key, pair<Key, Val>>, class Alloc = alloc>
class unordered_map
{
    using key_type = Key;
    using value_type = pair<Key, Val>;
    using maped_type = Val;
    using reference = maped_type&;
    using const_reference = const maped_type&;
    using pointer = maped_type*;
    using const_pointer = const maped_type*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using rep_type = hashtable<key_type, value_type, HashFcn, KeyEqual, ExtractKey, Alloc>;
    using iterator = typename rep_type::iterator;
    using const_iterator = typename rep_type::const_iterator;

private:
    rep_type t;

public:
    unordered_map() : t() { }

    ~unordered_map() { clear(); }

    iterator begin()
    {
        return t.begin();
    }

    iterator end()
    {
        return t.end();
    }

    size_type size()
    {
        return t.size();
    }

    bool empty()
    {
        return t.empty();
    }

    size_type buckets_count()
    {
        return t.bucket_count();
    }

    pair<iterator, bool> insert(const value_type& x)
    {
        return t.insert_unique(x);
    }

    void erase(const iterator& it)
    {
        t.erase(it);
    }

    void erase(const key_type& k)
    {
        t.erase(k);
    }

    iterator find(const key_type& x)
    {
        return t.find(x);
    }

    reference operator[](const key_type& x)
    {
        iterator result = t.find(x);
        if (result == end())
        {
            result = t.insert_unique(value_type(x, maped_type())).first;
        }
        return result->second;
    }

    void clear()
    {
        t.clear();
    }

};

}