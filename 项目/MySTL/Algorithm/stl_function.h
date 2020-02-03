template <class T>
struct less 
{
    bool operator()(const T& x, const T& y) const 
    {
        return x < y;
    }
};

template <class Iter>
struct less_iter
{
    bool operator()(const Iter& x, const Iter& y)
    {
        return *x < *y;
    }
};