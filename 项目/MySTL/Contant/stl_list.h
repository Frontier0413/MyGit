#pragma once
#include <initializer_list>
#include "../Iterator/stl_iterator.h"
#include "../Allocator/stl_alloc.h"
#include "../Allocator/stl_uninitialized.h"
#include "../TypeTraits/IsInteger.h"
namespace hxl
{
template <class T>
struct __list_node
{
    typedef void *void_pointer;
    void_pointer prev;
    void_pointer next;
    T data;
};

//list的迭代器
template <class T, class Ref, class Ptr>
struct __list_iterator
{
    using iterator = __list_iterator<T, T &, T *>;
    //对于常迭代器类型，其对应的引用和指针为常类型
    using const_iterator = __list_iterator<T, const T &, const T *>;
    using self = __list_iterator<T, Ref, Ptr>;

    //迭代器类型标签
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = Ptr;
    using reference = Ref;

    using list_node = __list_node<T>;

    list_node *node;

    //构造函数，将给定的节点指针封装成迭代器。
    __list_iterator(list_node *x) : node(x) {}

    //默认构造函数
    __list_iterator() {}

    //拷贝构造函数
    __list_iterator(const iterator &x) : node(x.node) {}

    bool operator==(const self &x) const
    {
        return node == x.node;
    }

    bool operator!=(const self &x) const
    {
        return node != x.node;
    }
    //对于普通迭代器，返回引用类型，对于常迭代器，其ref为const T&。
    reference operator*() const
    {
        return (*node).data;
    }

    //对于成员访问运算符，它只能是非静态的函数形式，且没有参数
    //对于p->data的形式，加入p的成员访问运算符返回的是一个指针成员ptr，则调用ptr->data
    //加入返回的不是指针而是另一个类型对象q，则继续调用q->data，直到返回指针成员为止
    pointer operator->() const
    {
        return &(operator*());
    }

