#include <string>
#include <iostream>
using namespace std;

class Solution{
    int romanToInt(const string& s)
    {
        int result = 0;
        for (int i = s.size() - 1; i >= 0; --i)
        {
            if (s[i] == 'M')
            {
                if (i > 0 && s[i - 1] == 'C'){
                    --i;
                    result += 900;
                }
                else result += 1000;
            }
            else if (s[i] == 'D')
            {
                if (i > 0 && s[i - 1] == 'C'){
                    --i;
                    result += 400;
                }
                else result += 500;
            }
            else if (s[i] == 'C')
            {
                if (i > 0 && s[i - 1] == 'X'){
                    --i;
                    result += 90;
                }
                else result += 100;
            }
            else if (s[i] == 'L')
            {
                if (i > 0 && s[i - 1] == 'X'){
                    --i;
                    result += 40;
                }
                else result += 50;
            }
            else if (s[i] == 'X')
            {
                if (i > 0 && s[i - 1] == 'I'){
                    --i;
                    result += 9;
                }
                else result += 10;
            }
            else if (s[i] == 'V')
            {
                if (i > 0 && s[i - 1] == 'I'){
                    --i;
                    result += 4;
                }
                else result += 5;
            }
            else result += 1;
        }
        return result;
    }
};