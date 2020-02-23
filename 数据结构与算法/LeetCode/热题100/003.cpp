#include <vector>
#include <iostream>
#include <unordered_map>
using namespace std;
/*
给定一个字符串，请你找出其中不含有重复字符的最长子串的长度。
示例 1:
输入: "abcabcbb"
输出: 3 
解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3。
示例 2:
输入: "bbbbb"
输出: 1
解释: 因为无重复字符的最长子串是 "b"，所以其长度为 1。
示例 3:
输入: "pwwkew"
输出: 3
解释: 因为无重复字符的最长子串是 "wke"，所以其长度为 3。
     请注意，你的答案必须是子串的长度，"pwke" 是一个子序列，不是子串
*/
class solution{

public:
    int lengthOfLongestSubstring(string s)
    {
        int size = s.size(), maxLength = 0, b = 0;
        int i, j;
        for (i = 0; i < size; ++i)
        {
            for (j = b; j < i; ++j)
            {
                if (s[j] == s[i])
                {
                    b = i + 1;
                    break;
                }
            }
            if (i - b + 1 >maxLength)
                maxLength = i - j + 1;
        }
        return maxLength;
    }
};