#include <string>
#include <iostream>
using namespace std;
class Sloution{
public:
    int myAtoi(string s)
    {
        int result = 0;
        bool ifPositive = true;
        bool ifAcceed = false;
        int size = s.size();
        for (int i = 0; i < size; ++i)
        {
            if (s[i] == ' '){
                if (ifAcceed)
                    return result;
            }
            else if (s[i] == '+'){
                if (!ifAcceed){
                    ifAcceed = true;
                    ifPositive = true;
                }
                else 
                    return result;
            }
            else if (s[i] == '-'){
                if (!ifAcceed){
                    ifAcceed = true;
                    ifPositive = false;
                }else 
                    return result;
            }
            else if (s[i] >= '0' && s[i] <= '9')
            {
                if(!ifAcceed)
                {
                    ifAcceed = true;
                }
                if (ifPositive)
                {
                    if (result > INT_MAX / 10 || (result == INT_MAX / 10 && s[i] - '0' >= 8)) return INT_MAX;
                    result = result * 10 + (s[i] - '0');
                }
                else 
                {
                    if (result < INT_MIN / 10 || (result == INT_MIN / 10 && '0' - s[i] <= -8)) return INT_MIN;
                    result = result * 10 + ('0' - s[i]);
                }
            }else{
                return result;
            }
        }
        return result;
    }
};

int main()
{
    string str("0+1");
    Sloution a;
    cout << a.myAtoi(str) << endl;
}