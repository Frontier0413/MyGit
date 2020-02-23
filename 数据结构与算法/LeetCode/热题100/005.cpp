#include <string>
#include <algorithm>
using namespace std;

class solution{
    string longestPalindrome(string& s)
    {
        int len = s.size();
        if (len == 0 || len ==1)
            return s;
        int mlen = 0;
        int start = 0;
        int end = 0;
        for (int i = 0; i < len; ++i)
        {
            int len1 = centerMaxPalindrome(s, i, i);
            int len2 = centerMaxPalindrome(s, i, i + 1);
            mlen = max(max(len1, len2), mlen);
            if (mlen > end - start)
            {
                start = i - (mlen - 1) / 2;
                end = i + mlen / 2 + 1;
            }
        }
        return s.substr(start, mlen);
    }

    int centerMaxPalindrome(string& s, int left, int right)
    {
        int len = s.size();
        while (left >= 0 && right < len && s[left] == s[right])
        {
            --left;
            ++right;
        }
        return right - left - 1;
    }    

};