    //前置++
    self &operator++()
    {
        node = (list_node *)((*node).next);
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    //前置--
    self &operator--()
    {
        node = (list_node *)((*node).prev);
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
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using list_node = __list_node<T>;
    using iterator = __list_iterator<T, T &, T *>;
    using const_iterator = __list_iterator<T, const T &, const T *>;
    using list_node_allocator = simple_alloc<list_node, Alloc>;

protected:
    using void_pointer = void *;
    //node为尾后节点指针
    list_node *node;

    //申请一个节点所需的内存
    list_node *get_node()
    {
        return list_node_allocator::allocate();
    }

    //回收节点p的内存
    void put_node(list_node *p)
    {
        list_node_allocator::deallocate(p);
    }

    //申请一个节点，使用初值x初始化
    list_node *create_node(const T &x)
    {
        list_node *p = get_node();
        construct(&p->data, x);
        return p;
    }

    list_node *create_node()
    {
        list_node *p = get_node();
        construct(&p->data);
        return p;
    }

    //释放节点p的对象。回收内存
    void destory_node(list_node *p)
    {
        destory(&p->data);
        put_node(p);
    }

    void node_init()
    {
        node = get_node();
        node->next = node;
        node->prev = node;
    }

public:
    bool empty() const
    {
        return node->next == node;
    }

    size_type size() const
    {
        return distance(begin(), end());
    }

    size_type max_size() const { return size_type(-1); }

    reference front()
    {
        return *begin();
    }

    const_reference front() const
    {
        return *begin();
    }

    reference back()
    {
        return *(--end());
    }

    const_reference back() const
    {
        return *(--end());
    }

    //隐式调用iterator(list_node*)构造一个临时对象返回
    iterator begin()
    {
        return (list_node *)(node->next);
    }

    const_iterator begin() const
    {
        return (list_node *)(node->next);
    }

    iterator end()
    {
        return node;
    }

    const_iterator end() const
    {
        return node;
    }

    void swap(list<T, Alloc> &x)
    {
        std::swap(node, x.node);
    }

    //在pos位置，插入元素值为x的节点,返回指向插入元素的迭代器
    //实际上，list元素操作很多都是调用insert完成的
    iterator insert(iterator position, const T &x)
    {
        //创建并初始化节点
        list_node *tmp = create_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        ((list_node *)position.node->prev)->next = tmp;
        position.node->prev = tmp;
        return tmp;
    }

    iterator insert(iterator position)
    {
        list_node *tmp = create_node();
        tmp->next = (void *)position.node;
        tmp->prev = position.node->prev;
        ((list_node *)position.node->prev)->next = tmp;
        position.node->prev = tmp;
        return tmp;
    }

    //将first，last区间的元素插入到position位置
    //该函数首先判断first，last是否为整型，即该调用是否是(pos, size_type, value)的形式
    //如果是，则调用fill_insert.
    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last)
    {
        using Integer = typename Is_Integer<InputIterator>::Integer;
        insert_dispatch(position, first, last, Integer());
    }

protected:
    template <class Integer>
    void insert_dispatch(iterator position, Integer n, Integer x, __true_type)
    {
        fill_insert(position, (size_type)n, (T)x);
    }

    template <class InputIterator>
    void insert_dispatch(iterator position, InputIterator first, InputIterator last, __false_type)
    {
        //对于list，插入操作很快。而且对于输入迭代器只能遍历一次，因此依次调用insert插入元素即可
        for (; first != last; ++first)
            insert(position, *first);
    }

    void fill_insert(iterator position, size_type n, const T &x)
    {
        //插入n个元素的insert只需要调用n次insert就行
        for (; n > 0; --n)
            insert(position, x);
    }

public:
    void insert(iterator position, size_type n, const T &x)
    {
        fill_insert(position, n, x);
    }
    //所有的push操作直接调用insert完成
    void push_front(const T &x)
    {
        insert(begin(), x);
    }

    void push_front()
    {
        insert(begin());
    }

    void push_back(const T &x)
    {
        insert(end(), x);
    }

    void push_back()
    {
        insert(end());
    }

    //删除position位置的节点,返回指向删除位置下一元素的迭代器
    iterator erase(iterator position)
    {
        list_node *next_node = (list_node *)position.node->next;
        list_node *prev_node = (list_node *)position.node->prev;
        next_node->prev = prev_node;
        prev_node->next = next_node;
        destory_node(position.node);
        return iterator(next_node);
    }

    //删除first，last区间的元素
    iterator erase(iterator first, iterator last)
    {
        //stl原版，对于区间内每个元素调用一次erase函数
        //但是我认为，对于每个元素调用一次erase，假设区间有n个待删除元素，对于每个元素，都要让他的前节点的next指针指向后节点，后节点prev指针指向前节点，加上删除节点操作，大于3n次操作
        //直接将区间当做一个整体来看待，则只需要n+2次操作。
        // while (first != last)
        // {
        //     erase(first);
        //     ++first;
        // }
        list_node* next_node = last.node;
        list_node* prev_node = (list_node*)first.node->prev;
        prev_node->next = next_node;
        next_node->prev = prev_node;
        while(first != last)
        {   
            destory_node((first++).node);
        }
        return last;
    }

    void clear()
    {
        //删除除了node之外的所有节点，从头节点开始，依次回收节点
        list_node *cur = (list_node *)node->next;
        while (cur != node)
        {
            list_node *tmp = cur;
            cur = (list_node *)cur->next;
            destory_node(tmp);
        }
        node->next = node;
        node->prev = node;
    }

    //将链表长度重置为new_size，如果new_size大于原长度，新添加的元素用x初始化，否则删除多余的元素
    void resize(size_type new_size, const T &x)
    {
        iterator i = begin();
        size_type len = 0;
        for (; i != end() && len < new_size; ++i, ++len)
            ;
        if (len == new_size)
            erase(i, end());
        else
            insert(end(), new_size - len, x);
    }

    void resize(size_type new_size)
    {
        resize(new_size, T());
    }

    void pop_front()
    {
        erase(begin());
    }

    void pop_back()
    {
        erase(--end());
    }

    //将list所有元素替换为n个初值为value的元素
    void assign(size_type n, const T &value)
    {
        fill_assign(n, value);
    }

    void fill_assign(size_type n, const T &value)
    {
        iterator i = begin();
        for (; i != end() && n > 0; ++i, --n)
        {
            *i = value;
        }
        if (n > 0)
            insert(end(), n, value);
        else
            erase(i, end());
    }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        using Integer = typename Is_Integer<InputIterator>::Integer;
        assign_dispatch(first, last, Integer());
    }

