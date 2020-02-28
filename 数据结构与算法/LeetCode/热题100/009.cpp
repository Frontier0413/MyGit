#include <iostream>
#include <stack>
using namespace std;
class Solution {
public:
    bool isPalindrome(int x) {
        if (x < 0)
            return false;
        if (x % 10 == 0 && x != 0)
            return false;
        int right = 0;
        while (x > right)
        {
            right = right * 10 + x % 10;
            x = x / 10;
        }
        return x == right || x == right / 10;
    }
};

int main()
{
    Solution a;
    cout << a.isPalindrome(-1) << endl;
    cout << a.isPalindrome(123) << endl;
    cout << a.isPalindrome(121) << endl;
    cout << a.isPalindrome(1221) << endl;
    cout << a.isPalindrome(10) << endl;
    cout << a.isPalindrome(2147483647) << endl;
}