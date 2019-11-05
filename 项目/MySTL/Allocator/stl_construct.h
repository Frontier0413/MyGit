#pragma once
#include <new>
#include "../Iterator/stl_iterator.h"
#include "../TypeTraits/type_traits.h"


//构造函数，在p指针指向的内存构造value的副本
template <class T1, class T2>
inline void construct(T1* p, const T2& value)
{
    new(p) T1(value);
}

//对于析构函数不是无关紧要的型别来说，必须显示调用析构函数来释放内存
template <class ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
    for( ; first < last; ++first)
    {
        destory(&*first);
    }
}

//对于析构函数是无关紧要的型别，则什么也不做
template <class ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __true_type)
{
   
}

//通过type_traits获取类型型别
template <class ForwardIterator, class T>
inline void __destory(ForwardIterator first, ForwardIterator last, T*)
{
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destory_aux(first, last, trivial_destructor());
}


//对于原生指针，destory直接通过指针调用析构函数
template <class T>
inline void destory(T* pointer)
{
    pointer->~T();
}

//destory函数，释放区间[first,last)内的元素
template <class ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last)
{
    __destory(first, last, value_type(first));
}
//destory函数通过value_type获取迭代器所指型别，
//再调用type_traits判断该型别是否有无关紧要的析构函数
//如果不是，则调用对应的析构函数
//如果析构函数是无关紧要的，则什么也不做

//char和wchar_t的特化版本
inline void destory(char*, char*) { }
inline void destory(wchar_t*, wchar_t*) { }