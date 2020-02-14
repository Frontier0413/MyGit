#pragma once
#include "../Contant/stl_deque.h"
namespace hxl
{
template <class T, class Sequence = deque<T> >
class queue
{
    friend bool operator ==(const queue&, const queue&);

    friend bool operator <(const queue&, const queue&);

public:
    using value_type    = Sequence::value_type;
    using size_type     = Sequence::size_type;
    using reference     = Sequence::reference;
    using const_reference = Sequence::const_reference;

protected:
    Sequence c;

public:
    bool empty() const 
    {
        return c.empty();
    }

    size_type size() const 
    {
        return c.size();
    }

    reference front()
    {
        return c.front();
    }

    const_reference front() const 
    {
        return c.front();
    }

    reference back()
    {
        return c.back();
    }

    const_reference back() const 
    {
        return c.back();
    }

    void push(const value_type& x)
    {
        c.push_back(x);
    }

    void pop()
    {
        c.pop_front();
    }
};

template <class T, class Sequence>
bool operator== (const queue<T, Sequence>& x, const queue<T, Sequence>& y)
{
    return x.c == y.c;
}

template <class T, class Sequence>
bool operator< (const queue<T, Sequence>& x, const queue<T, Sequence>& y)
{
    return x.c < y.c;
}
}
