#include "stl_construct.h"
#include "../Algorithm/stl_algobase.h"
#include <string.h>
//fill_n函数
//在[first,first + n)范围内，产生x的拷贝。
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

template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x)
{
    return __uninitialized_fill_n(first, n, x, value_type(x));
}

//copy函数
//copy函数将[first，last）前闭后开区间范围内的每一个对象，拷贝到result开始的未初始化的区域
//对于pod类型，调用高阶函数copy
//对于no pod类型，直接调用stl_construct中的construct函数。
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
    return __uninitialized_copy_aux(first, last, result, is_pod());

}

template <class InputIterator,class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{
    return __uninitialized_copy(first, last, result, value_type(first));
}

//fill函数
//在[first, last)范围内产生x的拷贝
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

template <class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
{
    __uninitialized_fill(first, last, x, value_type(first));
}