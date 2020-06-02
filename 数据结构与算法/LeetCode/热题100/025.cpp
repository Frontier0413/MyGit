#include <vector>
#include <iostream>
#include <stack>
using namespace std;

struct ListNode
{
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) { }
};


class Solution {
public:
    ListNode* reverseKGroup(ListNode* head, int k) {
        if (head == nullptr) return nullptr;
        ListNode* result = head;
        ListNode* cur = head;
        ListNode* tmp;
        stack<ListNode*> aux;
        for (int i = 0; i < k; ++i)
        {
            if (cur == nullptr) return result;
            cur = cur->next;
        }
        while (head != cur)
        {
            aux.push(head);
            head = head->next;
        }
        result = aux.top();
        aux.pop();
        tmp = result;
        while (!aux.empty())
        {
            tmp->next = aux.top();
            aux.pop();
            tmp = tmp->next;
        }
        tmp->next = cur;
        while (true)
        {
            for (int i = 0; i < k; ++i)
            {
                if (cur == nullptr) return result;
                cur = cur->next;
            }
            while (head != cur)
            {
                aux.push(head);
                head = head->next;
            }
            while (!aux.empty())
            {
                tmp->next = aux.top();
                aux.pop();
                tmp = tmp->next;
            }
            tmp->next = cur;
        }
    }
};

ListNode* getList(vector<int>& a)
{
    ListNode* result = nullptr;
    ListNode* ret = nullptr;
    ListNode* tmp;
    for (int i = 0; i < a.size(); ++i)
    {
        tmp = new ListNode(a[i]);
        if (result == nullptr) { result = tmp; ret = result; }
        else
        {
            result->next = tmp;
            result = tmp;
        }
    }
    return ret;
}

int main()
{
    vector<int> listvec{ 1, 2, 3, 4, 5 };
    ListNode* l = getList(listvec);
    Solution s;
    s.reverseKGroup(l, 2);
}