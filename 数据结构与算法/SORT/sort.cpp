#include "../../项目/MySTL/Algorithm/stl_algobase.h"
#include "../../项目/MySTL/Algorithm/stl_algo.h"
#include "../../项目/MySTL/myhead.h"
#include <string.h>
namespace hxl
{

//插入排序，时间复杂度为n^2。但是在数据较小时有较好的性能,稳定
//核心思想为依次将元素插入到已经排序好的区间内，初始时区间就为first一个元素
//优化是，如果新加入的元素小于first，则直接将整个区间往后移一，如果不小于first，则从区间末尾开始向前寻找该元素插入的位置，这样可以不用检查是否越界。
template <class RandomAccessIter>
void insertion_sort(RandomAccessIter first, RandomAccessIter last)
{
    if (first == last) return ;
    for (auto i = first + 1; i != last; ++i)
    {
        __linear_insert(first, i, value_type(first));
    }
}

template <class RandomAccessIter, class T>
void __linear_insert(RandomAccessIter first, RandomAccessIter last, T*)
{
    T value = *last;
    if (value < *first)
    {
        copy_backward(first, last, last + 1);
        *first = value;
    }
    else 
        __unguarded_linear_insert(last, value);
}

//插入排序中的一个步骤， 将value插入到以last为结尾的区间内
template <class RandomAccessIter, class T>
void __unguarded_linear_insert(RandomAccessIter last, T value)
{
    RandomAccessIter next = last;
    --next;
    //找到value > * next的第一个位置，该位置即为value的插入位置
    //当next指向first时，while循环必停止
    while (value < *next)
    {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}


//冒泡排序，时间复杂度为n^2，稳定
//核心思想是每次找出最大值放在区间的最后面
//外层循环中i可理解为已找出的最大值数目。内层循环每一次都从开头寻找，依次比较两个相邻元素，将最大值移到区间末尾
template <class RandomAccessIter>
void bubble_sort(RandomAccessIter first, RandomAccessIter last)
{
    __bubble_sort(first, last, distance_type(first));
}

template <class RandomAccessIter, class Distance>
void __bubble_sort(RandomAccessIter first, RandomAccessIter last, Distance*)
{
    Distance len = distance(first, last);
    bool orderly = false;
    for (Distance i = 0; !orderly && i < len; ++i)
    {
        orderly = true;
        for (Distance j = 0; j < len - i - 1; ++j)
        {
            if(*(first + j) > *(first + j + 1))
            {
                orderly = false;
                iter_swap(first + j, first + j + 1);
            }
        }
        //若程序执行到这里，orderly仍然为true，则说明此次内循环未发生任何交换，即此时已经是有序的了
    }
}


template <class RandomAccessIter>
void selection_sort(RandomAccessIter first, RandomAccessIter last)
{
    __selection_sort(first, last, distance_type(first));
}

//选择排序，时间复杂度n^2，不稳定
//核心思想为每一次找出第一个元素与之后的元素比较依次选取最小值。
template <class RandomAccessIter, class Distance>
void __selection_sort(RandomAccessIter first, RandomAccessIter last, Distance*)
{
    Distance len = distance(first, last);
    for (Distance i = 0; i < len; ++i)
    {
        Distance min = i;
        for (Distance j = i + 1; j < len; ++j)
        {
            if (*(first + j) < *(first + min))
                min = j;
        }
        iter_swap(first + i, first + min);
    }
}


//快速排序，时间复杂度为nlgn，不稳定
//核心思想是每次选取一个基准值，根据大小关系将区间分为两部分，递归对这两部分进行快速排序
template <class RandomAccessIter>
void quick_sort(RandomAccessIter first, RandomAccessIter last)
{
    __quick_sort(first, last, *first);
}

template <class RandomAccessIter, class T>
void __quick_sort(RandomAccessIter first, RandomAccessIter last, T value)
{
    if (first >= last) return ;
    RandomAccessIter left = first + 1, right = last;
    while (left <= right)
    {
        while (left <= last && *left <= value) ++left;
        while (right > first && *right > value) --right;
        if (left < right)
            iter_swap(left, right);
    }
    iter_swap(first, right);
    __quick_sort(first, left - 1, *first);
    __quick_sort(left, last, *left);
}


template <class RandomAccessIter>
void heap_sort(RandomAccessIter first, RandomAccessIter last)
{
    if (last - first < 2) return;
    RandomAccessIter next = first + 2;
    while(next != last)
    {
        push_heap(first, next);
        ++next;
    }
    while(last - first > 1)
    {
        pop_heap(first, last);
        --last;
    }
}

template <class RandomAccessIter>
void push_heap(RandomAccessIter first, RandomAccessIter last)
{
    __push_heap_aux(first, last, distance_type(first), value_type(first));
}

template <class RandomAccessIter, class Distance, class T>
void __push_heap_aux(RandomAccessIter first, RandomAccessIter last, Distance*, T*)
{
    __push_heap(first, Distance(last - first - 1), Distance(0), *(last - 1));
}

template <class RandomAccessIter, class Distance, class T>
void __push_heap(RandomAccessIter first, Distance holeIndex, Distance topIndex, T value)
{
    Distance parentIndex = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && value > *(first + parentIndex))
    {
        *(first + holeIndex) = *(first + parentIndex);
        holeIndex = parentIndex;
        parentIndex = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template <class RandomAccessIter>
void pop_heap(RandomAccessIter first, RandomAccessIter last)
{
    __pop_heap_aux(first, --last, distance_type(first), value_type(first));
}

template <class RandomAccessIter, class Distance, class T>
void __pop_heap_aux(RandomAccessIter first, RandomAccessIter last, Distance*, T*)
{
    T value = *last;
    *last = *first;
    __pop_heap(first, Distance(0), Distance((last - first - 1)), value);
}

template <class RandomAccessIter, class Distance, class T>
void __pop_heap(RandomAccessIter first, Distance holeIndex, Distance lastIndex, T value)
{
    Distance rightChild = holeIndex * 2 + 2;
    while (rightChild <= lastIndex)
    {
        if (*(first + rightChild) < *(first + rightChild - 1))
        {
            --rightChild;
        }
        *(first + holeIndex) = *(first + rightChild);
        holeIndex = rightChild;
        rightChild = holeIndex * 2 + 2;
    }
    if (rightChild - 1 == lastIndex)
    {
        *(first + holeIndex) = *(first + rightChild - 1);
        holeIndex = rightChild - 1;
    }
    __push_heap(first, holeIndex, Distance(0), value);
}

} // end of namespace

using namespace hxl;

int main()
{
    int a[10] = {7, 0, 3, 2, 5, 8, 4, 1, 9, 6};
    // int a[2] = {0, 1};
    heap_sort(a, a + 10);
    show_array(a);
}