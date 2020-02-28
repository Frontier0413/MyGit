#include <string>
#include <vector>
using namespace std;
class Solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        if(strs.size() == 0) return "";
        if (strs.size() == 1) return strs[0];
        string result = longestPrefix(strs[0], strs[1]);
        for(int i = 2; i < strs.size(); ++i)
        {
            result = longestPrefix(result, strs[i]);
        }
        return result;
    }

    string longestPrefix(string& a, string& b)
    {
        string result;
        for(int i = 0; i < a.size() && i < b.size(); ++i)
        {
            if(a[i] == b[i]) result.push_back(a[i]);
            else 
                return result;
        }
        return result;
    }
};