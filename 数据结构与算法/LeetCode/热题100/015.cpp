#include <vector>
#include <algorithm>
using namespace std;
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        vector<vector<int>> result;
        if (nums.size() < 3) return result;
        sort(nums.begin(), nums.end());
        int length = nums.size();
        int left, right, target;
        for(int index = 0; index < length - 2; ++index)
        {
            left = index + 1, right = length - 1;
            target = 0 - nums[index];
            while(left < right)
            {
                if (nums[left] + nums[right] == target)
                {
                    vector<int> tmp{nums[index], nums[left], nums[right]};
                    result.push_back(tmp);
                    while(left < right && nums[left] == nums[left + 1]) ++left;
                    while(left < right && nums[right] == nums[right - 1]) --right;
                    ++left, --right;
                }
                else if(nums[left] + nums[right] < target){
                    while(left < right && nums[left] == nums[left + 1]) ++left;
                    ++left;
                }
                else {
                    while(left < right && nums[right] == nums[right - 1]) --right;
                    --right;
                }
            }
            while (index < length - 1 && nums[index] == nums[index + 1]) ++index;
            ++index;
        }
        return result;
    }
};