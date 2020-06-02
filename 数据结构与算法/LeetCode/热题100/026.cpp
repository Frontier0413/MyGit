#include <vector>
#include <iostream>
using namespace std;

class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        int i = 0, length = nums.size();
        for (int j = 0; j < length; ++i, ++j)
        {
            while (j < nums.size() - 1 && nums[j] == nums[j + 1]) ++j;
            nums[i] = nums[j];
        }
        return i;
    }
};