    //将x的元素迁移到position位置之前
    void splice(iterator position, list &x)
    {
        if (!x.empty())
            this->transfer(position, x.begin(), x.end());
    }

    //将迭代器i指示的元素迁移到position位置之前
    void splice(iterator position, list &, iterator i)
    {
        iterator j = i;
        ++j;
        if (position == i || position == j)
            return;
        this->transfer(position, i, j);
    }

    //将区间first，last的元素迁移到position位置之前
    void splice(iterator position, list &, iterator first, iterator last)
    {
        if (first != last)
            this->transfer(position, first, last);
    }

    //删除所有值为value的节点
    void remove(const T &value)
    {
        iterator first = begin();
        iterator last = end();
        while (first != last)
        {
            iterator next = first;
            ++next;
            if (*first == value)
                erase(first);
            first = next;
        }
    }

    //删除连续且重复的节点
    void unique()
    {
        iterator first = begin();
        iterator last = end();
        if (first == last)
            return;
        iterator next = first;
        while (++next != last)
        {
            if (*first == *next)
                erase(next);
            else
                first = next;
            next = first;
        }
    }

    //将链表x有序合并到当前链表中，使用者应尽量保证list是sort之后的
    void merge(list &x)
    {
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();
        while (first1 != last1 && first2 != last2)
        {
            //如果first2指示的元素小于first1指示的元素，将其插入到first1之前
            if (*first2 < *first1)
            {
                iterator next = first2;
                transfer(first1, first2, ++next);
                first2 = next;
            }
            //如果不小于的话，继续寻找下一个插入点
            else
                ++first1;
        }
        //如果遍历完区间扔未找到位置插入剩余元素，则将剩余元素直接迁移到list之后
        if (first2 != last2)
            transfer(last1, first2, last2);
    }

    //翻转链表
    void reverse()
    {
        list_node *tmp = node;
        do
        {
            //交换当前节点的next指针与prev指针
            std::swap(tmp->next, tmp->prev);
            tmp = (list_node *)tmp->prev;
        } while (tmp != node);
    }

    void sort()
    {
        if (node->next != (void *)node && ((list_node *)node->next)->next != (void *)node)
        {
            list carry;
            list counter[64];
            int fill = 0;
            while (!empty())
            {
                carry.splice(carry.begin(), *this, begin());
                int i = 0;
                while (i < fill && !counter[i].empty())
                {
                    counter[i].merge(carry);
                    carry.swap(counter[i++]);
                }
                carry.swap(counter[i]);
                if (i == fill)
                    ++fill;
            }
            for (int i = 1; i < fill; ++i)
                counter[i].merge(counter[i - 1]);
            swap(counter[fill - 1]);
        }
    }

    //接受一个谓语参数，删除该满足该谓语的元素
    template <class Predicate>
    void remove_if(Predicate pred)
    {
        iterator first = begin();
        iterator last = end();
        while (first != last)
        {
            iterator next = first;
            ++next;
            if (pred(*first))
                erase(first);
            first = next;
        }
    }

    //接受一个二元谓词，使用该二元谓词代替==判断元素是否相等
    template <class BinaryPredicate>
    void unique(BinaryPredicate binary_pred)
    {
        iterator first = begin();
        iterator last = end();
        if (first == last)
            return;
        iterator next = first;
        while (++next != last)
        {
            if (binary_pred(*first, *next))
                erase(next);
            else
                first = next;
            next = first;
        }
    }

    //StrictWeakOrdering为严格弱序，stl容器中元素的比较规则
    template <class StrictWeakOrdering>
    void merge(list &x, StrictWeakOrdering comp)
    {
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first2, *first1))
            {
                iterator next = first2;
                transfer(first1, first2, ++next);
                first2 = next;
            }
            else
                ++first1;
        }
        if (first2 != last2)
            transfer(last1, first2, last2);
    }

    template <class StrictWeakOrdering>
    void sort(StrictWeakOrdering comp)
    {
        if (node->next != (void *)node && ((list_node *)node->next)->next != (void *)node)
        {
            list carry;
            list counter[64];
            int fill = 0;
            while (!empty())
            {
                carry.splice(carry.begin(), *this, begin());
                int i = 0;
                while (i < fill && !counter[i].empty())
                {
                    counter[i].merge(carry, comp);
                    carry.swap(counter[i++]);
                }
                carry.swap(counter[i]);
                if (i == fill)
                    ++fill;
            }

            for (int i = 1; i < fill; ++i)
                counter[i].merge(counter[i - 1], comp);
            swap(counter[fill - 1]);
        }
    }

