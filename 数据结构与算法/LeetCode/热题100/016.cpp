#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
class Solution {
public:
    int threeSumClosest(vector<int>& nums, int target) {
        int length = nums.size();
        sort(nums.begin(), nums.end());
        int result = target - nums[0] - nums[1] - nums[length - 1];
        if (length == 3) return target - result;
        int distance = 0;
        for (int index = 0; index < length - 2; ++index)
        {
            int left = index + 1, right = length - 1;
            while (left < right)
            {
                distance = target - nums[index] - nums[left] - nums[right];
                if (distance == 0) return target;
                else if (abs(distance) < abs(result)) result = distance;

                if (distance < 0) --right;
                else ++left;
            }
        }
        return target - result;
    }
};

int main()
{
    Solution a;
    vector<int> testVec{-1, 1, 2, -4};
    cout << a.threeSumClosest(testVec, 1) << endl;
}