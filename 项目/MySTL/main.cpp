#include <iostream>
#include <time.h>
#include "Contant/stl_vector.h"

int main()
{
	clock_t start_time = clock();
	vector<int> myvec;
	myvec.insert(myvec.end(), 1, 777);
	myvec.insert(myvec.end(), 1, 777);
	myvec.insert(myvec.end(), 1, 777);
	myvec.insert(myvec.end(), 1, 777);
	myvec.insert(myvec.end(), 1, 777);
	myvec.insert(myvec.begin(), 2, 4396);
	for(int i = 0; i < myvec.size(); ++i)
		std::cout << myvec[i] << std::endl;
	clock_t end_time = clock();
	std::cout << double(end_time - start_time)/CLOCKS_PER_SEC*1000 << "ms" << std::endl;
}