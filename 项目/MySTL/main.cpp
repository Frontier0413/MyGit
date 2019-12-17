#include <iostream>
#include <vector>
#include <memory>
#include <string.h>
#include "myhead.h"
#include "Contant/stl_vector.h"
#include "Contant/stl_list.h"
#pragma pack(2)
struct test1
{
    char a;
    double b;
    int c;
    int d;
    int e;
};

struct test2
{
    char s;
    int i;
    test1 t;
};

int&& test()
{
    int a = 1;
    return std::move(a);
}

int main()
{
   std::cout << test() << std::endl;
}