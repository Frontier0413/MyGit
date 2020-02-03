#pragma once
#include <string.h>
#include <iostream>
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
//对于输入迭代器，由于只能遍历一次，因此只能用first != last判断循环条件,速度较慢
template <class InputIterator, class OutputIterator>
inline OutputIterator __copy(InputIterator first, InputIterator  last, OutputIterator result, input_iterator_tag)
{
	for (; first != last; ++first, ++result)
	{
		*result = *first;
	}
	return result;
}

//对于随机访问迭代器，可以先求出距离，再通过距离循环，速度较快，
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
//对于pod类型，直接调用memmove函数拷贝内存
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

//copy_dispatch，copy的转调用函数
template <class InputIterator, class OutputIterator>
struct __copy_dispatch
{
	OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
	{
		return __copy(first, last, result, iterator_category(first));
	}
};

//对于原生指针的copy_dispatch特化版本
//对于指针类型，其迭代器类型一定是随机访问迭代器，因此我们只关注指针所指类型是否是pod类型
template <class T>
struct __copy_dispatch<T*, T*>
{
	T* operator()(T* first, T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};

//对于原生指针的copy_dispatch特化版本
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
//即result++ = first++。返回result + (last - first)位置的迭代器
template <class InputIterator, class OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
	return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}





template <class RandomAccessIterator, class BidirectionalIterator, class Distance>
inline BidirectionalIterator __copy_backward_d(RandomAccessIterator first, RandomAccessIterator last, BidirectionalIterator result, Distance*)
{
	for (Distance n = last - first; n > 0; --n)
	{
		*--result = *--last;
	}
	return result;
}

template <class T>
inline T* __copy_backward_t(const T* first, const T* last, T* result, __true_type)
{
	memmove(result - (last - first), first, sizeof(T) * (last - first));
	return result - (last - first);
}

template <class T>
inline T* __copy_backward_t(const T* first, const T* last, T* result, __false_type)
{
	return __copy_backward_d(first, last, result, (ptrdiff_t*)0);
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 __copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, bidirectional_iterator_tag)
{
	
	for (; last != first;)
	{
		*--result = *--last;
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
		//std::cout << "dispatching..." << std::endl;
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

//copy_backward函数，将区间[first, last)标识的区间从后向前拷贝到result开始的区间内
//即result-- = last--。返回result - (last - first)
template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator1 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result)
{
	return __copy_backward_dispatch<BidirectionalIterator1, BidirectionalIterator2>()(first, last, result);
}
               




//按字典序比较，如果第一序列某元素小于第二序列，返回true。如果比较操作到达第一序列结尾，第二序列未结束，返回true。其余返回false。
template <class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
{
	for(; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if( *first1 < *first2 )
			return true;
		if( *first2 < *first1 )
			return false;
	}
	return first1 == last1 && first2 != last2;
}

//按字典序比较，如果第一序列某元素小于第二序列，返回true。如果比较操作到达第一序列结尾，第二序列未结束，返回true。其余返回false。
template <class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare comp)
{
	for(; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if( comp(*first1, *first2) )
			return true;
		if( comp(*first2, *first1) )
			return false;
	}
	return first1 == last1 && first2 != last2;
}

inline bool lexicographical_compare(const unsigned char* first1, const unsigned char* last1, const unsigned char* first2, const unsigned char* last2)
{
	const size_t len1 = last1 - first1;
	const size_t len2 = last2 - first2;
	const int result = memcmp(first1, first2, len1 < len2 ? len1 : len2);
	//如果result=0，说明两个序列不想上下，则比较长度，len1<len2返回true。
	//否则，如果result<0，说明第一序列小于第二序列，返回true。
	return result != 0 ? result < 0 : len1 < len2;
}

//返回较大元素的引用
template <class T>
inline const T& max(const T& a, const T& b)
{
	return a < b ? b : a;
}

template <class T, class Compare>
inline const T& max(const T& a, const T& b, Compare comp)
{
	return comp(a, b) ? b : a;
}

//返回较小元素的引用
template <class T>
inline const T& min(const T& a, const T& b)
{
	return b < a ? b : a;
}

template <class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp)
{
	return comp(b, a) ? b : a;
}

// //平行比较两个序列，返回两个序列的第一个不匹配点
// template <class InputIterator1, class InputIterator2>
// inline pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator2 first2)
// {
// 	while(first1 != last1 && *first1 == *first2)
// 	{
// 		++first1;
// 		++first2;
// 	}
// 	return pair<InputIterator1, InputIterator2>(first1, first2); 
// }

// //平行比较两个序列，返回两个序列的第一个不匹配点
// template <class InputIterator1, class InputIterator2, class BinaryPredicate>
// inline pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator2 first2, BinaryPredicate binary_pred)
// {
// 	while(first1 != last1 && binary_pred(*first1, *first2))
// 	{
// 		++first1;
// 		++first2;
// 	}
// 	return pair<InputIterator1, InputIterator2>(first1, first2); 
// }
