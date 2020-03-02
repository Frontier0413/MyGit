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
    if (first == last)
        return;
    for (auto i = first + 1; i != last; ++i)
    {
        __linear_insert(first, i, value_type(first));
    }
}

template <class RandomAccessIter, class T>
static void __linear_insert(RandomAccessIter first, RandomAccessIter last, T *)
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
static void __unguarded_linear_insert(RandomAccessIter last, T value)
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
static void __bubble_sort(RandomAccessIter first, RandomAccessIter last, Distance *)
{
    Distance len = distance(first, last);
    bool orderly = false;
    for (Distance i = 0; !orderly && i < len; ++i)
    {
        orderly = true;
        for (Distance j = 0; j < len - i - 1; ++j)
        {
            if (*(first + j) > *(first + j + 1))
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
static void __selection_sort(RandomAccessIter first, RandomAccessIter last, Distance *)
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
static void __quick_sort(RandomAccessIter first, RandomAccessIter last, T value)
{
    if (first >= last)
        return;
    if (last - first == 1)
    {
        if (*last < *first)
            iter_swap(first, last);
        return;
    }
    RandomAccessIter left = first + 1, right = last;
    while (left <= right)
    {
        while (left <= last && *left <= value)
            ++left;
        while (right > first && *right > value)
            --right;
        if (left < right)
            iter_swap(left, right);
    }
    iter_swap(first, right);
    __quick_sort(first, left - 1, *first);
    __quick_sort(left, last, *left);
}

//堆排序，时间复杂度nlgn，不稳定
//核心思想是构建最大堆，然后依次从堆顶取出元素放在末尾
template <class RandomAccessIter>
void heap_sort(RandomAccessIter first, RandomAccessIter last)
{
    if (last - first < 2)
        return;
    RandomAccessIter next = first + 2;
    while (next != last)
    {
        push_heap(first, next);
        ++next;
    }
    while (last - first > 1)
    {
        pop_heap(first, last);
        --last;
    }
}

template <class RandomAccessIter>
static void push_heap(RandomAccessIter first, RandomAccessIter last)
{
    __push_heap_aux(first, last, distance_type(first), value_type(first));
}

template <class RandomAccessIter, class Distance, class T>
static void __push_heap_aux(RandomAccessIter first, RandomAccessIter last, Distance *, T *)
{
    __push_heap(first, Distance(last - first - 1), Distance(0), *(last - 1));
}

template <class RandomAccessIter, class Distance, class T>
static void __push_heap(RandomAccessIter first, Distance holeIndex, Distance topIndex, T value)
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
static void pop_heap(RandomAccessIter first, RandomAccessIter last)
{
    __pop_heap_aux(first, --last, distance_type(first), value_type(first));
}

template <class RandomAccessIter, class Distance, class T>
static void __pop_heap_aux(RandomAccessIter first, RandomAccessIter last, Distance *, T *)
{
    T value = *last;
    *last = *first;
    __pop_heap(first, Distance(0), Distance((last - first - 1)), value);
}

template <class RandomAccessIter, class Distance, class T>
static void __pop_heap(RandomAccessIter first, Distance holeIndex, Distance lastIndex, T value)
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

//归并排序，时间复杂度nlgn,稳定
//核心思想是每次将区间分成两个大小相同的区间，分别对其进行归并排序，然后将这两个区间按序合并。
template <class RandomAccessIter>
void merge_sort(RandomAccessIter first, RandomAccessIter last)
{
    __merge_sort(first, --last);
}

template <class RandomAccessIter>
static void __merge_sort(RandomAccessIter first, RandomAccessIter last)
{
    if (last <= first)
        return;
    RandomAccessIter mid = first + (last - first) / 2;
    __merge_sort(first, mid);
    __merge_sort(mid + 1, last);
    merge(first, mid, mid + 1, last, value_type(first));
}

template <class RandomAccessIter, class T>
static void merge(RandomAccessIter first1, RandomAccessIter last1, RandomAccessIter first2, RandomAccessIter last2, T *)
{
    T *tmp = new T[last2 - first1 + 1];
    T *p = tmp, *del = tmp;
    RandomAccessIter first = first1, last = last2;
    while (first1 <= last1 && first2 <= last2)
    {
        if (*first1 < *first2)
        {
            *tmp = *first1;
            ++tmp;
            ++first1;
        }
        else
        {
            *tmp = *first2;
            ++tmp;
            ++first2;
        }
    }
    while (first1 <= last1)
    {
        *tmp = *first1;
        ++first1;
        ++tmp;
    }
    while (first2 <= last2)
    {
        *tmp = *first2;
        ++first2;
        ++tmp;
    }

    while (first <= last)
    {
        *first = *p;
        ++first;
        ++p;
    }
    delete[] del;
}


//希尔排序，时间复杂度nlgn，不稳定
//核心思想与插入排序类似，是一种改进的插入排序，其每一次取一个间隔，按间隔将区间分组，分别对组进行插入排序，直到间隔为1为止
//这样做的目的是是区间内元素大致有序，因为插入排序在大致有序的区间内性能较好
template <class RandomAccessIter>
void shell_sort(RandomAccessIter first, RandomAccessIter last)
{
    __shell_sort(first, last, distance_type(first));
}

template <class RandomAccessIter, class Distance>
static void __shell_sort(RandomAccessIter first, RandomAccessIter last, Distance *)
{
    Distance len = last - first;
    for (Distance i = len / 2; i > 0; i /= 2)
    {
        for (Distance j = 0; j < i; ++j)
        {
            shell_insertion(first + j, last, i);
        }
    }
}

template <class RandomAccessIter, class Distance>
static void shell_insertion(RandomAccessIter first, RandomAccessIter last, Distance x)
{
    RandomAccessIter end = first;
    while (end + x < last)
        end += x;
    for (RandomAccessIter i = first + x; i <= end; i += x)
    {
        __shell_insertion(first, i, *i, x);
    }
}

template <class RandomAccessIter, class T, class Distance>
static void __shell_insertion(RandomAccessIter first, RandomAccessIter last, T value, Distance x)
{
    RandomAccessIter next = last - x;
    if (value < *first)
    {
        while (last > first)
        {
            *last = *next;
            last -= x;
            next -= x;
        }
        *last = value;
    }
    else
    {
        while (last > first)
        {
            if (*next < value)
            {
                *last = value;
                break;
            }
            else
            {
                *last = *next;
                last -= x;
                next -= x;
            }
        }
    }
}


//计数排序，时间复杂度为n + k,空间复杂度n + k;不存在稳定性的税法
//核心思想是记录每个元素出现的次数，用元素值作为下标，每次在数组对应位置+1，之后根据统计数组按序还原区间
//计数排序不是比较排序。只适用于区间内的元素都属于正整数且元素最大值不会太大的情况，但其速度非常快。
template <class RandomAccessIter>
void count_sort(RandomAccessIter first, RandomAccessIter last)
{
    RandomAccessIter old_first = first;
    RandomAccessIter max = first;
    while(first < last)
    {
        if(*first > *max)
            max = first;
        ++first;
    }
    __count_sort(old_first, last, *max + 1);
}

template <class RandomAccessIter, class T>
void __count_sort(RandomAccessIter first, RandomAccessIter last, T max_num)
{
    T* count_arry = new T[max_num];
    memset(count_arry, 0, sizeof(T) * max_num);
    for(RandomAccessIter i = first; i < last; ++i)
    {
        ++count_arry[*i];
    }
    for(T i = 0; i < max_num; ++i)
    {
        for(T j = 0; j < count_arry[i]; ++j, ++first)
            *first = i;
    }
    delete[] count_arry;
}


//桶排序，是快速排序的改进
//其核心思想是将元素放入按序排序好的桶中，分别对桶排序，最后将元素从桶中倒出来，
//快速排序本质上是将元素分为两个桶，桶排序是一种以空间换取时间的算法，且其具有较大的局限性。


//基数排序，桶排序的扩展
//其核心思想是不根据元素值域而是根据元素的位数来划分，如果区间内元素最大值为4396，则其为4位数
//将所有元素都转化为4位数的表现形式，然后只比较个位数排序，依次是十位...最高位。
} // namespace hxl

using namespace hxl;

int main()
{
    int a[10] = {7, 0, 3, 2, 5, 8, 4, 1, 9, 6};
    count_sort(a, a + 10);
    show_array(a);
}