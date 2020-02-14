#pragma once
#include "../Adapter/pair.h"
#include "../Allocator/stl_alloc.h"
#include "../Allocator/stl_construct.h"
#include "../Algorithm/stl_function.h"
#include "../Iterator/stl_iterator.h"

namespace hxl
{
using RB_tree_color = bool;
const RB_tree_color RED = false;
const RB_tree_color BLACK = true;

template <class val>
struct RB_tree_node
{
    using link_type = RB_tree_node<val>*;
    static link_type NIL;

    link_type left;
    link_type right;
    link_type parent;
    val value;
    RB_tree_color color;

    //创建新节点时，所有指针默认指向NIL，默认颜色为红色
    RB_tree_node(const val& v, RB_tree_color col = RED, link_type ptr = NIL) : left(ptr), right(ptr), parent(ptr),value(v), color(col) { }

    static link_type maximum(link_type x)
    {
        while(x->right != NIL)
            x = x->right;
        return x;
    }

    static link_type minimum(link_type x)
    {
        while(x->left != NIL)
            x = x->left;
        return x;
    }
};

//NIL节点永远为黑，所有指针初始时指向nullptr
template <class val>
RB_tree_node<val>* RB_tree_node<val>::NIL = new RB_tree_node<val>(val(), BLACK, nullptr);


template <class val, class ref, class ptr>
class RB_tree_iterator
{
public:
    using value_type            = val;
    using reference             = ref;
    using pointer               = ptr;
    using difference_type       = ptrdiff_t;
    using iterator_category     = bidirectional_iterator_tag;

    using self                  = RB_tree_iterator<val, ref, ptr>;
    using link_type             = RB_tree_node<val>*;

    link_type node;

public:
    RB_tree_iterator(link_type x) : node(x) { }

    self& operator++()
    {
        if(node->right != RB_tree_node<value_type>::NIL)
            node = RB_tree_node<value_type>::minimum(node->right);
        else 
        {
            link_type y = node->parent;
            link_type x = node;
            while(y != RB_tree_node<value_type>::NIL && x != y->left)
            {
                x = y;
                y = x->parent;
            }
            node = y;
        }
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        this->operator++();
        return tmp;
    }

    self& operator--()
    {
        if(node->left != RB_tree_node<value_type>::NIL)
            node = RB_tree_node<value_type>::maximum(node->left);
        else 
        {
            link_type x = node;
            link_type y = x->parent;
            while(y != RB_tree_node<value_type>::NIL && x != y->right)
            {
                x = y;
                y = x->parent;
            }
            node = y;
        }
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        this->operator--();
        return tmp;
    }

    reference operator*()
    {
        return node->value;
    }

