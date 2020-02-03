#include "../Adapter/pair.h"
#include "../Allocator/stl_alloc.h"
#include "../Allocator/stl_construct.h"
#include "../Algorithm/stl_function.h"
#include "../Iterator/stl_iterator.h"

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
    RB_tree_node(const val& v, RB_tree_color col = RED, link_type* ptr = NIL) : left(ptr), right(ptr), parent(ptr),value(v), color(col) { }

    link_type maximum(link_type x)
    {
        while(x->right != NIL)
            x = x->right;
        return x;
    }

    link_type minimum(link_type x)
    {
        while(x->left != NIL)
            x = x->left;
        return x;
    }
};

//NIL节点永远为黑，所有指针初始时指向nullptr
template <class val>
static RB_tree_node<val>* RB_tree_node<val>::NIL = new RB_tree_node<val>(val(), BLACK, nullptr);


template <class val, class ref, class ptr>
class RB_tree_iterator
{

    using value_type            = val;
    using reference             = ref;
    using pointer               = ptr;
    using difference_type       = ptrdiff_t;
    using iterator_category     = bidirectional_iterator_tag;

    using self                  = RB_tree_iterator<val, ref, ptr>;
    using link_type             = RB_tree_node<val>*;

private:
    link_type node;
    using RB_tree_node<val>::NIL;

public:
    RB_tree_iterator(link_type x) : node(x) { }

    self& operator++()
    {
        if(node->right != NIL)
            node = RB_tree_node<value>::minimum(node->right);
        else 
        {
            link_type y = node->parent;
            link_type x = node;
            while(y != NIL && x != y->left)
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
        if(node->left != NIL)
            node = RB_tree_node<value_type>::maximum(node->left);
        else 
        {
            link_type x = node;
            link_type y = x->parent;
            while(y != NIL && x != y->right)
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
    link_type root = nullptr;
    size_type node_count;
    Compare key_comp;
    using RB_tree_node<Value>::NIL;

protected:
    link_type get_node()
    {
        return node_allocator::allocate();
    }

    //以x为作为value初值创建新节点，新节点color默认为RED
    link_type create_node(const Value& x)
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

public:
    RB_tree(const Compare& comp = Compare()) : node_count(0), key_comp(comp) { }

    iterator begin()
    {
        return RB_tree_node<value_type>::minimum(root);
    }

    iterator end()
    {
        return NIL;
    }

    bool empty()
    {
        return node_count == 0;
    }

    size_type size()
    {
        return node_count;
    }

    iterator insert_equal(const value_type& x);

    pair<iterator, bool> insert_unique(const value_type& x);

    void erase(iterator pos);

protected:
    void insert_fixup(link_type x);

    void erase_fixup(link_type x);
} ;