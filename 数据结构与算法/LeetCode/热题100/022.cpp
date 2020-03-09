#include <vector>
#include <algorithm>
#include <iostream>
#include <stack>
using namespace std;


struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:
    vector<string> generateParenthesis(int n) {
        vector<string> result;
        if (n == 0) return result;
        result = { "" };
        int leftCount = 0, rightCount = 0;
        //每次将保存的字符串长度+1
        for (int i = 0; i < 2 * n; ++i)
        {
            vector<string> tmp;
            //对当前result里的字符串，依次判断其能添加'('或者')'到末尾。
            for (int j = 0; j < result.size(); ++j)
            {  
                //统计当前字符串中的左括号和右括号数
                for (int k = 0; k < result[j].size(); ++k)
                {
                    if (result[j][k] == '(') ++leftCount;
                    else if (result[j][k] == ')') ++rightCount;
                }
                if (leftCount < n && leftCount == rightCount)
                {
                    tmp.push_back(result[j] + '(');
                }
                else if (leftCount < n && leftCount > rightCount)
                {
                    tmp.push_back(result[j] + '(');
                    tmp.push_back(result[j] + ')');
                }
                else if (leftCount == n && rightCount < n)
                {
                    tmp.push_back(result[j] + ')');
                }
                else
                {

                }
                leftCount = 0, rightCount = 0;
            }
            result = tmp;
        }
        return result;
    }
};

int main()
{
    Solution a;
    vector<string> res(a.generateParenthesis(4));
    cout << res[0] << endl;
}