    pointer operator->()
    {
        return (&operator*());
    }
};

template <class val, class ref, class ptr>
bool operator==(const RB_tree_iterator<val, ref, ptr>& x, const RB_tree_iterator<val, ref, ptr>& y)
{
    return x.node == y.node;
}

template <class val, class ref, class ptr>
bool operator!=(const RB_tree_iterator<val, ref, ptr>& x, const RB_tree_iterator<val, ref, ptr>& y)
{
    return x.node != y.node;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class RB_tree
{
public:
    using node_allocator        = simple_alloc<RB_tree_node<Value>, alloc>;
    using key_type              = Key;
    using value_type            = Value;
    using pointer               = value_type*;
    using const_pointer         = const value_type*;
    using reference             = value_type&;
    using const_reference       = const value_type&;
    using size_type             = size_t;
    using difference_type       = ptrdiff_t;
    using link_type             = RB_tree_node<value_type>*;
    using iterator              = RB_tree_iterator<value_type, reference, pointer>;
    using const_iterator        = RB_tree_iterator<value_type, const_reference, const_pointer>;

private:
    link_type root = RB_tree_node<value_type>::NIL;
    size_type node_count;
    Compare key_comp;
    link_type left_most = RB_tree_node<value_type>::NIL;

protected:
    link_type get_node()
    {
        return node_allocator::allocate();
    }

    //以x为作为value初值创建新节点，新节点color默认为RED
    link_type create_node(const value_type& x)
    {
        link_type tmp = get_node();
        construct(tmp, x);
        return tmp;
    }

    void put_node(link_type p)
    {
        node_allocator::deallocate(p);
    }

    //销毁p节点并回收内存
    void destory_node(link_type p)
    {
        destory(&p->value);
        put_node(p);
    }

    key_type key(const link_type& x)
    {
        return KeyOfValue()(x->value);
    }

    void update_left_most()
    {
        left_most = RB_tree_node<value_type>::minimum(root);
    }
public:
    RB_tree(const Compare& comp = Compare()) : node_count(0), key_comp(comp) { }

    ~RB_tree()
    {
        clear();
    }

    iterator begin()
    {
        return left_most;
    }

    iterator end()
    {
        return RB_tree_node<value_type>::NIL;
    }

    bool empty()
    {
        return node_count == 0;
    }

    size_type size()
    {
        return node_count;
    }

    iterator insert_equal(const value_type& x)
    {
        link_type new_node = create_node(x);
        link_type p = root;
        link_type y = RB_tree_node<value_type>::NIL;
        while(p != RB_tree_node<value_type>::NIL)
        {
            y = p;
            if(key_comp(key(p), key(new_node)))
                p = p->right;
            else 
                p = p->left;
        }
        new_node->parent = y;
        if(y == RB_tree_node<value_type>::NIL)
            root = new_node;
        else if(key_comp(key(y), key(new_node)))
            y->right = new_node;
        else 
            y->left = new_node;
        ++node_count;
        insert_fixup(new_node);
        update_left_most();
        return new_node;
    }

    pair<iterator, bool> insert_unique(const value_type& x)
    {
        link_type new_node = create_node(x);
        link_type p = root;
        link_type y = RB_tree_node<value_type>::NIL;
        while(p != RB_tree_node<value_type>::NIL)
        {
            y = p;
            if (!key_comp(key(p), key(new_node)) && !key_comp(key(new_node), key(p)))
            {
                destory_node(new_node);
                return make_pair<iterator, bool>(new_node, false);
            }
            else if (key_comp(key(new_node), key(p)))
                p = p->left;
            else 
                p = p->right;
        }
        new_node->parent = y;
        if(y == RB_tree_node<value_type>::NIL)
            root = new_node;
        else if (key_comp(key(new_node), key(y)))
            y->left = new_node;
        else 
            y->right = new_node;
        ++node_count;
        insert_fixup(new_node);
        update_left_most();
        return make_pair<iterator, bool>(new_node, true);
    }

    void erase(iterator pos)
    {
        //用y来记录被实际被删除的节点，x记录用来取代删除节点的节点
        link_type z = pos.node;
        RB_tree_color ori_color = z->color;
        link_type x = RB_tree_node<value_type>::NIL;
        link_type y;
        if (z->left == RB_tree_node<value_type>::NIL)
        {
            y = z;
            x = z->right;
            transplant(y, x);
        }
        else if (z->right == RB_tree_node<value_type>::NIL)
        {
            y = z;
            x = z->left;
            transplant(y, x);
        }
        else 
        {
            y = RB_tree_node<value_type>::minimum(z->right);
            ori_color = y->color;
            x = y->right;
            if (y->parent != z)
            {
                transplant(y, x);
                y->right = z->right;
                y->right->parent = y;
            }
            else 
            {
                x->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        --node_count;
        put_node(z);
        if (ori_color == BLACK)
            erase_fixup(x);
        
    }

    iterator find(const key_type& x)
    {
        link_type p = root;
        while (p != RB_tree_node<value_type>::NIL)
        {
            if (!key_comp( key(p), x) && !key_comp(x, key(p)) )
                return p;
            else if ( key_comp(key(p), x) )
                p = p->right;
            else 
                p = p->left;
        }
        return p;
    }

    void clear()
    {
        while (node_count != 0)
        {
            erase(root);
        }
    }
protected:
    void right_rotate(link_type x)
    {
        link_type y = x->left;
        transplant(x, y);
        x->left = y->right;
        if (x->left != RB_tree_node<value_type>::NIL)
            x->left->parent = x;
        y->right = x;
        x->parent = y;
    }

    void left_rotate(link_type x)
    {
        link_type y = x->right;
        transplant(x, y);
        x->right = y->left;
        if (x->right != RB_tree_node<value_type>::NIL)
            x->right->parent = x;
        y->left = x;
        x->parent = y;
    }

    //用y树取代x树
    void transplant(link_type x, link_type y)
    {
        if (x->parent == RB_tree_node<value_type>::NIL)
        {
            root = y;
        }
        else if (x == x->parent->left)
        {
            x->parent->left = y;
        }
        else 
        {
            x->parent->right = y;
        }
        y->parent = x->parent;
    }

    void insert_fixup(link_type z)
    {
        while (z->parent->color == RED)
        {
            if (z->parent == z->parent->parent->left)
            {
                link_type y = z->parent->parent->right;
                if (y->color == RED)
                {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z = z->parent->parent;
                }
                else 
                {
                    if (z == z->parent->right)
                    {
                        z = z->parent;
                        left_rotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    right_rotate(z->parent->parent);
                }
            }
            else 
            {
                link_type y = z->parent->parent->left;
                if (y->color == RED)
                {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else 
                {
                    if (z == z->parent->left)
                    {
                        z = z->parent;
                        right_rotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    left_rotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void erase_fixup(link_type z)
    {
        link_type y = RB_tree_node<value_type>::NIL;
        while (z->color == BLACK && z != root)
        {
            if (z == z->parent->left)
            {
                y = z->parent->right;
                if (y->color == RED)
                {
                    y->color = BLACK;
                    z->parent->color = RED;
                    left_rotate(z->parent);
                    y = z->parent->right;
                }
                if (y->left->color == BLACK && y->right->color == BLACK)
                {
                    y->color = RED;
                    z = z->parent;
                }
                else
                {
                    if (y->right->color == BLACK)
                    {
                        y->color = RED;
                        y->left->color = BLACK;
                        right_rotate(y);
                        y = z->parent->right;
                    }
                    y->color = z->parent->color;
                    z->parent->color = BLACK;
                    y->right->color = BLACK;
                    left_rotate(z->parent);
                    z = root;
                }
            }
            else 
            {
                y = z->parent->left;
                if (y->color == RED)
                {
                    y->color = BLACK;
                    z->parent->color = RED;
                    right_rotate(z->parent);
                    y = z->parent->left;
                }
                if (y->left->color == BLACK && y->right->color == BLACK)
                {
                    y->color = RED;
                    z = z->parent;
                }
                else 
                {
                    if (y->left->color == BLACK)
                    {
                        y->color = RED;
                        y->right->color = BLACK;
                        left_rotate(y);
                        y = z->parent->left;
                    }
                    y->color = z->parent->color;
                    z->parent->color = BLACK;
                    y->left->color = BLACK;
                    right_rotate(z->parent);
                    z = root;
                }
            }
        }
        z->color = BLACK;
    }
};
}