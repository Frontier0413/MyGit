#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;


struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};
 

class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode* fward = head;
        for (int i = 0; i < n; ++i) fward = fward->next;
        if (fward == nullptr) return head->next;
        fward = fward->next;
        ListNode* p = head;
        while (fward != nullptr)
        {
            fward = fward->next;
            p = p->next;
        }
        p->next = p->next->next;
        return head;
    }
};