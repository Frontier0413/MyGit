#include <deque>
#include <iostream>
#include <stack>
enum RBTreeNodeColor{Red, Black};

template <class T>
struct RBTreeNode
{
    static RBTreeNode* NIL;
    RBTreeNodeColor color;
    RBTreeNode* left;
    RBTreeNode* right;
    RBTreeNode* parent;
    T value;
    //创建红黑树节点时，其left，right，parent指针默认值都为NIL
    RBTreeNode(const T& _value, RBTreeNodeColor _color, RBTreeNode* null_ptr = NIL) :  
        value(_value), color(_color),left(null_ptr), right(null_ptr), parent(null_ptr) { }
};

template <class T>
RBTreeNode<T>* RBTreeNode<T>::NIL = new RBTreeNode<T>(T(),Black, nullptr);

template <class T>
class RBTree
{
public:
    RBTreeNode<T>* min_node(RBTreeNode<T>* x)
    {
        while(x->left != RBTreeNode<T>::NIL)
        {
            x = x->left;
        }
        return x;
    }

    RBTreeNode<T>* max_node(RBTreeNode<T>* x)
    {
        while(x->right != RBTreeNode<T>::NIL)
        {
            x = x->right;
        }
        return x;
    }

    RBTreeNode<T>* search(const T key)
    {
        RBTreeNode<T>* x = root;
        while(x != RBTreeNode<T>::NIL && x->value != key)
        {
            if(x->value < key)
                x = x->right;
            else 
                x = x->left;
        }
        return x;
    }

    RBTreeNode<T>* successor(RBTreeNode<T>* x)
    {
        if(x->right != RBTreeNode<T>::NIL)
        {
            return min_node(x->right);
        }
        else 
        {
            RBTreeNode<T>* y = x->parent;
            while(x != y->left && y != RBTreeNode<T>::NIL)
            {
                x = y;
                y = x->parent;
            }                 
            return y;                                                                               
        }
    }

