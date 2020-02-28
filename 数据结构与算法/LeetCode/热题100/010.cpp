#include <string>
#include <iostream>
using namespace std;
class Solution {
public:
    bool isMatch(string s, string p) {
        int lens = s.size(), lenp = p.size();
        if (lenp == 0) return lens ==0;
        if (lens == 0) return (lenp >= 2 && p[1] == '*') && isMatch(s, p.substr(2, lenp));
        bool firstMatch = (s[0] == p[0] || p[0] == '.');
        if (lenp >= 2 && p[1] == '*')
            return isMatch(s, p.substr(2, lenp)) || (firstMatch && isMatch(s.substr(1, lens), p));
        else 
            return firstMatch && isMatch(s.substr(1, lens), p.substr(1, lenp));
    }

};

int main()
{
    Solution a;
    cout << a.isMatch("aa", "a");
    cout << a.isMatch("aa", "a*");
    cout << a.isMatch("ab", ".*");
    cout << a.isMatch("aab", "c*a*b");
    cout << a.isMatch("mississippi", "mis*is*p*.");
    cout << a.isMatch("acbcb", ".*cbcb");
    cout << a.isMatch("acbcb", ".*cb");
    cout << a.isMatch("aaa", "a*a");
    cout << a.isMatch("aa", "a*");
    cout << a.isMatch("", "a*b*c*");
    cout << a.isMatch("", "a*b*c");
}