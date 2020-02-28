#include <string>
#include <iostream>
using namespace std;
class Solution 
{
public:
    string intToRoman(int num)
    {
        int roman = 0;
        string result;
        while (num > 0)
        {
            if (num >= 1000)
            {
                roman = num / 1000;
                while(roman--) result.push_back('M');
                num %= 1000;
            }
            else if(num >= 500)
            {
                roman = num / 100;
                if (roman == 9){
                    result += string("CM");
                    num -= 900;
                }
                else 
                {
                    result.push_back('D');
                    num -= 500;
                }
            }
            else if (num >= 100)
            {
                roman = num / 100;
                if (roman == 4){
                    result += string("CD");
                    num -= 400;
                }
                else 
                {
                    while(roman--) result.push_back('C');
                    num %= 100;
                }
            }
            else if (num >= 50)
            {
                roman = num / 10;
                if (roman == 9)
                {
                    result += string("XC");
                    num -= 90;
                }
                else 
                {
                    result.push_back('L');
                    num -= 50;
                }
            }
            else if (num >= 10)
            {
                roman = num / 10;
                if (roman == 4)
                {
                    result += string("XL");
                    num -= 40;
                }
                else 
                {
                    while(roman--) result.push_back('X');
                    num %= 10;
                }
            }
            else if (num >= 5)
            {
                if (num == 9)
                {
                    result += string("IX");
                    num -= 9;
                }
                else 
                {
                    result.push_back('V');
                    num -= 5;
                }
            }
            else 
            {
                if (num == 4)
                {
                    result += string("IV");
                    num -= 4;
                }
                else 
                {
                    while(num--) result.push_back('I');
                }
            }
        }
        return result;
    }
};

int main()
{
    Solution a;
    cout << a.intToRoman(3) << endl;
    cout << a.intToRoman(4) << endl;
    cout << a.intToRoman(9) << endl;
    cout << a.intToRoman(58) << endl;
    cout << a.intToRoman(1994) << endl;
    cout << a.intToRoman(10) << endl;
}