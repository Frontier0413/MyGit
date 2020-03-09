#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

using namespace std;
class Solution
{
public:
    vector<string> letterCombinations(string& digits)
    {
        int size = digits.size();
        vector<string> result;
        if (size == 0) return result;
        result = {""};
        for (int i = 0; i < size; ++i)
        {
            vector<string> tmp;
            if (digits[i] == '7')
            {
                for (int j = 0; j < result.size(); ++j)
                {
                    tmp.push_back(result[j] + 'p');
                    tmp.push_back(result[j] + 'q');
                    tmp.push_back(result[j] + 'r');
                    tmp.push_back(result[j] + 's');
                }
            }
            else if (digits[i] == '8')
            {

                for (int j = 0; j < result.size(); ++j)
                {
                    tmp.push_back(result[j] + 't');
                    tmp.push_back(result[j] + 'u');
                    tmp.push_back(result[j] + 'v');
                }
            }
            else if (digits[i] == '9')
            {
                for (int j = 0; j < result.size(); ++j)
                {
                    tmp.push_back(result[j] + 'w');
                    tmp.push_back(result[j] + 'x');
                    tmp.push_back(result[j] + 'y');
                    tmp.push_back(result[j] + 'z');
                }
            }
            else 
            {
                for (int j = 0; j < result.size(); ++j)
                {
                    int k = (digits[i] - '2') * 3;
                    char s1 = 'a' + k;
                    char s2 = 'a' + k + 1;
                    char s3 = 'a' + k + 2;
                    tmp.push_back(result[j] + s1);
                    tmp.push_back(result[j] + s2);
                    tmp.push_back(result[j] + s3);
                }
            }
            result = tmp;
        }
        return result;
    }
};
