#include <string>
#include <iostream>
#include <vector>
using namespace std;
class Solution {
public:
    string convert(string s, int numRows) {
        if (numRows == 1) return s;
        string result;
        int size= s.size();
        int interval = 2 * numRows - 2;
        for (int i = 0, inte = interval; i < numRows; ++i, inte -= 2){
            for (int index = i, k = inte; index < size;)
            {
                result.push_back(s[index]);
                if (k == 0 || k == interval)
                    index += interval;
                else{
                    index += k;
                    k = interval - k;
                }
            }
        }
        return result;
    }
};

int main()
{
    ////////////PAHNAPLSIIGYYIR
    string str("PAYPALISHIRING");
    Solution a;
    string res = a.convert(str, 3);
    cout << res << endl;
}