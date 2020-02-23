#pragma once
#include <initializer_list>
#include "../Iterator/stl_iterator.h"
#include "../Allocator/stl_alloc.h"
#include "../Allocator/stl_uninitialized.h"
#include "../TypeTraits/IsInteger.h"

namespace hxl
{
//返回每一块缓存区可容纳的元素数目
//默认每一块缓存为512字节，如果存储类型大于512字节，则每块缓存只存储一个元素
inline size_t __deque_buf_size(size_t size)
{
    return size < 512 ? size_t(512 / size) : size_t(1);
}

template <class T, class Ref, class Ptr>
struct _Deque_Iterator
{
    using iterator          = _Deque_Iterator<T, T&, T*>;
    using const_iterator    = _Deque_Iterator<T, const T&, const T*>;

    //返回每块缓存可存储的元素数量
    static size_t buffer_size()
    {
        return __deque_buf_size(sizeof(T));
    }

    //没有继承迭代器类的自定义迭代器需要自定义型别信息
    using iterator_category = random_access_iterator_tag;
    using value_type        = T;
    using pointer           = Ptr;
    using reference         = Ref;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;

    //指向用于指向缓存块的指针
    using map_pointer       = T**;
    using Self              = _Deque_Iterator;

    //last为当前缓存块的尾后指针
    T* cur;
    T* first;
    T* last;
    map_pointer node;

    //对y解引用，即得到该缓存快的首地址指针
    _Deque_Iterator(T* x, map_pointer y) 
      : cur(x), first(*y), last(*y + buffer_size()), node(y) { }

    _Deque_Iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

    _Deque_Iterator(const iterator& x)
      : cur(x.cur), first(x.first), last(x.last), node(x.node) { }

    reference operator*() const 
    {
        return *cur;
    }

    //返回cur指针
    pointer operator->() const
    {
        return cur;
    }


    difference_type operator-(const Self& x) const
    {
        //当前相距距离为相差的内存块数目乘以每个内存块元素数目加上x.cur到x.last的距离与cur到first的距离
        return difference_type(buffer_size()) * (node - x.node -1) + 
            (cur - first) + (x.last - x.cur);
    }

