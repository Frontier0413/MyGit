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
    // ListNode* mergeKLists(vector<ListNode*>& lists) {
    //     ListNode* result = nullptr;
    //     for (int i = 0; i < lists.size(); ++i)
    //     {
    //         result = mergeTwoLists(result, lists[i]);
    //     }
    //     return result;
    // }

    ListNode* mergeKLists(vector<ListNode*>& lists)
    {
        if (lists.size() == 0) return nullptr;
        while (lists.size() != 1)
        {
            vector<ListNode*> tmp;
            for (int i = 0, length = lists.size(); i < length; i += 2)
            {
                if (i + 1 < length) tmp.push_back(mergeTwoLists(lists[i], lists[i + 1]));
                else  tmp.push_back(lists[i]);
            }
            lists.swap(tmp);
        }
        return lists[0];
    }

    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        if (l1 == nullptr) return l2;
        if (l2 == nullptr) return l1;
        ListNode* result = new ListNode(0);
        ListNode* temp = result;
        while (l1 && l2)
        {
            if (l1->val < l2->val)
            {
                if (l1 == l1->next) {
                    cout << "error!" << endl;
                    throw;
                }
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
        result = temp->next;
        delete temp;
        return result;
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
    Solution s;
    vector<int> a{ 1, 4, 5 };
    vector<int> b{ 1, 3, 4 };
    vector<int> c{ 2, 6 };
    vector<ListNode*> listvec;
    listvec.push_back(getList(a));
    listvec.push_back(getList(b));
    listvec.push_back(getList(c));
    s.mergeKLists(listvec);
}