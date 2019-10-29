//equal�������жϲ���[first1,last1)��first2��ʼ�������Ƿ����
#include "../Iterator/stl_iterator.h"
#include "../TypeTraits/type_traits.h"
#include <string.h>
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

//fill��������[first,last)����������Ԫ�ظ�����ֵ
template <class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& value)
{
	for (; first != last; ++first)
	{
		*first = value;
	}
}

template <class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value)
{
	for (; n > 0; --n, ++first)
	{
		*first = value;
	}
	return first;
}

//iter_swap����
template <class ForwardIterator1, class ForwardIterator2, class T>
inline void __iter_swap(ForwardIterator1 a, ForwardIterator2 b, T*)
{
	T tmp = *a;
	*a = *b;
	*b = tmp;
}

template <class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
{
	__iter_swap(a, b, value_type(a));
}

//swap����
template <class T>
inline void swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

 
//copyϵ��
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

//__copy_d����
template <class RandomAccessIterator, class Outputiterator, class Distance>
inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
{
	for (Distance n = lst - first; n > 0; --n, ++result, ++first) 
	{
		*result = *first;
	}
	return result;
}

//__copy_t����
template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, __true_type)
{
	memmove(result, first, sizeof(T) * (last - first));
	return result + last - first;
}

template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, __false_type)
{
	//ԭ��ָ����һ��������ʵ�����
	//����false_type���޷�ֱ�ӵ���memmove��������˽���__copy_d����ִ�С�
	return __copy_d(first, last, result, (ptrdiff_t*)0);
}

//__copy_dispatch���������汾
template <class InputIterator, class OutputIterator>
struct __copy_dispatch
{
	OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
	{
		return __copy(first, last, result, iterator_category(first));
	}
};

//__copy_dispatch������ԭ��ָ��ƫ�ػ��汾
template <class T>
struct __copy_dispatch<T*, T*>
{
	T* operator()(T* first, T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};

//__copy_dispatch�����ĳ�ԭ��ָ��ƫ�ػ��汾
template <class T>
struct __copy_dispatch<const T*, T*>
{
	T* operator()(const T* first, const T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};

//copy����������ӿ�
template <class InputIterator, class OutputIterator>
inline OutputIterator  copy(InputIterator first, InputIterator last, OutputIterator last)
{
	return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}

//copy������char*ƫ�ػ��汾
inline char* copy(const char* first, const char* last, char* result)
{
	memmove(result, first, last - first);
	return result + last - first;
}

//copy������const char*ƫ�ػ��汾
inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + last - first;
}


//copy_backward����
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

template <class RandomAccessIterator1, class BidirectionalIterator2>
inline RandomAccessIterator2 __copy_backward(RandomAccessIterator1 first, RandomAccessIterator1 last, BidirectionalIterator2 result, random_access_iterator_tag)
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
inline BidirectionalIterator1 copy_backward(BidirectionalIterator1 first1, BidirectionalIterator1 last1, BidirectionalIterator2 result)
{
	return __copy_backward_dispatch<BidirectionalIterator1, BidirectionalIterator2>(first, last, result);
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
               