#pragma once
#include "../Contant/stl_heap.h"
#include "../Contant/stl_vector.h"
namespace hxl
{
template <class T, class Sequence = vector<T>, class Compare = less<typename Sequence::value_type> >
class priority_queue
{
public:
    using value_type    = typename Sequence::value_type;
    using size_type     = typename Sequence::size_type;
    using reference     = typename Sequence::reference;
    using const_reference = typename Sequence::const_reference;

protected:
    Sequence c;
    Compare comp;

public:
    priority_queue() : c() { }

    explicit priority_queue(const Compare& x) : c(), comp(x) { }

    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x) : c(first, last), comp(x)
    {
        make_heap(c.begin(), c.end(), comp);
    }

    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last) : c(first, last)
    {
        make_heap(c.begin(), c.end(), comp);
    }

    bool empty() const
    {
        return c.empty();
    }

    size_type size() const
    {
        return c.size();
    }

    const_reference top() const 
    {
        return c.front();
    }

    void push(const value_type& value)
    {
        try 
        {
            c.push_back(x);
            push_heap(c.begin(), c.end(), comp);
        }catch(...)
        {
            c.clear();
        }
    }

    void pop()
    {
        try 
        {
            pop_heap(c.begin(), c.end(), comp);
            c.pop_back();
        }catch(...)
        {
            c.clear();
        }
    }
};
}