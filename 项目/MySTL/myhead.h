#include <iostream>
#include <vector>
#include <list>
#include "Contant/stl_vector.h"
#include "Contant/stl_list.h"
#include "Contant/stl_deque.h"

namespace hxl
{
	template <class T>
void show_vector(vector<T> &vec)
{
	//std::cout << "capcity is " << vec.capacity() << std::endl;
	for( auto i = 0; i < vec.size(); ++i)
		std::cout << vec[i] << " ";
	std::cout << std::endl;
}

template <class T>
void show_vector(std::vector<T> &vec)
{
	//std::cout << "capcity is " << vec.capacity() << std::endl;
	for( auto i = 0; i < vec.size(); ++i)
		std::cout << vec[i] << " ";
	std::cout << std::endl;
}

template <class T>
void show_list(list<T>& x)
{
	auto first = x.begin();
	while(first != x.end())
		std::cout << *first++ << " ";
	std::cout << std::endl;
}

template <class T>
void show_list(std::list<T>& x)
{
	auto first = x.begin();
	while(first != x.end())
		std::cout << *first++ << " ";
	std::cout << std::endl;
}

template <class T, unsigned int N>
void show_array(T (&a)[N])
{
	for( int i = 0; i < N; ++i)
		std::cout << a[i] << " ";
	std::cout << std::endl;
}

template <class T>
void show_deque(deque<T>& x)
{
	auto first = x.begin();
	while(first != x.end())
		std::cout << *first++ << " ";
	std::cout << std::endl;
}
}