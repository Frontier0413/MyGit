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
	//返回vector内存长度，即finish到end_of_storage
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
		return position;
	}

	iterator erase(iterator first, iterator last)
	{
		iterator i = copy(last, finish, first);
		destory(i, finish);
		finish = finish - (last - first);
		return first;
	}

	void resize(size_type new_size, const T& x)
	{
		if (new_size < size())
			erase(begin() + new_size, end());
		else
			insert(end(), new_size - size(), x);
	}

	void resize(size_type new_size)
	{
		resize(new_size, T());
	}

	void clear()
	{
		erase(begin(), end());
	}

	void insert(iterator position, size_type n, const T& x);

public:
	vector() : start(0), finish(0), end_of_storage(0) { }

	vector(size_type n, const T& value)
	{
		fill_initialize(n, value);
	}

	vector(int n, const T& value)
	{
		fill_initialize(n, value);
	}

	vector(long n, const T& value)
	{
		fill_initialize(n, value);
	}

	explicit vector(size_type n)
	{
		fill_initialize(n, T());
	}

	~vector()
	{
		destory(start, finish);
		deallocate();
	}
};

template<class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x)
{
	if (finish != end_of_storage)
	{
		construct(finish, *(finish - 1));
		++finish;
		T x_copy = x;
		copy_backward(position, finish - 2, finish - 1);
		*position = x_copy;
	}
	else
	{
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		iterator new_start = data_allocator::allocate(len);
		iterator new_finish = new_start;

		try
		{
			new_finish = uninitialized_copy(start, position, new_start);
			construct(new_finish, x);
			++new_finish;
			new_finish = uninitialized_copy(position, finish, new_finish);
		}
		catch (...) {
			destory(new_start, new_finish);
			data_allocator::deallocate(new_start, len);
			throw;
		}

		destory(begin(), end());
		deallocate();

		start = new_start;
		finish = new_finish;
		end_of_storage = new_start + len;
	}
}

template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
{
	if (n != 0)
	{
		if (size_type(end_of_storage - finish) >= n)
		{
			T x_copy = x;
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n)
			{
				uninitialized_copy(finish - n, finish, finish);
				finish += n;
				copy_backward(position, old_finish - n, old_finish);
				fill(position, position + n, x_copy);
			}
			else
			{
				uninitialized_fill_n(finish, n - elems_after, x_copy);
				finish += n - elems_after;
				uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				fill(position, old_finish, x_copy);
			}
		}
		else
		{
			const size_type old_size = size();
			const size_type len = old_size + (old_size > n ? old_size : n);
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try
			{
				new_finish = uninitialized_copy(start, position, new_start);
				new_finish = uninitialized_fill_n(new_finish, n, x);
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...)
			{
				destory(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}
			destory(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}