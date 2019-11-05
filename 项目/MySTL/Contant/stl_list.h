//list节点
#pragma once
#include "../Iterator/stl_iterator.h"
#include "../Allocator/stl_alloc.h"
#include "../Allocator/stl_uninitialized.h"
template <class T>
struct __list_node
{
    typedef void* void_pointer;
    void_pointer prev;
    void_pointer next;
    T data;
};

//list的迭代器
template <class T, class Ref, class Ptr>
struct __list_iterator
{
    //
    using iterator = __list_iterator<T, T&, T*>;
    using self = __list_iterator<T, Ref, Ptr>;

    //迭代器类型标签
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;
    //T类型__list_node指针类型
    using link_type = __list_node<T>*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    link_type node;

    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator& x) : node(x.node) {}

    bool operator==(const self& x) const
    {
        return node == x.node;
    }

    bool operator!=(const self& x) const
    {
        return node != x.node;
    }

    reference operator*() const
    {
        return (*node).data;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    self& operator++()
    {
        node = (link_type)( (*node).next );
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--()
    {
        node = (link_type)((*node).prev);
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

//list
template <class T, class Alloc = alloc>
class list
{
    public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using list_node = __list_node<T>;
    using iterator = __list_iterator<T, T&, T*>;
    using const_iterator = __list_iterator<T, const T&, const T*>;
    using list_node_allocator = simple_alloc<list_node, Alloc>;
 

    link_type get_node()
    {
        return list_node_allocator::allocate();
    }

    void put_node(link_type p)
    {
        list_node_allocator::deallcaote(p);
    }

    link_type create_node(const T& x)
    {
        link_type p = get_node();
        construct(&p->data, x);
        return p;
    }

    void destory_node(link_type p)
    {
        destory(&p->data);
        put_node(p);
    }
};