protected:
    template <class Integer>
    void assign_dispatch(Integer n, Integer value, __true_type)
    {
        fill_assign((size_type)n, (T)value);
    }

    template <class InputIterator>
    void assign_dispatch(InputIterator first2, InputIterator last2, __false_type)
    {
        iterator first1 = begin();
        iterator last1 = end();
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
            *first1 = *first2;
        if (first2 == last2)
            erase(first1, last1);
        else
            insert(last1, first2, last2);
    }

    //将first,last区间元素移动到position之前
    //区间可以是其他链表的区间，也可以是自身链表区间，但调用者需要保证，position不处于区间中
    void transfer(iterator position, iterator first, iterator last)
    {
        if (position != last)
        {
            ((list_node *)(last.node->prev))->next = position.node;
            ((list_node *)(first.node->prev))->next = last.node;
            ((list_node *)(position.node->prev))->next = first.node;

            list_node *tmp = (list_node *)position.node->prev;
            position.node->prev = last.node->prev;
            last.node->prev = first.node->prev;
            first.node->prev = tmp;
        }
    }

public:
    list()
    {
        node_init();
    }

    list(size_type n, const T &value)
    {
        node_init();
        insert(begin(), n, value);
    }

    explicit list(size_type n)
    {
        node_init();
        insert(begin(), n, T());
    }

    template <class InputIterator>
    list(InputIterator first, InputIterator last)
    {
        node_init();
        insert(begin(), first, last);
    }

    list(const std::initializer_list<T> &l)
    {
        node_init();
        insert(begin(), l.begin(), l.end());
    }

    list(const list<T, Alloc> &x)
    {
        node_init();
        insert(begin(), x.begin(), x.end());
    }

    list<T, Alloc> &operator=(const list<T, Alloc> &x)
    {
        if (this != &x)
        {
            iterator first1 = begin();
            iterator last1 = end();

            const_iterator first2 = x.begin();
            const_iterator last2 = x.end();

            while (first1 != last1 && first2 != last2)
            {
                *first1 = *first2;
                ++first1;
                ++first2;
            }
            if (first2 == last2)
                erase(first1, last1);
            else
                insert(last1, first2, last2);
        }
        return *this;
    }

    ~list()
    {
        clear();
        put_node(node);
    }
};

template <class T, class Alloc>
inline bool operator==(const list<T, Alloc> &x, const list<T, Alloc> &y)
{
    using const_iterator = typename list<T, Alloc>::const_iterator;

    const_iterator end1 = x.end();
    const_iterator end2 = y.end();

    const_iterator i1 = x.begin();
    const_iterator i2 = y.begin();

    while (i1 != end1 && i2 != end2 && *i1 == *i2)
    {
        ++i1;
        ++i2;
    }
    //如果每个元素相等且元素长度相等，才认为两个list相等
    return i1 == end1 && i2 == end2;
}

template <class T, class Alloc>
inline bool operator<(const list<T, Alloc> &x, const list<T, Alloc> &y)
{
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, class Alloc>
inline bool operator!=(const list<T, Alloc> &x, const list<T, Alloc> &y)
{
    return !(x == y);
}

template <class T, class Alloc>
inline bool operator>(const list<T, Alloc> &x, const list<T, Alloc> &y)
{
    return y < x;
}

template <class T, class Alloc>
inline bool operator<=(const list<T, Alloc> &x, const list<T, Alloc> &y)
{
    return !(y < x);
}

template <class T, class Alloc>
inline bool operator>=(const list<T, Alloc> &x, const list<T, Alloc> &y)
{
    return !(x < y);
}

template <class T, class Alloc>
inline void swap(list<T, Alloc> &x, list<T, Alloc> &y)
{
    x.swap(y);
}
}