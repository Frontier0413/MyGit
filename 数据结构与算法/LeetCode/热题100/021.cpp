#include <vector>
#include <algorithm>
#include <iostream>
#include <stack>
using namespace std;


struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        if (l1 == nullptr) return l2;
        if (l2 == nullptr) return l1;
        ListNode* result = new ListNode(0);
        ListNode* temp = result;
        while (l1 && l2)
        {
            if (l1->val < l2->val)
            {
                result->next = l1;
                l1 = l1->next;
            }
            else
            {
                result->next = l2;
                l2 = l2->next;
            }
            result = result->next;
        }
        if (l1) result->next = l1;
        else result->next = l2;
        return temp->next;
    }
};