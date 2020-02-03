#include "RBTree.h"
#include <vector>
#include <random>
using namespace std;

int main()
{
    vector<int> a{41, 38, 31, 12, 19, 8};
    RBTree<int> tree;
    for(int i = 0; i < a.size(); ++i)
    {
        tree.insert(a[i]);
    }
    tree.show();
}    