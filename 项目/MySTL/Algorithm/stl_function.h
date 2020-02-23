#pragma once
namespace hxl
{

template <class Arg, class Result>
struct unary_function
{
    using result_type = Result;
    using argument_type = Arg;
};

template <class Arg1, class Arg2, class Result>
struct binary_function
{
    using result_type = Result;
    using first_argument_type = Arg1;
    using second_argument_type = Arg2;
};

template <class T>
struct less : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const 
    {
        return x < y;
    }
};

template <class Iter>
struct less_iter : public binary_function<Iter, Iter, bool>
{
    bool operator()(const Iter& x, const Iter& y)
    {
        return *x < *y;
    }
};

template <class key, class value>
struct identity : public unary_function<value, key>
{
    key operator()(const value& val)
    {
        return val;
    }
};

template <class key, class value>
struct select1st : public unary_function<value, key>
{
    key operator()(const value& val)
    {
        return val.first;
    }
};

template <class T>
struct KeyEqual : public unary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y)
    {
        return x == y;
    }
};
}