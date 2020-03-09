#include <vector>
#include <algorithm>
#include <iostream>
#include <stack>
using namespace std;

class Solution {
public:
    bool isValid(string s) {
        if (s == "") return true;
        stack<char> sta;
        for (int i = 0; i < s.size(); ++i)
        {
            if (s[i] == '(' || s[i] == '{' || s[i] == '[') sta.push(s[i]);
            else
            {
                if (s[i] == ')')
                {
                    if (!sta.empty() && sta.top() == '(')
                    {
                        sta.pop();
                    }
                    else return false;
                }
                else if (s[i] == '}')
                {
                    if (!sta.empty() && sta.top() == '{')
                    {
                        sta.pop();
                    }
                    else return false;
                }
                else if (s[i] == ']')
                {
                    if (!sta.empty() && sta.top() == '[')
                    {
                        sta.pop();
                    }
                    else return false;
                }
            }
        }
        if (sta.empty()) return true;
        return false;
    }
};

int main()
{
    Solution a;
    cout << a.isValid("]") << endl;
    return 0;
}