#pragma once
#include "../Iterator/stl_iterator.h"
namespace hxl
{

template <class ForwardIter, class T, class Distance>
ForwardIter __lower_bound(ForwardIter first, ForwardIter last, const T& val, Distance*)
{
    Distance len = distance(first, last);
    Distance half;
    ForwardIter middle;

    while (len > 0)
    {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle < val)
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else 
            len = half;
    }

    return first;
}

template <class ForwardIter, class T>
inline ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& val)
{
    return __lower_bound(first, last, val, distance_type(first));
}

template <class InputIterator, class Function>
void for_each(InputIterator first, InputIterator last, Function f)
{
    while (first != last)
    {
        f(first);
        ++first;
    }
}

template <class InputIterator, class T>
int count(InputIterator first, InputIterator last, const T& x)
{
    int result = 0;
    while (first != last)
    {
        if (*first == x)
            ++result;
        ++first;
    }
    return result;
}

template <class InputIterator, class Function>
int count_if(InputIterator first, InputIterator last, Function f)
{
    int result = 0;
    while (first != last)
    {
        if (f(*first))
            ++result;
        ++first;
    }
    return result;
}

//随机洗牌算法，将区间内元素随机打乱
template <class RandomAccessIter>
void random_shuffle(RandomAccessIter first, RandomAccessIter last)
{
    __random_shuffle(first, last, distance_type(first));
}

template <class RandomAccessIter, class Distance>
void __random_shuffle(RandomAccessIter first, RandomAccessIter last, Distance*)
{
    if (first == last)  return ;
    for (auto i = first + 1; i < last; ++first)
    {
        iter_swap(i, first + Distance(rand() % (i - first + 1)));
    }
}

}