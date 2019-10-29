#include <new>
#include "../TypeTraits/type_traits.h"


//the basic construct funciton
template <class T1, class T2>
inline void construct(T1* p, const T2& value)
{
    new(p) T1(value);
}

//basic destory function
//only accept one pointer as a parameter
template <class T>
inline void destory(T* pointer)
{
    pointer->~T();
}

//basic destory function
//accept two iterators as parameters
template <class ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last)
{
    __destory(first, last, value_type(first));
}

//check the value type
//if the type has a trivial destructor
//choose the right version
template <class ForwardIterator, class T>
inline void __destory(ForwardIterator first, ForwardIterator last, T*)
{
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destory_aux(first, last, trivial_destructor());
}

template <class ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
    for( ; first < last; ++first)
    {
        destory(&*first);
    }
}

template <class ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __true_type)
{
   
}

//if the type is char or w_char
//the funcitona don`t have to anything
inline void destory(char*, char*) { }
inline void destory(wchar_t*, wchar_t*) { }