    RBTreeNode<T>* prededcessor(RBTreeNode<T>* x)
    {
        if(x->left != NIL)
        {
            return max_node(x->left);
        }
        else 
        {
            RBTreeNode<T>* y = x->parent;
            while(x != y->right && y != RBTreeNode<T>::NIL)
            {
                x = y;
                y = x->parent;
            }
            return y;
        }
    }

protected:
    void left_rotate(RBTreeNode<T>* x)
    {
        RBTreeNode<T>* y = x->right;
        x->right = y->left;
        if(y->left != RBTreeNode<T>::NIL)
        {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if(x->parent == RBTreeNode<T>::NIL)
        {
            root = y;
        }
        else if(x == x->parent->left)
        {
            x->parent->left = y;
        }
        else 
        {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void right_rotate(RBTreeNode<T>* x)
    {
        RBTreeNode<T>* y = x->left;
        x->left = y->right;
        if(y->right != RBTreeNode<T>::NIL)
        {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if(x->parent == RBTreeNode<T>::NIL)
        {
            root = y;
        }
        else if(x == x->parent->left)
        {
            x->parent->left = y;
        }
        else 
        {
            x->parent->right = y;
        }
        y->right = x;
        x->parent = y;
    }

    void transplant(RBTreeNode<T>* x, RBTreeNode<T>* y)
    {
        if(x->parent == RBTreeNode<T>::NIL)
        {
            root = y;
        }
        else if (x == x->parent->left)
        {
            x->parent->left = y;
        }
        else 
        {
            x->parent->right = y;
        }
        y->parent = x->parent;
    }

    void tree_insert_fixup(RBTreeNode<T>* z)
    {
        while(z->parent->color == Red)
        {
            if(z->parent == z->parent->parent->left)
            {
                RBTreeNode<T>* y = z->parent->parent->right;
                if(y->color == Red)
                {
                    y->color = Black;
                    z->parent->color = Black;
                    z->parent->parent->color = Red;
                    z = z->parent->parent;
                }
                else
                {
                    if(z == z->parent->right)
                    {
                        z = z->parent;
                        left_rotate(z);
                    }
                    z->parent->color = Black;
                    z->parent->parent->color = Red;
                    right_rotate(z->parent->parent);
                }
            }
            else 
            {
                RBTreeNode<T>* y = z->parent->parent->left;
                if(y->color == Red)
                {
                    y->color = Black;
                    z->parent->color = Black;
                    z->parent->parent->color = Red;
                    z = z->parent->parent;
                }
                else
                {
                    if(z == z->parent->left)
                    {
                        z = z->parent;
                        right_rotate(z);
                    }
                    z->parent->color = Black;
                    z->parent->parent->color = Red;
                    left_rotate(z->parent->parent);
                }
            }
        }
        root->color = Black;
    }

    void tree_erase_fixup(RBTreeNode<T>* z)
    {
        //p用来记录x的兄弟节点
        RBTreeNode<T>* p;
        //当z为红时，只需要简单的改变z的颜色既能保持红黑树的性质
        while(z != root && z->color == Black)
        {
            //当y为黑色节点时，该函数才会被调用，此时y不为NIL，因此y的兄弟节点必定存在且不为NIL，x取代了y的位置，因此，x的兄弟节点必定存在且不为NIL。
            //在后续上溯的过程中，x不可能为NIL，因此x的兄弟不可能为NIL。

            if(z == z->parent->left)
            {
                p = z->parent->right;
                //case 1
                if(p->color == Red)                                                                                                                     []
                {
                    p->color = Black;
                    z->parent->color = Red;
                    left_rotate(z->parent);
                    //当p为Red时，其子节点不可能为NIL
                    p = z->parent->right;
                }
                //case 2
                if(p->left->color == Black && p->right->color == Black)
                {
                    p->color = Red;
                    z = z->parent;
                }
                //case 3 & 4
                else
                {
                    if(p->right->color == Black)
                    {
                        p->left->color = Black;
                        p->color = Red;
                        right_rotate(p);
                        p = z->parent->right;
                    }
                    p->color = z->parent->color;
                    z->parent->color = Black;
                    p->right->color = Black;
                    left_rotate(z->parent);
                    z = root;
                }
            }
            else
            {
                p = z->parent->left;
                if(p->color == Red)
                {
                    p->color = Black;
                    z->parent->color = Red;
                    right_rotate(z->parent);
                    p = z->parent->left;
                }
                if(p->right->color == Black && p->left->color == Black)
                {
                    p->color = Red;
                    z = z->parent;
                }
                else
                {
                    if(p->left->color == Black)
                    {
                        p->right->color = Black;
                        p->color = Red;
                        left_rotate(p);
                        p = z->parent->left;
                    }
                    p->color = z->parent->color;
                    z->parent->color = Black;
                    p->left->color = Black;
                    right_rotate(z->parent);
                    z = root;
                }
            }
        }
        z->color = Black;
    }

public:
    void insert(const T& value)
    {
        RBTreeNode<T>* p = new RBTreeNode<T>(value, Red);
        RBTreeNode<T>* y = RBTreeNode<T>::NIL;
        RBTreeNode<T>* x = root;
        while(x != RBTreeNode<T>::NIL)
        {
            y = x;
            if(x->value < p->value)
                x = x->right;
            else 
                x = x->left;
        }
        p->parent = y;
        if(y == RBTreeNode<T>::NIL)
        {
            root = p;
        }
        else if(y->value < p->value)
        {
            y->right = p;
        }
        else 
        {
            y->left = p;
        }
        tree_insert_fixup(p);
    }

    void erase(RBTreeNode<T>* z)
    {
        //始终用y来记录被删除的节点，当z有一个子节点为空时，y为被删除的z节点，当两个子节点都不为空时，y为实际被删除的z的后继节点，
        RBTreeNode<T>* y = z;
        RBTreeNode<T>* x;
        RBTreeNodeColor y_ori_color = y->color;
        if (z->left == RBTreeNode<T>::NIL)
        {
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == RBTreeNode<T>::NIL)
        {
            x = z->left;
            transplant(z, z->left);
        }
        else 
        {
            y = min_node(z->right);
            y_ori_color = y->color;
            x = y->right;
            //如果y是z的直接子节点，则无须改变y的右节点
            //否则，则需要令y的右子节点为z的右子节点
            if(y->parent != z)
            {
                //当y不是z的直接子节点时，需要将y移动到z位置，因此使用y->right代替y。
                //此时因为y->right为y的直接子节点，且y没有左子节点，因此无需改变y->right的节点指针。
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            else 
            {
                //x可能为NIL，因此这一行不可缺少
                x->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            //与普通二叉树不同的是，此时取代z节点的y节点将继承z节点的颜色
            //此时相当于删除了节点y，用x节点取代了y
            y->color = z->color;
            delete z;
        }
        //y_ori_color用来记录被取代的位置的颜色，如果z只有一个节点，则其为z的颜色，如果z有两个节点，则其记录z节点的后继节点的颜色
        if(y_ori_color == Black)
        {
            //x用来记录用来取代被删除节点的节点，如果z有一个子节点为NIL，则x为z的另一个子节点；如果z有两个子节点，则x为z的后继节点的右子节点。在删除z后，x用来取代y
            tree_erase_fixup(x);
        }
    }

public:
    void show()
    {
        std::deque<RBTreeNode<T>* > deq;
        deq.push_back(root);
        while(!deq.empty())
        {
            if(deq.front() != RBTreeNode<T>::NIL)
            {
                std::cout << deq.front()->value << " ";
                if(deq.front()->color == Red)
                    std::cout << "Red" << std::endl;
                else 
                    std::cout << "Black" << std::endl;
                deq.push_back(deq.front()->left);
                deq.push_back(deq.front()->right);
            }
            else 
            {
                std::cout << "NIL" << std::endl;
            }
            deq.pop_front();
        }
    }

    void in_order_show()
    {
        RBTreeNode<T>* x = root;
        std::stack<RBTreeNode<T>* > stk;
        while(x != RBTreeNode<T>::NIL || !stk.empty())
        {
            while(x != RBTreeNode<T>::NIL)
            {
                stk.push(x);
                x = x->left;
            }
            if(!stk.empty())
            {
                x = stk.top();
                stk.pop();
            }
            std::cout << x->value << std::endl;
            if(x->right != RBTreeNode<T>::NIL)
                x = x->right;
            else
                x = RBTreeNode<T>::NIL;
        }
    }

//private:
    RBTreeNode<T>* root = RBTreeNode<T>::NIL;
};
