#include <vector>
#include <iostream>
using namespace std;

struct ListNode
{
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) { }
};


class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        if (head == nullptr) return nullptr;
        if (head->next == nullptr) return head;
        ListNode* result = head->next;
        ListNode* tmp = nullptr;
        while (head && head->next)
        {
            if (tmp) tmp->next = head;
            tmp = head->next;
            head->next = tmp->next;
            tmp->next = head;
            tmp = head;
            head = head->next;
        }
        return result;
    }
};