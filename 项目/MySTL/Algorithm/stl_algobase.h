#pragma once
#include <string.h>
#include "../Iterator/stl_iterator.h"
#include "../TypeTraits/type_traits.h"
//equal函数，判断区间[first1, last1)与first2开始的区间是否相同
//使用者应该保证first2开始的区间长度不小于[first1, last1)区间长度，否则会引发内存访问错误
//当第一区间长度小于第二区间时，只比较第一区间长度的元素。
template <class InputIterator1, class InputIterator2>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
{
	for (; first1 != last1; ++first1, ++first2)
	{
		if (*first1 != *first2)
			return false;
	}
	return true;
}

//相比于上一个版本的equal函数，多出一个参数BinaryPredicate。使用该参数替代==运算符来判断元素是否相等
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate binary_pred)
{
	for (; first1 != last1; ++first1, ++first2)
	{
		if (!binary(*first1, *first2))
			return false;
	}
	return true;
}

//fill函数，接受三个参数。将[first, last)标识的区间内所有元素调用赋值操作符=赋予值value
template <class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& value)
{
	for (; first != last; ++first)
	{
		*first = value;
	}
}

//fill_n函数，由first开始的区间内n个元素赋予值value。
template <class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value)
{
	for (; n > 0; --n, ++first)
	{
		*first = value;
	}
	return first;
}

//iter_swap内部实现。
template <class ForwardIterator1, class ForwardIterator2, class T>
inline void __iter_swap(ForwardIterator1 a, ForwardIterator2 b, T*)
{
	T tmp = *a;
	*a = *b;
	*b = tmp;
}

//iter_swap函数。交换两个迭代器所指代的内容
template <class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
{
	__iter_swap(a, b, value_type(a));
}

//旧时的swap函数。引入右值引用之后改用新的版本。
template <class T>
inline void swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

 
//copy系列
template <class InputIterator, class OutputIterator>
inline OutputIterator __copy(InputIterator first, InputIterator  last, OutputIterator result, input_iterator_tag)
{
	for (; first != last; ++first, ++result)
	{
		*result = *first;
	}
	return result;
}

template <class RandomAccessIterator, class OutputIterator>
inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag)
{
	return __copy_d(first, last, result, distance_type(first));
}

template <class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
{
	for (Distance n = last - first; n > 0; --n, ++result, ++first) 
	{
		*result = *first;
	}
	return result;
}

template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, __true_type)
{
	memmove(result, first, sizeof(T) * (last - first));
	return result + (last - first);
}

template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, __false_type)
{
	return __copy_d(first, last, result, (ptrdiff_t*)0);
}

template <class InputIterator, class OutputIterator>
struct __copy_dispatch
{
	OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
	{
		return __copy(first, last, result, iterator_category(first));
	}
};

template <class T>
struct __copy_dispatch<T*, T*>
{
	T* operator()(T* first, T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};

template <class T>
struct __copy_dispatch<const T*, T*>
{
	T* operator()(const T* first, const T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};

//copy函数，将区间[first, last)标识的区间拷贝到result开始的区间内
//即result++ = first++。
template <class InputIterator, class OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
	return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}
//为实现高效率的拷贝，copy函数内部使用了不同的拷贝方式
//对于原生指针，copy_dispatch会交给__copy_t来执行，对于迭代器类型，copy_dispatch会交由__copy执行
//copy函数判断迭代器类型，对于输入迭代器，只能通过==判断拷贝循环条件(__copy)。
//对于随机访问迭代器，可以通过n = last - first，使用n来作为循环判断条件，效率更高(__copy_d)。
//__copy_t通过__type_traits判断型别是否是pod类型。如果是，直接采用底层函数memmove。
//如果不是，因为原生指针也是一种随机访问迭代器，可以转交给copy_d执行.
//无法使用memcpy，因为无法保证区间不重叠。
inline char* copy(const char* first, const char* last, char* result)
{
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}


//copy_backward函数，将区间[first, last)标识的区间拷贝到result开始的区间内
//与copy不同的是，从last开始向前依次拷贝，即result-- = last--。
template <class RandomAccessIterator, class BidirectionalIterator, class Distance>
inline BidirectionalIterator __copy_backward_d(RandomAccessIterator first, RandomAccessIterator last, BidirectionalIterator result, Distance*)
{
	for (Distance n = last - first; n > 0; --n, --last, --result)
	{
		*result = *last;
	}
	return result;
}

template <class T>
inline T* __copy_backward_t(const T* first, const T* last, T* result, __true_type)
{
	memmove(result, first, sizeof(T) * (last - first));
	return result - sizeof(T) * (last - first);
}

template <class T>
inline T* __copy_backward_t(const T* first, const T* last, T* result, __false_type)
{
	return __copy_backward_d(first, last, result, (ptrdiff_t*)0);
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 __copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, input_iterator_tag)
{
	for (; last != first; --last, --result)
	{
		*result = *last;
	}
	return result;
}

template <class RandomAccessIterator, class BidirectionalIterator>
inline BidirectionalIterator __copy_backward(RandomAccessIterator first, RandomAccessIterator last, BidirectionalIterator result, random_access_iterator_tag)
{
	return __copy_backward_d(first, last, result, distance_type(first));
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
struct __copy_backward_dispatch
{
	BidirectionalIterator2 operator()(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result)
	{
		return __copy_backward(first, last, result, iterator_category(first));
	}
};

template <class T>
struct __copy_backward_dispatch<T*, T*>
{
	T* operator()(T* first, T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_backward_t(first, last, result, t());
	}
};

template <class T>
struct __copy_backward_dispatch<const T*, T*>
{
	T* operator()(const T* first, const T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_backward_t(first, last, result, t());
	}
};

template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator1 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result)
{
	return __copy_backward_dispatch<BidirectionalIterator1, BidirectionalIterator2>()(first, last, result);
}

inline char* copy_backward(const char* first, const char* last, char* result)
{
	memmove(result, first, last - first);
	return result - (last - first);
}

inline wchar_t* copy_backward(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result - (last - first);
}
               