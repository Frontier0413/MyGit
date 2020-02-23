#pragma once
#include "../Contant/stl_deque.h"
namespace hxl
{
template <class T, class Sequence = deque<T> >
class stack
{
    friend bool operator ==(const stack&, const stack&);

    friend bool operator <(const stack&, const stack&);

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

    reference top()
    {
        return c.back();
    }

    const_reference top() const 
    {
        return c.back();
    }

    void pop()
    {
        c.pop_back();
    }

    void push(const value_type& x)
    {
        c.push_back(x);
    }
};

template <class T, class Sequence>
bool operator== (const stack<T, Sequence>& x, const stack<T, Sequence>& y)
{
    return x.c == y.c;
}

template <class T, class Sequence>
bool operator< (const stack<T, Sequence>& x, const stack<T, Sequence>& y)
{
    return x.c < y.c;
}
}
