#pragma once
#include <cstddef>
#include "../Allocator/stl_alloc.h"
#include "../Allocator/stl_uninitialized.h"

template <class T, class Alloc = alloc>
class vector
{
public:
	//定义vector类型别名
	typedef T			value_type;
	typedef value_type* pointer;
	typedef value_type* iterator;
	typedef value_type& reference;
	typedef size_t		size_type;
	typedef ptrdiff_t	defference_type;

protected:
	//默认使用第二级配置器
	typedef simple_alloc<value_type, Alloc> data_allocator;
	//start为内存开始位置，finish为尾元素后位置，end_of_storage为内存结束位置
	iterator start;
	iterator finish;
	iterator end_of_storage;

	//分配n个T类型所需的内存，并使用x为初值初始化
	iterator allocate_and_fill(size_type n, const T& x)
	{
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}

	void insert_aux(iterator position, const T& x);

	//释放start到end_of_storage区间内的内存
	void deallocate()
	{
		if (start)
		{
			data_allocator::deallocate(start, end_of_storage - start);
		}
	}
	
	//分配n个T类型所需的内存并使用value初始化
	void fill_initialize(size_type n, const T& value)
	{
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

public:
	//返回首元素迭代器
	iterator begin() const { return start; }
	//返回尾后迭代器
	iterator end() const { return finish; }
	//返回vector元素的长度，即start到finish
	size_type size() const
	{
		return size_type(end() - begin());
	}
	//返回vector内存长度，即start到end_of_storage
	size_type capacity() const
	{
		return size_type(end_of_storage - begin());
	}
	//判断vector元素是否为空，为空返回true
	bool empty() const
	{
		return begin() == end();
	}

	//返回第n个元素的引用
	reference operator[](size_type n)
	{
		return *(begin() + n);
	}

	//返回首元素的引用
	reference front()
	{
		return *begin();
	}

	//返回尾元素的引用
	reference back()
	{
		return *(end() - 1);
	}
	//end()是尾后位置，而back()是尾元素

	//添加value元素到vector的最后位置
	//如果内存不足，调用insert_aux()
	void push_back(const T& value)
	{
		//判断内存是否足够添加元素
		if (finish != end_of_storage)
		{
			construct(finish, value);
			++finish;
		}
		else
		{
			insert_aux(end(), value);
		}
	}

	//删除最后一个元素
	void pop_back()
	{
		--finish;
		destory(finish);
	}

	//删除迭代器position指代的元素,返回删除前position的下一个元素位置
	iterator erase(iterator position)
	{
		//如果position不是最后一个元素，则需要调整位置，将position后面的元素向前挪动一个单位，同时删除最后一个元素
		//如果是最后一个元素，则直接删除即可
		if (position + 1 != end())
		{
			copy(position + 1, finish, position);
		}
		//删除元素之后，vector长度减一，删除最后一个元素。
		--finish;
		//vector的迭代器是原生指针，直接调用destory函数释放内存
		destory(finish);
		//返回删除元素的下一元素,如果是删除最后一个元素，则返回尾后迭代器。
		return position;
	}

	//删除first、last区间的元素
	//返回区间的下一个元素迭代器
	iterator erase(iterator first, iterator last)
	{
		//将last到first区间的元素拷贝到first开始的区间
		iterator i = copy(last, finish, first);
		//将后面的元素删除
		destory(i, finish);
		//重置finish位置
		finish = finish - (last - first);
		return first;
	}

	//将vector长度设置为new_size
	//如果new_size长度比原长度小，则从后删除多余元素
	//如果new_size长度比原长度大，则添加元素，并用初值x初始化
	void resize(size_type new_size, const T& x)
	{
		if (new_size < size())
			erase(begin() + new_size, end());
		else
			insert(end(), new_size - size(), x);
	}

	//重置vector长度，如果添加元素，使用默认值初始化
	void resize(size_type new_size)
	{
		resize(new_size, T());
	}

	//删除start到finish区间所有元素
	void clear()
	{
		erase(begin(), end());
	}

	void insert(iterator position, size_type n, const T& x);

public:
	vector() : start(0), finish(0), end_of_storage(0) { }

	//构造函数，vector初始化为n个元素，初值为value
	vector(size_type n, const T& value)
	{
		fill_initialize(n, value);
	}

	//构造函数，vector初始化为n个元素，初值为value
	vector(int n, const T& value)
	{
		fill_initialize(n, value);
	}

	//构造函数，vector初始化为n个元素，初值为value
	vector(long n, const T& value)
	{
		fill_initialize(n, value);
	}

	//构造函数，vector初始化为n个元素，初值为默认初始化
	explicit vector(size_type n)
	{
		fill_initialize(n, T());
	}

	//析构函数，销毁元素，释放start到end_of_storage的内存
	~vector()
	{
		destory(start, finish);
		deallocate();
	}
};

//在position指代的迭代器位置前，插入元素x，后面的元素依次向后移动
template<class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x)
{
	//如果内存足够插入元素
	if (finish != end_of_storage)
	{
		//在finish位置构造一个新元素，并将finish位置向后移动一位
		construct(finish, *(finish - 1));
		++finish;
		T x_copy = x;
		//将区间position到finish-2从后开始拷贝到finish-1开始的区间
		copy_backward(position, finish - 2, finish - 1);
		//令position位置为拷贝的元素
		*position = x_copy;
	}
	//如果内存不足以添加元素
	else
	{
		const size_type old_size = size();
		//如果原长度为0，则新长度为1，如果原长度不为0，则新长度为原长度的两倍
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		//分配新长度所需的内存
		iterator new_start = data_allocator::allocate(len);
		iterator new_finish = new_start;

		try
		{
			//将原vector中start到position区间元素拷贝到new_start开始的区间
			new_finish = uninitialized_copy(start, position, new_start);
			//用x作为初值在下一位置构造元素
			construct(new_finish, x);
			++new_finish;
			//再讲posotion到finish区间内元素拷贝到新vector内。
			new_finish = uninitialized_copy(position, finish, new_finish);
		}
		catch (...) {
			//如果出现异常，释放申请的内存
			destory(new_start, new_finish);
			data_allocator::deallocate(new_start, len);
			throw;
		}

		//释放原vector内存
		destory(begin(), end());
		deallocate();
		//重置vector位置
		start = new_start;
		finish = new_finish;
		end_of_storage = new_start + len;
	}
}

