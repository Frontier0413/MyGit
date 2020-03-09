#include <vector>
#include <algorithm>
using namespace std;

class Solution
{
public:
    vector<vector<int>> fourSum(vector<int>& nums, int target)
    {
        vector<vector<int>> result;
        int length = nums.size();
        sort(nums.begin(), nums.end());
        for (int index = 0; index < length - 3;)
        {
            for (int i = index + 1; i < length - 2;)
            {
                int left = i + 1, right = length - 1;
                int target1 = target - nums[index] - nums[i];
                while (left < right)
                {
                    int sum = nums[left] + nums[right];
                    if (sum == target1)
                    {
                        vector<int> tmp;
                        tmp.push_back(nums[index]);
                        tmp.push_back(nums[i]);
                        tmp.push_back(nums[left]);
                        tmp.push_back(nums[right]);
                        result.push_back(tmp);
                        while (left < right && nums[left] == nums[left + 1]) ++left;
                        ++left;
                        while (left < right && nums[right] == nums[right - 1]) --right;
                        --right;
                    }
                    else if (sum < target1)
                    {
                        while (left < right && nums[left] == nums[left + 1]) ++left;
                        ++left;
                    }
                    else
                    {
                        while (left < right && nums[right] == nums[right - 1]) --right;
                        --right;
                    }
                }
                while (i < length - 1 && nums[i] == nums[i + 1]) ++i;
                ++i;
            }
            while (index < length - 1 && nums[index] == nums[index + 1]) ++index;
            ++index;
        }
        return result;
    }
};

int main()
{
    vector<int> vec{0,0,0,0};
    Solution a;
    a.fourSum(vec, 0);
}