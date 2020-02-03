template <class T1, class T2>
struct pair
{
    T1 first;
    T2 second;

    pair() : first(T1()), second(T2()) { }
    pair(const T1& x, const T2& y) : first(x), second(y) { }

    bool operator==(const pair<T1, T2>& x)
    {
        return (first == x.first) && (second == x.second);
    }
};

template <class T1, class T2>
pair<T1, T2> make_pair(const T1& x, const T2& y)
{
    return pair<T1, T2>(x, y);
}