//在position位置前插入n个元素，使用x为初值初始化
template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
{
	if (n != 0)
	{
		//如果剩余内存足够插入n个元素
		if (size_type(end_of_storage - finish) >= n)
		{
			//
			T x_copy = x;
			//elems_after为插入位置之后的元素
			const size_type elems_after = finish - position;
			//记录之前finish位置
			iterator old_finish = finish;
			//如果插入位置之后的元素多于要插入的元素
			if (elems_after > n)
			{
				//对于finish往后的位置，是未初始化的内存，因此不能直接调用赋值运算符
				//必须调用构造函数，因此这里的处理是对于未初始化的内存和已经初始化的内存分开处理
				uninitialized_copy(finish - n, finish, finish);
				finish += n;
				copy_backward(position, old_finish - n, old_finish);
				fill(position, position + n, x_copy);

			}
			else
			{
				//对于elems_after小于n的情况，插入元素会出现在finish位置后
				//因此直接在finish位置后构造对应数目的元素，避免先移动后拷贝
				uninitialized_fill_n(finish, n - elems_after, x_copy);
				finish += n - elems_after;
				uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				fill(position, old_finish, x_copy);
			}
		}
		//如果剩余内存不足以插入元素
		else
		{
			const size_type old_size = size();
			//如果插入元素的长度大于vector原长度，则新长度为old+n，否则为两倍原长度
			const size_type len = old_size + (old_size > n ? old_size : n);
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try
			{
				//先将start到position的元素拷贝到新内存
				//再在新内存构造要插入的元素
				//最后将position到finish拷贝到新内存
				new_finish = uninitialized_copy(start, position, new_start);
				new_finish = uninitialized_fill_n(new_finish, n, x);
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...)
			{
				//如果出现异常，析构对象并释放内存
				destory(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}
			//释放原vector内存，重置标志位
			destory(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}