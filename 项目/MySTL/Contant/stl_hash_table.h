#include "stl_vector.h"
#include "../Algorithm/stl_algo.h"
#include "../Adapter/pair.h"
namespace hxl
{

const static unsigned long stl_num_primes = 28;

static const unsigned long stl_prime_list[stl_num_primes] =
    {
        53ul, 97ul, 193ul, 389ul, 769ul,
        1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
        49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
        1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
        50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
        1610612741ul, 3221225473ul, 4294967291ul};

inline unsigned long stl_next_prime(unsigned long n)
{
    const unsigned long *first = stl_prime_list;
    const unsigned long *last = stl_prime_list + (int)stl_num_primes;
    const unsigned long *pos = lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

template <class Val>
struct hashtable_node
{
    Val value;
    hashtable_node *next;

    hashtable_node(Val v) : value(v), next(nullptr) {}
};

template <class Key, class Val, class HashFcn, class KeyEqual, class ExtractKey, class Alloc = alloc>
class hashtable;

template <class Key, class Val, class Ref, class Ptr, class HashFcn, class KeyEqual, class ExtractKey, class Alloc>
class hashtable_iterator
{
public:
    using node = hashtable_node<Val>;
    using self = hashtable_iterator<Key, Val, Ref, Ptr, HashFcn, KeyEqual, ExtractKey, Alloc>;
    using hashtable = hashtable<Key, Val, HashFcn, KeyEqual, ExtractKey, Alloc>;
    using reference = Ref;
    using pointer = Ptr;
    using value_type = Val;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator_category = forward_iterator_tag;

    node *cur;
    hashtable *ht;

public:
    hashtable_iterator(node *n, const hashtable *tab) : cur(n), ht(const_cast<hashtable*>(tab)) {}

    hashtable_iterator() {}

    hashtable_iterator(const hashtable_iterator &it) : cur(it.cur), ht(it.ht) {}

    self &operator=(const hashtable_iterator &it)
    {
        cur = it.cur;
        ht = it.ht;
        return *this;
    }

    reference operator*()
    {
        return cur->value;
    }

    pointer operator->()
    {
        return &(operator*());
    }

    bool operator==(const hashtable_iterator &it)
    {
        return cur == it.cur;
    }

    bool operator!=(const hashtable_iterator &it)
    {
        return cur != it.cur;
    }

    self &operator++()
    {
        node* old = cur;
        cur = cur->next;
        if (!cur)
        {
            size_type n = ht->bkt_num(old->value);
            while (!cur && ++n < ht->buckets.size())
                cur = ht->buckets[n];
        }
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }
};

template <class Key, class Val, class HashFcn, class KeyEqual, class ExtractKey, class Alloc>
class hashtable
{
public:
    using key_type = Key;
    using value_type = Val;
    using iterator = hashtable_iterator<key_type, value_type, value_type &, value_type *, HashFcn, KeyEqual, ExtractKey, Alloc>;
    using const_iterator = hashtable_iterator<key_type, value_type, const value_type &, const value_type *, HashFcn, KeyEqual, ExtractKey, Alloc>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using node = hashtable_node<value_type>;
    using reference = value_type &;
    using pointer = value_type *;
    using const_reference = const value_type &;
    using const_pointer = const value_type *;
    using node_allocator = simple_alloc<node, alloc>;
    friend class hashtable_iterator<key_type, value_type,value_type&, value_type*, HashFcn, KeyEqual, ExtractKey, Alloc>;

protected:
    node *get_node()
    {
        return node_allocator::allocate(1);
    }

    node *create_node(const value_type &x)
    {
        node *p = get_node();
        construct(p, x);
        return p;
    }

    void put_node(node *p)
    {
        destory(p);
        node_allocator::deallocate(p);
    }

private:
    HashFcn hasher;
    KeyEqual IfEqual;
    ExtractKey KeyOfValue;
    size_type elements_num;
    size_type buckets_num;
    vector<node*> buckets;

protected:
    void resize(size_type n)
    {
        size_type new_buckets_num = next_size(n);
        if (new_buckets_num > buckets_num)
        {
            buckets_num = new_buckets_num;
            vector<node *> tmp(buckets_num, nullptr);
            for (size_type i = 0; i < buckets.size(); ++i)
            {
                node *cur = buckets[i];
                while (cur)
                {
                    buckets[i] = buckets[i]->next;
                    size_type n = bkt_num(cur->value);
                    cur->next = tmp[n];
                    tmp[n] = cur;
                    cur = buckets[i];
                }
            }
            buckets.swap(tmp);
        }
    }

    size_type bkt_num(const value_type& obj)
    {
        return hasher(KeyOfValue(obj)) % buckets_num;
    }

    size_type bkt_num(const key_type& key)
    {
        return hasher(key) % buckets_num;
    }

    size_t next_size(size_type n)
    {
        return stl_next_prime(n);
    }

public:
    hashtable() : hasher(), IfEqual(), KeyOfValue(), elements_num(0), buckets_num(*stl_prime_list), buckets(buckets_num, nullptr) {}

    ~hashtable()
    {
        clear();
    }

    iterator begin()
    {
        for (size_type i = 0; i < buckets.size(); ++i)
        {
            if (buckets[i])
                return iterator(buckets[i], this);
        }
        return iterator(nullptr, this);
    }

    iterator end()
    {
        return iterator(nullptr, this);
    }

    bool empty()
    {
        return elements_num == 0;
    }

    size_type size()
    {
        return elements_num;
    }

    size_type bucket_count()
    {
        return buckets.size();
    }

    pair<iterator, bool> insert_unique(const value_type &x)
    {
        resize(elements_num + 1);
        size_type n = bkt_num(x);
        node *new_node = create_node(x);
        node *cur = buckets[n];
        while (cur)
        {
            if (IfEqual(KeyOfValue(new_node->value), KeyOfValue(cur->value)))
                return pair<iterator, bool>(iterator(cur, this), false);
            else
                cur = cur->next;
        }
        new_node->next = buckets[n];
        buckets[n] = new_node;
        ++elements_num;
        return pair<iterator, bool>(iterator(buckets[n], this), true);
    }

    iterator insert_equal(const value_type &x)
    {
        resize(elements_num + 1);
        node *new_node = create_node(x);
        size_type n = bkt_num(x);
        new_node->next = buckets[n];
        buckets[n] = new_node;
        ++elements_num;
        return iterator(buckets[n], this);
    }

    iterator find(const key_type &x)
    {
        size_type n = bkt_num(x);
        node *cur = buckets[n];
        while (cur)
        {
            if (IfEqual(KeyOfValue(cur->value), x))
                return iterator(cur, this);
            else
                cur = cur->next;
        }
        return iterator(nullptr, this);
    }

    size_type erase(const key_type &x)
    {
        size_type n = bkt_num(x);
        size_type erased = 0;
        node *cur = buckets[n];
        node *next = cur->next;
        while (next)
        {
            if (IfEqual(KeyOfValue(next->value), x))
            {
                cur->next = next->next;
                put_node(next);
                ++erased;
                --elements_num;
            }
            else
            {
                cur = next;
            }
            next = cur->next;
        }
        if (IfEqual(KeyOfValue(buckets[n]->value), x))
        {
            cur = buckets[n];
            buckets[n] = cur->next;
            put_node(cur);
            ++erased;
            --elements_num;
        }
        return erased;
    }

    void erase(const iterator& it)
    {
        size_type n = bkt_num(*it);
        node* cur = buckets[n];
        if (cur == it.cur)
        {
            buckets[n] = cur->next;
            put_node(cur);
            --elements_num;
        }
        else 
        {
            node* next = cur->next;
            while(next)
            {
                if (next == it.cur)
                {
                    cur->next = next->next;
                    put_node(next);
                    --elements_num;
                }
                else 
                {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }

    void clear()
    {
        for (size_type i = 0; i < buckets.size(); ++i)
        {
            node* cur = buckets[i];
            while(cur)
            {
                node* next = cur->next;
                put_node(cur);
                cur = next;
            }
            buckets[i] = nullptr;
        }
        elements_num = 0;
    }
};

} // namespace hxl