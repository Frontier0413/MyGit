#pragma once
#include <cstddef>
//迭代器类型标签
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag, public output_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag{};

//迭代器基础类，自行设计迭代器时，最好继承下面这个类
template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
struct iterator
{
    typedef Category iterator_category;
    typedef T value_type;
    typedef Distance difference_type;
    typedef Pointer pointer;
    typedef Reference reference;
};

//提取迭代器内嵌型别
template <class Iterator>
struct iterator_traits
{
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::difference_type   difference_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
};

//针对原生指针的iterator_traits特化版本
template <class T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef ptrdiff_t                  difference_type;
    typedef T*                         pinter;
    typedef T&                         reference;
};

//针对顶层const指针的iterator_traits特化版本
template <class T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef ptrdiff_t                  difference_type;
    typedef const T*                   pinter;
    typedef const T&                   reference;
};

//获取迭代器的迭代器类型
template <class Iterator> 
inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator& )
{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    //返回该迭代器类型标签的空对象
    return category();
}

//获取迭代器的distance_type
template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator& )
{
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}
//返回指针类型是为了在判断类型时，可以使用指针，这样避免了拷贝，因为这个值往往是不会用到的，所以
//只要能赋值成功就能代表类型，同时也可以避免隐式的类型转换带来的歧义。

//获取迭代器的value_type
template <class Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator& )
{
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}



template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type 
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while(first != last)
    {
        first++;
        n++;
    }
    return n;
}

template <class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type 
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    return last - first;
}

//获取迭代器first与last之间的距离
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    __distance(first, last, category());
}
//对于随机访问迭代器，获取距离的最快方式就是直接相减，而对于不能随机访问的迭代器，只能通过循环来获得距离


template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag)
{
    while(n--)
        ++i;
}

template<class BidirectionalIterator, class Distance>
inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
{
    if(n >= 0)
    {
        while(n--)  ++i;
    }
	else
	{
		while (n++)  --i;
	}
}

template<class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag)
{
    i += n;
}

//将迭代器i向前移动n个距离
template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n)
{
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    __advance(i, n, category());
}
//同样的，对于随机访问迭代器，前进n距离只需要+n即可，对于其他类型，则需要通过循环。


template <class Iterator>
class reverse_iterator
{
protected:
    Iterator current;

public:
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type        = typename iterator_traits<Iterator>::value_type;
    using difference_type   = typename iterator_traits<Iterator>::difference_type;
    using pointer           = typename iterator_traits<Iterator>::pointer;
    using reference         = typename iterator_traits<Iterator>::reference;

    using iterator_type     = Iterator;
    using Self              = reverse_iterator<Iterator>;

public:
    reverse_iterator() { }

    explicit reverse_iterator(iterator_type x) : current(x) {}

    reverse_iterator(const Self& x) : current(x.current) {}

    template<class Iter>
    reverse_iterator(const reverse_iterator<Iter>& x) : current(x.base()) {}

    iterator_type base() const { return current; }

    reference operator*() const 
    {
        Iterator tmp = current;
        return *--tmp;
    }

    pointer operator->() const 
    {
        return &(operator*());
    }

    Self& operator++()
    {
        --current;
        return *this;
    }

    Self operator++(int)
    {
        Self tmp = *this;
        --current;
        return tmp;
    }

    Self& operator--()
    {
        ++current;
        return *this;
    }

    Self operator--(int)
    {
        Self tmp = *this;
        ++current;
        return tmp;
    }

    Self operator+(difference_type n) const 
    {
        return Self(current - n);
    }

    Self& operator+=(difference_type n)
    {
        current -= n;
        return *this;
    }

    Self operator-(difference_type n) const 
    {
        return Self(current + n);
    }

    Self& operator-=(difference_type n)
    {
        current += n;
        return *this;
    }

    reference operator[](difference_type n) const 
    {
        return *(*this + n);
    }
};

template <class Iterator>
inline bool operator==(const reverse_iterator<Iterator>& x, 
                       const reverse_iterator<Iterator>& y) {
  return x.base() == y.base();
}

template <class Iterator>
inline bool operator<(const reverse_iterator<Iterator>& x, 
                      const reverse_iterator<Iterator>& y) {
  return y.base() < x.base();
}

template <class Iterator>
inline bool operator!=(const reverse_iterator<Iterator>& x,
                       const reverse_iterator<Iterator>& y)
{
    return !(x == y)
}

template <class Iterator>
inline bool operator>(const reverse_iterator<Iterator>& x, 
                      const reverse_iterator<Iterator>& y) {
  return y < x;
}

template <class Iterator>
inline bool operator<=(const reverse_iterator<Iterator>& x, 
                       const reverse_iterator<Iterator>& y) {
  return !(y < x);
}

template <class Iterator>
inline bool operator>=(const reverse_iterator<Iterator>& x, 
                      const reverse_iterator<Iterator>& y) {
  return !(x < y);
}

template <class Iterator>
inline typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& x, 
          const reverse_iterator<Iterator>& y) {
  return y.base() - x.base();
}