    //如果当前缓存快已经到结尾，则指向下一缓存快
    Self& operator++()
    {
        ++cur;
        //如果到达尾后位置
        if(cur == last)
        {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    Self operator++(int)
    {
        Self tmp = *this;
        ++*this;
        return tmp;
    }

    //如果当前到达缓存快首地址指针，则指向上一缓存快的尾地址
    Self& operator--()
    {
        if(cur == first)
        {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    Self& operator--(int)
    {
        Self tmp = *this;
        --*this;
        return tmp;
    }

    Self& operator+=(difference_type n)
    {
        //offset用于判断移动n距离之后是否还在当前缓存区内
        difference_type offset = n + (cur - first);
        //如果仍在当前缓存区，则直接对cur指针进行操作即可
        if(offset >= 0 && offset < difference_type(buffer_size()))
            cur += n;
        else 
        {
            difference_type node_offset = 
                offset > 0 ? offset / difference_type(buffer_size())
                            : -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }

    Self operator+(difference_type n) const
    {
        Self tmp = *this;
        return tmp += n;
    }

    Self& operator-=(difference_type n)
    {
        return *this += -n;
    }

    Self operator-(difference_type n) const
    {
        Self tmp = *this;
        return tmp -= n;
    }

    reference operator[](difference_type n) const 
    {
        return *(*this + n);
    }

    bool operator==(const Self& x) const 
    {
        return cur == x.cur;
    }

    bool operator!=(const Self& x) const 
    {
        return !(*this == x);
    }

    bool operator<(const Self& x) const 
    {
        return (node == x.node) ? (cur < x.cur) : (node < x.node);
    }

    bool operator>(const Self& x) const 
    {
        return x < *this;
    }

    bool operator<=(const Self& x) const 
    {
        return !(x < *this);
    }

    bool operator>=(const Self& x) const 
    {
        return !(*this < x);
    }

    void set_node(map_pointer new_node)
    {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }
};


template <class T, class Alloc = alloc>
class deque
{
public:
    using iterator          = _Deque_Iterator<T, T&, T*>;
    using const_iterator    = _Deque_Iterator<T, const T&, const T*>;
    using reverse_iterator  = __reverse_iterator<iterator>;
    using const_reverse_iterator = __reverse_iterator<const iterator>;

    using value_type        = T;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;

    //初始化容器，使容器能容纳num_elements个元素
    deque(size_t num_elements) : map(nullptr), map_size(0), start(), finish()
    {
        initialize_map(num_elements);
    }

    deque() : map(nullptr), map_size(0), start(), finish() { }

protected:

    //初始化map指针，使其都指向申请的缓冲区，保证两端各有一个备用的缓冲区
    void initialize_map(size_t num_elements)
    {
        //计算出所需缓冲区数目
        size_t num_nodes = num_elements / __deque_buf_size(sizeof(T)) + 1;
        //map大小为8与当前所需map大小加2的和求中取较大的那个,即每次至少多配置两个缓冲区
        map_size = max((size_t)initial_map_size, num_nodes + 2);

        map = allocate_map(map_size);

        //双端队列应该保证至少两端各留一个缓冲区备用
        //实际上此时左右两端的缓冲只有map指针，而并没有申请实际的内存
        T** nstart = map + (map_size - num_nodes) / 2;
        //此时nstart,nfinish仍任是前闭后开区间
        T** nfinish = nstart + num_nodes;

        try 
        {
            create_nodes(nstart, nfinish);
        }catch(...)
        {
            deallocate_map(map, map_size);
            map = nullptr;
            map_size = 0;
        }
        start.set_node(nstart);
        finish.set_node(nfinish - 1);
        start.cur = start.first;
        finish.cur = finish.first + num_elements % __deque_buf_size(sizeof(T));
    }

    //对区间[nstart,nfinish)内每一个指针申请一个缓冲区内存
    void create_nodes(T** nstart, T** nfinish)
    {
        T** cur;
        try{
            for(cur = nstart; cur < nfinish; ++cur)
                *cur = allocate_node();
        }catch(...)
        {
            destory_nodes(nstart, cur);
        }
    }

    //回收区间[nstart,nfinish)指针所指向的缓冲区
    void destory_nodes(T** nstart, T** nfinish)
    {
        for(T** n = nstart; n < nfinish; ++n)
            deallocate_node(*n);
    }


protected:
    //指向map首地址的指针
    T** map;
    //记录当前map的大小
    size_t map_size;
    //第一个缓冲区的第一个元素
    iterator start;
    //最后一个缓冲区的最后一个元素
    iterator finish;

    enum { initial_map_size = 8 };
    //用于分配缓冲区的配置器
    using node_alloc_type = simple_alloc<T, Alloc>;
    //用于分配map的配置器
    using map_alloc_type  = simple_alloc<T*, Alloc>;

    using map_pointer = pointer*;

protected:

    //分配一个缓冲区
    T* allocate_node()
    {
        return node_alloc_type::allocate(__deque_buf_size(sizeof(T)));
    }
    
    //回收缓冲区p的内存
    void deallocate_node(T* p)
    {
        node_alloc_type::deallocate(p, __deque_buf_size(sizeof(T)));
    }

    //分配n个map指针所需的内存
    T** allocate_map(size_t n)
    {
        return map_alloc_type::allocate(n);
    }

    //回收map内存
    void deallocate_map(T** p, size_t n)
    {
        map_alloc_type::deallocate(p, n);
    }

    static size_t buffer_size()
    {
        return __deque_buf_size(sizeof(T));
    }

public:
    iterator begin() { return start; }
    iterator end() { return finish; }
    const_iterator begin() const { return start; }
    const_iterator end() const { return finish; }

    //调用迭代器的[]运算符实现随机访问,下标n其实就是相对于start迭代器偏移n个单位的元素
    reference operator[](size_type n)
    {
        return start[difference_type(n)];
    }

    const_reference operator[](size_type n) const 
    {
        return start[difference_type(n)];
    }

    //相对来说较为安全的访问元素方式，在调用下标运算符之前会先检查该下标是否合法
    void range_check(size_type n) const 
    {
        if(n >= this->size())
            throw("deque");
    }

    reference at(size_type n)
    {
        range_check(n);
        return (*this)[n];
    }

    const_reference at(size_type n) const 
    {
        range_check(n);
        return (*this)[n];
    }

    reference front() 
    {
        return *start;
    }

    reference back()
    {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    const_reference front() const 
    {
        return *start;
    }

    const_reference back() const
    {
        const_iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    //通过迭代器的-运算符，得到尾后迭代器与头迭代器的距离
    size_type size() const 
    {
        return finish - start;
    }

    size_type max_size() const 
    {
        return size_type(-1);
    }

    bool empty() const 
    {
        return finish == start;
    }

public:
    //拷贝构造函数
    deque(const deque& x) : deque(x.szie())
    {
        uninitialized_copy(x.begin(), x.end(), start);
    }

    //列表初始化
    deque(std::initializer_list<T> l) : deque(l.size())
    {
        uninitialized_copy(l.begin(), l.end(), start);
    }

    //将deque初始化为含有n个值为value的元素
    deque(size_type n, const value_type& value) : deque(n)
    {
        fill_initialize(value);
    }

    //接受迭代器范围的构造函数
    template <class InputIterator>
    deque(InputIterator first, InputIterator last) : deque()
    {
        using Integer = typename Is_Integer<InputIterator>::Integer;
        initialize_dispatch(first, last, Integer());
    }

private:
    template <class Integer>
    void initialize_dispatch(Integer n, Integer value, __true_type)
    {
        initialize_map(n);
        fill_initialize(value);
    }

    template <class InputIterator>
    void initialize_dispatch(InputIterator first, InputIterator last, __false_type)
    {
        range_initialize(first, last, iterator_category(first));
    }

public:
    //析构函数，检测是否使用了内存，如果map不为空，即表明deque使用了动态内存，则需要手动释放
    ~deque()
    {
        if(map)
        {
            destory(start, finish);
            destory_nodes(start.node, finish.node + 1);
            deallocate_map(map, map_size);
        }
    }

    //拷贝赋值运算符
    deque& operator=(const deque& x)
    {
        const size_type len = size();
        //检测自赋值
        if(&x != this)
        {
            //如果x长度小于deque长度，将x元素拷贝至容器内，再将剩余元素删除
            if(len >= x.size())
                erase(copy(x.begin(), x.end(), start), finish);
            //如果x长度大于deque长度，对于已有的元素，采用赋值运算覆盖原元素，随后在finish位置调用insert插入剩余元素
            else 
            {
                const_iterator mid = x.begin() + difference_type(len);
                copy(x.begin(), mid, start);
                insert(finish, mid, x.end());
            }
        }
        return *this;
    }

    void swap(deque& x)
    {
        std::swap(start, x.start);
        std::swap(finish, x.finish);
        std::swap(map, x.map);
        std::swap(map_size, x.map_size);
    }

public:
    void assign(size_type n, const value_type& value)
    {
        fill_assign(n, value);
    }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        using Integer = typename Is_Integer<InputIterator>::Integer;
        assign_dispatch(first, last, Integer());
    }

private:
    void fill_assign(size_type n, const value_type& value)
    {
        if(n > size())
        {
            fill(begin(), end(), value);
            insert(end(), n - size(), value);
        }
        else 
        {
            erase(begin() + n, end());
            fill(begin(), end(), value);
        }
    }

    template <class Integer>
    void assign_dispatch(Integer n, Integer value, __true_type)
    {
        fill_assign(size_type(n), value_type(value));
    }

    template <class InputIterator>
    void assign_dispatch(InputIterator first, InputIterator last, __false_type)
    {
        assign_aux(first, last, iterator_category(first));
    }

    template <class InputIterator>
    void assign_aux(InputIterator first, InputIterator last, input_iterator_tag)
    {
        iterator cur = begin();
        for(; first != last && cur != end(); ++cur, ++first)
            *cur = *first;
        if(first == last)
            erase(cur, end());
        else 
            insert(end(), first, last);
    }

    template <class ForwardIterator>
    void assign_aux(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
    {
        size_type len = distance(first, last);
        if(len > size())
        {
            ForwardIterator mid = first;
            advance(mid, size());
            copy(first, mid, begin());
            insert(end(), mid. last);
        }
        else 
        {
            erase(copy(first, last, begin()), end());
        }
    }

public:
    void push_back(const value_type& t)
    {
        if(finish.cur != finish.last - 1)
        {
            construct(finish.cur, t);
            ++finish.cur;
        }
        else 
            push_back_aux(t);
    }

    void push_front(const value_type& t)
    {
        if(start.cur != start.first)
        {
            construct(start.cur - 1, t);
            --start.cur;
        }
        else 
            push_front_aux(t);
    }

    void pop_back()
    {
        if(finish.cur != finish.first)
        {
            --finish.cur;
            destory(finish.cur);
        }
        else 
            pop_back_aux();
    }

    void pop_front()
    {
        if(start.cur != start.last - 1)
        {
            destory(start.cur);
            ++start.cur;
        }
        else 
            pop_front_aux();
    }

public:
    //在pos位置前插入元素x，后续元素向后移动，返回指向该元素x的迭代器
    iterator insert(iterator position, const value_type& x)
    {
        if(position.cur == start.cur)
        {
            push_front(x);
            return start;
        }
        else if(position.cur == finish.cur)
        {
            push_back(x);
            iterator tmp = finish;
            --tmp;
            return tmp;
        }
        else 
        {
            //如果不在头位置或者尾后位置，则调用辅助函数
            return insert_aux(position, x);
        }
    }

    void insert(iterator position, size_type n, const value_type& x)
    {
        fill_insert(position, n, x);
    }

    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last)
    {
        using Integer = typename Is_Integer<InputIterator>::Integer;
        insert_dispatch(position, first, last, Integer());
    }


private:
    void fill_insert(iterator position, size_type n, const value_type& x)
    {
        if(position.cur == start.cur)
        {
            iterator new_start = reserve_elements_at_front(n);
            try 
            {
                uninitialized_fill(new_start, start, x);
                start = new_start;
            }catch(...)
            {
                destory_nodes(new_start.node, start.node);
            }
        }
        else if(position.cur == finish.cur)
        {
            iterator new_finish = reserve_elements_at_back(n);
            try 
            {
                uninitialized_fill(finish, new_finish, x);
                finish = new_finish;
            }
            catch(...)
            {
                destory_nodes(finish.node + 1, new_finish.node + 1);
            }
        }
        else 
            insert_aux(position, n, x);
    }

    template <class Integer>
    void insert_dispatch(iterator position, Integer n, Integer x, __true_type)
    {
        fill_insert(position, (size_type) n, (value_type) x);
    }

    template <class InputIterator>
    void insert_dispatch(iterator position, InputIterator first, InputIterator last, __false_type)
    {
        insert(position, first, last, iterator_category(first));
    }

public:
    //删除pos迭代器指代的元素，返回pos下一位置的迭代器
    iterator erase(iterator position)
    {
        iterator next = position;
        ++next;
        difference_type index = position - start;
        if(size_type(index) < (this->size() >> 1))
        {
            copy_backward(start, position, next);
            pop_front();
        }
        else 
        {
            copy(next, finish, position);
            pop_back();
        }
        return start + index;
    }

    //删除区间[first, last)区间的元素，返回指向删除区间下一元素的迭代器
    iterator erase(iterator first, iterator last)
    {
        if(first == start && last == finish)
        {
            clear();
            return finish;
        }
        else 
        {
            difference_type n = last - first;
            difference_type elems_before = first - start;
            //如果待删除区间之前的元素更少，则将更少的元素向后移动更效率
            //这里并没有直接比较删除区间之前的元素与之后的元素，而是判断与剩余元素的一半的大小关系,因为deque迭代器的减法操作可能更费时
            if(elems_before < difference_type( (this->size() - n) / 2 ))
            {
                copy_backward(start, first, last);
                iterator new_start = start + n;
                destory(start, new_start);
                destory_nodes(start.node, new_start.node);
                start = new_start;
            }
            else 
            {
                copy(last, finish, first);
                iterator new_finish = finish - n;
                destory(new_finish, finish);
                destory_nodes(new_finish.node + 1, finish.node + 1);
                finish = new_finish;
            }
            return start + elems_before;
        }
    }

    void clear()
    {
        //start与finish的node可能是同一个缓冲区，因此判断从start.node + 1开始，避免同一缓冲区时未初始化的内存被调用析构函数
        for(map_pointer node = start.node + 1;
            node < finish.node; ++node)
        {
            destory(*node, *node + buffer_size());
            deallocate_node(*node);
        }
        //如果不是同一缓冲区，则说明start的缓冲区尚未被释放
        if(start.node != finish.node)
        {
            destory(start.cur, start.last);
            destory(finish.first, finish.cur);
            //这里只释放了finish.node指向的缓冲区，start.node的缓冲区留作备用
            deallocate_node(finish.first);
        }
        //指向同一缓冲区时，释放元素即可，唯一一块缓冲区留作备用
        else 
        {
            destory(start.cur, finish.cur);
        }

        finish = start;
    }

protected:

    //填充初始化
    void fill_initialize(const value_type& value)
    {
        map_pointer cur;
        try 
        {
            for(cur = start.node; cur < finish.node; ++cur)
                uninitialized_fill(*cur, *cur + buffer_size(), value);
            uninitialized_fill(finish.first, finish.cur, value);
        }catch(...)
        {
            destory(start, iterator(*cur, cur));
        }
    }

    // 对于输入迭代器，由于无法事先知道序列大小，因此对每个元素依次调用push_back函数
    template <class InputIterator>
    void range_initialize(InputIterator first, InputIterator last, input_iterator_tag)
    {
        initialize_map(0);
        try 
        {
            for(; first != last; ++first)
                push_back(*first);
        }catch(...)
        {
            clear();
        }
    }

    // 对于前向迭代器，先求出序列长度，初始化map之后以此对每个元素初始化
    template <class ForwardIterator>
    void range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
    {
        size_type n = distance(first, last);
        initialize_map(n);

        map_pointer cur_node;
        try 
        {
            for(cur_node = start.node;
                cur_node < finish.node; ++cur_node)
            {
                ForwardIterator mid = first;
                //每次拷贝缓冲区元素数目个元素
                advance(mid, buffer_size());
                uninitialized_copy(first, mid, *cur_node);
                first = mid;
            }
            uninitialized_copy(first, last, finish.first);
        }catch(...)
        {
            destory(start, iterator(*cur_node, cur_node));
        }
    }

    //在调用辅助函数之前已经判断，调用情况为边界条件，因此直接申请内存而无需去判断
    void push_back_aux(const value_type& t)
    {
        value_type t_copy = t;
        reserve_map_at_back();
        *(finish.node + 1) = allocate_node();
        try 
        {
            construct(finish.cur, t_copy);
            finish.set_node(finish.node + 1);
            finish.cur = finish.first;
        }catch(...)
        {
            deallocate_node(*(finish.node + 1));
        }
    }

    void push_front_aux(const value_type& t)
    {
        value_type t_copy = t;
        reserve_map_at_front();
        *(start.node - 1) = allocate_node();
        try 
        {
            start.set_node(start.node - 1);
            start.cur = start.last - 1;
            construct(start.cur, t_copy);
        }catch(...)
        {
            ++start;
            deallocate_node(*(start.node - 1));
        }
    }

    void pop_back_aux()
    {  
        deallocate_node(finish.first);
        finish.set_node(finish.node - 1);
        finish.cur = finish.last - 1;
        destory(finish.cur);
    }

    void pop_front_aux()
    {
        destory(start.cur);
        deallocate_node(start.first);
        start.set_node(start.node + 1);
        start.cur = start.first;
    }

    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last, input_iterator_tag)
    {
        copy(first, last, inserter(*this, position));
    }

    template <class ForwardIterator>
    void insert(iterator position, ForwardIterator first, ForwardIterator last, forward_iterator_tag)
    {
        size_type n = distance(first, last);
        if(position.cur == start.cur)
        {
            iterator new_start = reserve_elements_at_front(n);
            try 
            {
                uninitialized_copy(first, last, new_start);
                start = new_start;
            }catch(...)
            {
                destory_nodes(new_start.node, start.node);
            }
        }
        else if(position.cur == finish.cur)
        {
            iterator new_finish = reserve_elements_at_back(n);
            try 
            {
                uninitialized_copy(first, last, finish);
                finish = new_finish;
            }catch(...)
            {
                destory_nodes(finish.node + 1, new_finish.node + 1);
            }
        }
        else 
        {
            insert_aux(position, first, last, n);
        }
    }

    iterator insert_aux(iterator position, const value_type& x)
    {
        difference_type index = position - start;
        value_type x_copy = x;
        if(size_type(index) < this->size() / 2)
        {
            //如果内存不够的话，push_front函数会处理
            push_front(front());
            iterator front1 = start;
            ++front1;
            iterator front2 = front1;
            ++front2;
            position = start + index;
            iterator position1 = position;
            ++position1;
            copy(front2, position1, front1);
        }
        else 
        {
            push_back(back());
            iterator back1 = finish;
            --back1;
            iterator back2 = back1;
            --back2;
            position = start + index;
            copy_backward(position, back2, back1);
        }
        *position = x_copy;
        return position;
    }

    void insert_aux(iterator position, size_type n, const value_type& x)
    {
        const difference_type elems_before = position - start;
        size_type length = this->size();
        value_type x_copy = x;
        if(elems_before < difference_type(length / 2))
        {
            iterator new_start = reserve_elements_at_front(n);
            iterator old_start = start;
            position = start + elems_before;
            try 
            {
                if(elems_before >= difference_type(n))
                {
                    iterator start_n  = start + difference_type(n);
                    uninitialized_copy(start, start_n, new_start);
                    start = new_start;
                    copy(start_n, position, old_start);
                    fill(position - difference_type(n), position, x_copy);
                }
                else 
                {
                    uninitialized_copy_fill(start, position, new_start, start, x_copy);
                    start = new_start;
                    fill(old_start, position, x_copy);
                }
            }catch(...)
            {
                destory_nodes(new_start.node, start.node);
            }
        }
        else 
        {
            iterator new_finish = reserve_elements_at_back(n);
            iterator old_finish = finish;
            const difference_type elems_after = difference_type(length) - elems_before;
            position = finish - elems_after;
            try 
            {
                if(elems_after > difference_type(n))
                {
                    iterator finish_n = finish - difference_type(n);
                    uninitialized_copy(finish_n, finish, finish);
                    finish = new_finish;
                    copy_backward(position, finish_n, old_finish);
                    fill(position, position + difference_type(n), x_copy);
                }
                else 
                {
                    uninitialized_fill_copy(finish, position + difference_type(n), x_copy, position, finish);
                    finish = new_finish;
                    fill(position, old_finish, x_copy);
                }
            }catch(...)
            {
                destory_nodes(finish.node + 1, new_finish.node + 1);
            }
        }
    }

    template <class ForwardIterator>
    void insert_aux(iterator position, ForwardIterator first, ForwardIterator last, size_type n)
    {
        const difference_type elems_before = position - start;
        size_type length = size();
        if(elems_before < length / 2)
        {
            iterator new_start = reserve_elements_at_front(n);
            iterator old_start = start;
            position = start + elems_before;
            try 
            {
                if(elems_before >= difference_type(n))
                {
                    iterator start_n = start + difference_type(n);
                    uninitialized_copy(start, start_n, new_start);
                    start = new_start;
                    copy(start_n, position, old_start);
                    copy(first, last, position - difference_type(n));
                }
                else 
                {
                    ForwardIterator mid = first;
                    advance(mid, difference_type(n) - elems_before);
                    uninitialized_copy_copy(start, position, first, mid, new_start);
                    start = new_start;
                    copy(mid, last, old_start);
                }
            }catch(...)
            {
                destory_nodes(new_start.node, start.node);
            }
        }
        else 
        {
            iterator new_finish = reserve_elements_at_back(n);
            iterator old_finish = finish;
            const difference_type elems_after = difference_type(length) - elems_before;
            position = finish - elems_after;
            try 
            {
                if(elems_after > difference_type(n))
                {
                    iterator finish_n = finish - difference_type(n);
                    uninitialized_copy(finish_n, finish, finish);
                    finish = new_finish;
                    copy_backward(position, finish_n, old_finish);
                    copy(first, last, position);
                }
                else 
                {
                    ForwardIterator mid = first;
                    advance(mid, elems_after);
                    uninitialized_copy_copy(mid, last, position, finish, finish);
                    finish = new_finish;
                    copy(first, mid, position);
                }
            }catch(...)
            {
                destory_nodes(finish.node + 1, new_finish.node + 1);
            }
        }
    }

    iterator reserve_elements_at_front(size_type n)
    {
        size_type vacancies = start.cur - start.first;
        if(n > vacancies)
            new_elements_at_front(n - vacancies);
        return start - difference_type(n);
    }

    iterator reserve_elements_at_back(size_type n)
    {
        size_type vacancies = (finish.last - finish.cur) - 1;
        if(n > vacancies)
            new_elements_at_back(n - vacancies);
        return finish + difference_type(n);
    }

    void new_elements_at_front(size_type new_elements)
    {
        size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
        reserve_map_at_front(new_nodes);
        size_type i;
        try 
        {
            for(i = 1; i <= new_nodes; ++i)
                *(start.node - i) = allocate_node();
        }catch(...)
        {
            for(size_type j = 1; j < i; ++j)
                deallocate_node(*(start.node - j));
            throw;
        }
    }

    void new_elements_at_back(size_type new_elements)
    {
        size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
        reserve_map_at_back(new_nodes);
        size_type i;
        try 
        {
            for(i = 1; i <= new_nodes; ++i)
                *(finish.node + 1) = allocate_node();
        }catch(...)
        {
            for(size_type j = 1; j < i; ++j)
                deallocate_node(*(finish.node + j));
        }
    }

    void reserve_map_at_back(size_type nodes_to_add = 1)
    {
        if(nodes_to_add + 1 > map_size - (finish.node - map))
            reallocate_map(nodes_to_add, false);
    }

    void reserve_map_at_front(size_type nodes_to_add = 1)
    {
        if(nodes_to_add > size_type(start.node - map))
            reallocate_map(nodes_to_add, true);
    }

    void reallocate_map(size_type nodes_to_add, bool add_at_front)
    {
        size_type old_num_nodes = finish.node - start.node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_nstart;
        if(map_size > 2 * new_num_nodes)
        {
            //将剩余map平均分配给前后两端，如果是在前端插入新节点，则将原start向后移动需要添加的节点数，保证足够插入元素
            new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            if(new_nstart < start.node)
                copy(start.node, finish.node + 1, new_nstart);
            else 
                copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
        }
        else 
        {
            //当前map不够存储节点数目时，新的map大小为原map大小加上原map大小与新加入的节点数中取大的那个，最后再加上2(预留给前后两端各一个)
            size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;

            map_pointer new_map = allocate_map(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                                    + (add_at_front ? nodes_to_add : 0);
            copy(start.node, finish.node + 1, new_nstart);
            deallocate_map(map, map_size);

            map = new_map;
            map_size = new_map_size;
        }

        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);
    }
};

template <class T, class Alloc>
inline bool operator==(const deque<T, Alloc>& x, const deque<T, Alloc>& y)
{
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc>
inline bool operator<(const deque<T, Alloc>& x, const deque<T, Alloc>& y)
{
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, class Alloc>
inline bool operator!=(const deque<T, Alloc>& x, const deque<T, Alloc>& y)
{
    return !(x == y);
}

template <class T, class Alloc>
inline bool operator>(const deque<T, Alloc>& x, const deque<T, Alloc>& y)
{
    return y < x;
}

template <class T, class Alloc>
inline bool operator<=(const deque<T, Alloc>& x, const deque<T, Alloc>& y)
{
    return !(y < x);
}

template <class T, class Alloc>
inline bool operator>=(const deque<T, Alloc>& x, const deque<T, Alloc>& y)
{
    return !(x < y);
}

template <class T, class Alloc>
inline void swap(deque<T, Alloc>& x, deque<T, Alloc>& y)
{
    x.swap(y);
}
}