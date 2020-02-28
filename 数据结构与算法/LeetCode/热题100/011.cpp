#include <vector>
#include <iostream>
using namespace std;
class Solution
{
public:
    int maxArea(vector<int>& height)
    {
        int left = 0,right = height.size() - 1;
        int maxArea = 0, area = 0;
        while (left < right)
        {
            if (height[left] < height[right])
            {
                area = (right - left) * height[left];
                if (area > maxArea) maxArea = area;
                ++left;
            }
            else 
            {
                area = (right - left) * height[right];
                if (area > maxArea) maxArea = area;
                --right;
            }
        }
        return maxArea;
    }
};