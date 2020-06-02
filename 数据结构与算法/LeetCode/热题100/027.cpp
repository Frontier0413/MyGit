#include <vector>
#include <iostream>
using namespace std;

class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        int i = 0, length = nums.size();
        for (int j = 0; j < length; ++j)
        {
            while (j < length && nums[j] == val) ++j;
            if (j < length)
            {
                nums[i] = nums[j];
                ++i;
            }
        }
        return i;
    }
};