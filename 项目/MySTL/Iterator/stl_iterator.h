#include <cstddef>
//迭代器类型标签
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag {};
struct bidirectional_iterator_tag {};
struct random_access_iterator_tag {};

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

//这个函数可以得到某个迭代器的category
template <class Iterator> 
inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator& )
{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    //返回该迭代器类型标签的空对象
    return category();
}

//获取迭代器的distance_type
//为什么返回指针类型?
template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator& )
{
    return static<typename iterator_traits<Iterator>::difference_type*>(0);
}

//获取迭代器的value_type
template <class Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator& )
{
    return static<typename iterator_traits<Iterator>::value_type*>(0);
}

//distance函数，返回两个迭代器的距离
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type 
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    iterator_traits<InputIterator>::difference_type n = 0;
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

template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    __distance(first, last, category());
}

//advance函数,将给定迭代器i移动给定距离n
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

template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n)
{
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    __advance(i, n, category());
}
