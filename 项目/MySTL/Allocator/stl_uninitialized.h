#pragma once
#include <string.h>
#include "../Allocator/stl_construct.h"
#include "../Algorithm/stl_algobase.h"

namespace hxl
{
//fill_n函数
//在first迭代器开始的区间内，初始化n个元素为元素x的拷贝
template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
{
    ForwardIterator cur = first;
	for (; n > 0; --n, ++cur)
	{
		construct(&*cur, x);
	}
	return cur;
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
{
    return fill_n(first, n, x);
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

//fill_n函数通过value_type获得迭代器的值类型，在通过type_traits判断元素类型是否为pod类型
//对于pod类型，直接调用高阶函数fill_n。对于非pod类型，遍历区间依次调用construct函数
template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x)
{
    return __uninitialized_fill_n(first, n, x, value_type(first));
}


inline char* uninitialized_copy(const char* first, const char* last, char* result)
{
    memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type)
{
    return copy(first, last, result);
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type)
{
    ForwardIterator cur = result;
    for( ; first != last; ++first, ++cur)
    {
        construct(&*cur, *first);
    }
    return cur;
}

template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*)
{
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());

}

//copy函数
//copy函数将[first，last）前闭后开区间范围内的每一个对象，拷贝到result开始的未初始化的区域
//对于pod类型，调用高阶函数copy
//对于no pod类型，遍历区间依次调用stl_construct中的construct函数。
template <class InputIterator,class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{
    return __uninitialized_copy(first, last, result, value_type(first));
}


template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type)
{
    fill(first, last, x);
}

template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type)
{
    ForwardIterator cur = first;
    for ( ; cur != last; ++cur)
    {
        construct(&*cur, x);
    }
}

template <class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    __uninitialized_fill_aux(first, last, x, is_POD());
}

//fill函数
//在[first, last)范围内产生x的拷贝
//对于pod类型，调用高阶函数fill
//对于非pod类型，遍历区间依次调用construct函数
template <class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
{
    __uninitialized_fill(first, last, x, value_type(first));
}

//先将[first1,last1)区间拷贝到result，再将[first2,last2)区间拷贝到后续位置,返回下一位置的迭代器
template <class InputIterator1, class InputIterator2, class ForwardIterator>
inline ForwardIterator uninitialized_copy_copy(InputIterator1 first1, InputIterator1 last1,
                                                InputIterator2 first2, InputIterator2 last2,
                                                ForwardIterator result)
{
    ForwardIterator mid = uninitialized_copy(first1, last1, result);
    try 
    {
        return uninitialized_copy(first2, last2, mid);
    }catch(...)
    {
        destory(result, mid);
    }
}

//先将区间[result,mid)初始化为x，再将[first, last)区间拷贝到后续位置，返回下一位置的迭代器
template <class ForwardIterator, class T, class InputIterator>
inline ForwardIterator uninitialized_fill_copy(ForwardIterator result, ForwardIterator mid, const T& x,
                                                InputIterator first, InputIterator last)
{
    uninitialized_fill(result, mid, x);
    try 
    {
        return uninitialized_copy(first, last, mid);
    }catch(...)
    {
        destory(result, mid);
    }
}

//先将[first1, last1)区间拷贝到first2开始的区间，然后后续位置到last2区间初始化为x，无返回值
template <class InputIterator, class ForwardIterator, class T>
inline void uninitialized_copy_fill(InputIterator first1, InputIterator last1,
                                    ForwardIterator first2, ForwardIterator last2,
                                    const T& x)
{
    ForwardIterator mid2 = uninitialized_copy(first1, last1, first2);
    try 
    {
        uninitialized_fill(mid2, last2, x);
    }catch(...)
    {
        destory(first2, mid2);
    }
}
}