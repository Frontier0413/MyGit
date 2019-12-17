#pragma once
#include "type_traits.h"

template <class type>
struct Is_Integer
{
    using Integer = __false_type;
};

template<>
struct Is_Integer<bool>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<char>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<signed char>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<unsigned char>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<char16_t>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<char32_t>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<wchar_t>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<short>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<unsigned short>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<int>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<unsigned int>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<long>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<unsigned long>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<long long>
{
    using Integer = __true_type;
};

template<>
struct Is_Integer<unsigned long long>
{
    using Integer = __true_type;
};