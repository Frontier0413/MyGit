#include <vector>
#include <algorithm>
using namespace std;
class Solution {
public:
    int threeSumClosest(vector<int>& nums, int target) {
        int result = 0;
        int secTarget;
        int length = nums.size();
        sort(nums.begin(), nums.end());
        for(int index = 0; index < length - 2; ++index)
        {
            int distace = target - nums[index];
        }
    }
};