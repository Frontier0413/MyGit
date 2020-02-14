#pragma once
#include "../Iterator/stl_iterator.h"
#include "../Algorithm/stl_function.h"

namespace hxl
{
//将first,last区间元素放入堆中，其中last-1为新加入的元素，其余元素为已构建好的堆
template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    __push_heap_dispatch(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __push_heap_dispatch(RandomAccessIterator first, RandomAccessIterator last, T* tptr)
{
    less<T> comp;
    __push_heap_aux(first, last, distance_type(first), tptr, comp);
}

template <class RandomAccessIterator, class Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    __push_heap_aux(first, last, distance_type(first), value_type(first), comp);
}

template <class RandomAccessIterator, class Distance, class T, class Compare>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
{
    __push_heap(first, Distance( (last - first) - 1), Distance(0), T(*(last - 1)), comp);
}

template <class RandomAccessIterator, class Distance, class T, class Compare>
void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, Compare comp)
{
    Distance parent = (holeIndex - 1) / 2;
    //如果新加入的节点值大于其父亲节点的值，交换两节点的值
    while(holeIndex > topIndex && comp(*(first + parent), value))
    {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}



template <class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    __pop_heap_dispatch(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __pop_heap_dispatch(RandomAccessIterator first, RandomAccessIterator last, T* tptr)
{
    less<T> comp;
    __pop_heap_aux(first, last, tptr, comp);
}

template <class RandomAccessIterator, class Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    __pop_heap_aux(first, last, value_type(first), comp);
}

template <class RandomAccessIterator, class T, class Compare>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*, Compare comp)
{
    __pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first), comp);
}

template <class RandomAccessIterator, class T, class Distance, class Compare>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance*, Compare comp)
{
    //此时的last已经是调用pop_heap函数时的区间减去1得到的位置，即该堆的最后一个元素。
    *result = *first;
    //在调用adjust_heap时，last-frist表示的长度其实相当于尾后位置，这个位置已经赋值用来保存堆的根节点，
    __adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}

template <class RandomAccessIterator, class Distance, class T, class Compare>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, Compare comp)
{
    Distance topInedx = holeIndex;
    //secondChild为holeIndex的右子节点
    Distance secondChild = 2 * holeIndex + 2;
    while(secondChild < len)
    {
        //如果右子节点的值小于左子节点的值
        if (comp(*(first + secondChild), *(first + secondChild - 1)))
        {
            //则选取左子节点
            secondChild--;
        }
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = 2 * secondChild + 2;
    }
    //此时的len相当于前闭后开区间的last，即尾后位置，当右子节点为尾后位置时，说明左子节点仍在范围中
    if (secondChild == len)
    {
        *(first + holeIndex) = *(first + (secondChild - 1));
        holeIndex = secondChild - 1;
    }
    __push_heap(first, holeIndex, topInedx, value, comp);
}

template <class RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    while (last - first > 1)
        pop_heap(first, last--);
}

template <class RandomAccessIterator, class Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    while(last - first > 1)
        pop_heap(first, last--, comp);
}

template <class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    __make_heap_dispatch(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __make_heap_dpsiatch(RandomAccessIterator first, RandomAccessIterator last, T* tptr)
{
    less<T> comp;
    __make_heap(first, last, tptr, distance_type(first), comp);
}

template <class RandomAccessIterator, class Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    __make_heap(first, last, value_type(first), distance_type(first), comp);
}

template <class RandomAccessIterator,class T, class Distance, class Compare>
void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*, Compare comp)
{
    if (last - first < 2) return;
    Distance len = last - first;
    Distance parent = (len - 2) / 2;
    while(true)
    {
        __adjust_heap(first, parent, len, T(*(first + parent)), comp);
        if(parent == 0) return;
        --parent;
    }
}
}