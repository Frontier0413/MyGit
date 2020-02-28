#include <iostream>
using namespace std;
class Solution{
public:
    int reverse(int x)
    {
        int result = 0;
        int m;
        while (x){
            if (result > INT_MAX / 10) return 0;
            if (result < INT_MIN / 10) return 0;
            m = x % 10;
            result = result * 10 + m;
            x = x / 10;
        }
        return result;
    }
};

int main()
{
    char a = '0', b = '9';
     cout << b - a << endl;
}