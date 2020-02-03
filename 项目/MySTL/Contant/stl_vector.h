#pragma once
#include <cstddef>
#include <initializer_list>
#include "../Allocator/stl_alloc.h"
#include "../Allocator/stl_uninitialized.h"
#include "../TypeTraits/IsInteger.h"

template <class T, class Ref, class Ptr>
class __vector_iterator
{
public:
	using value_type = T;
	using reference = Ref;
	using pointer = Ptr;
	using difference_type = ptrdiff_t;
	using iterator_category = random_access_iterator_tag;

	using self = __vector_iterator<T, Ref, Ptr>;
	//using iterator = __vector_iterator<T, T&, T*>;
	//using const_iterator = __vector_iterator<T, const T&, const T*>;
protected:
	T* p;

public:
	__vector_iterator(T* t) : p(t) { }

	__vector_iterator() : p(nullptr) { }

	__vector_iterator(const self& x) : p(x.p) { }


	self& operator=(const self& x)
	{
		p = x.p;
		return *this;
	}

	reference operator*() const
	{
		return *p;
	}

	pointer operator->() const 
	{
		return p;
	}

	bool operator==(const self& x) const
	{
		return p == x.p;
	}

	bool operator!=(const self& x) const 
	{
		return !(p == x.p);
	}

	bool operator<(const self& x) const 
	{
		return p < x.p;
	}

	bool operator>(const self& x) const 
	{
		return x.p < p;
	}

	bool operator<=(const self& x) const 
	{
		return !(x.p < p);
	}

	bool operator>=(const self& x) const 
	{
		return !(p < x.p);
	}

	self& operator++()
	{
		++p;
		return *this;
	}

	self operator++(int)
	{
		self tmp = *this;
		++p;
		return tmp;
	}

	self& operator+=(difference_type n)
	{
		p += n;
		return *this;
	}

	self operator+(difference_type n)
	{
		self tmp = *this;
		return tmp += n;
	}

	self& operator--()
	{
		--p;
		return *this;
	}

	self operator--(int)
	{
		self tmp = *this;
		--p;
		return tmp;
	}

	self& operator-=(difference_type n)
	{
		p -= n;
		return *this;
	}

	self operator-(difference_type n)
	{
		self tmp = *this;
		return tmp -= n;
	}

	difference_type operator-(const self& x) const 
	{
		return p - x.p;
	}
};

template <class T, class Alloc = alloc>
class vector
{
public:
	//定义vector类型别名
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = const value_type *;
	using iterator = __vector_iterator<T, T&, T*>;
	using const_iterator = __vector_iterator<T, const T&, const T*>;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

protected:
	//默认使用第二级配置器
	using data_allocator = simple_alloc<value_type, Alloc>;
	//start为内存开始位置，finish为尾元素后位置，end_of_storage为内存结束位置
	pointer start;
	pointer finish;
	pointer end_of_storage;

	//分配n个T类型所需的内存，并使用x为初值初始化,返回指向首地址的指针
	pointer allocate_and_fill(size_type n, const T& x)
	{
		pointer result = data_allocator::allocate(n);
		try
		{
			uninitialized_fill_n(result, n, x);
			return result;
		}
		catch (...)
		{
			data_allocator::deallocate(result, n);
		}
	}

	//分配n个字节的内存，并将first到last区间的元素拷贝的申请的内存上,返回首地址指针
	template <class ForwardIterator>
	pointer allocate_and_copy(size_type n, ForwardIterator first, ForwardIterator last)
	{
		pointer result = data_allocator::allocate(n);
		try
		{
			uninitialized_copy(first, last, result);
			return result;
		}
		catch (...)
		{
			data_allocator::deallocate(result, n);
		}
	}

	//释放start到end_of_storage区间内的内存
	void deallocate()
	{
		if (start)
		{
			data_allocator::deallocate(start, end_of_storage - start);
		}
	}

	//分配n个T类型所需的内存并使用value初始化,设置内存标志位
	void fill_initialize(size_type n, const T &value)
	{
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

public:
	//返回首元素迭代器
	iterator begin() { return start; }
	//const vector表示的是vector的元素不可改变，可以认为vector的元素都是const的，因此可用Const_iterator
	const_iterator begin() const { return start; }

	//返回尾后迭代器
	iterator end() { return finish; }
	const_iterator end() const { return finish; }

	//返回vector元素的长度，即start到finish
	size_type size() const
	{
		return size_type(end() - begin());
	}

	//返回vector容器所能提升到的最大长度
	size_type max_size() const
	{
		return size_type(-1) / sizeof(T);
	}

	//返回vector内存长度，即start到end_of_storage
	size_type capacity() const
	{
		return size_type(end_of_storage - start);
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

	const_reference operator[](size_type n) const
	{
		return *(begin() + n);
	}

	//返回下标为n的引用，但先检查是否越界
	reference at(size_type n)
	{
		range_check(n);
		return (*this)[n];
	}

	//返回首元素的引用
	reference front()
	{
		return *begin();
	}

	const_reference front() const
	{
		return *begin();
	}

	//返回尾元素的引用
	reference back()
	{
		return *(end() - 1);
	}

	const_reference back() const
	{
		return *(end() - 1);
	}
	//end()是尾后位置，而back()是尾元素

	//添加value元素到vector的最后位置
	//如果内存不足，调用insert_aux()
	void push_back(const T &value)
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

	//将vector与x互换
	void swap(vector<T, Alloc> &x)
	{
		std::swap(start, x.start);
		std::swap(finish, x.finish);
		std::swap(end_of_storage, x.end_of_storage);
	}

	//删除最后一个元素
	void pop_back()
	{
		--finish;
		destory(finish);
	}

	//删除迭代器position指代的元素,返回删除之后position位置的迭代器，即指向删除前position下一个元素的迭代器
	iterator erase(iterator position)
	{
		//如果position不是最后一个元素，则需要调整位置，将position后面的元素向前挪动一个单位，同时删除最后一个元素
		//如果是最后一个元素，则直接删除即可
		if (position + 1 != end())
		{
			//在调用赋值运算符的时候，被覆盖的元素需要调用析构函数的就会在赋值函数中自己调用
			copy(position + 1, end(), position);
		}
		//删除元素之后，vector长度减一，删除最后一个元素。
		--finish;
		destory(finish);
		//返回删除元素的下一元素,如果是删除最后一个元素，则返回尾后迭代器。
		return position;
	}

	//删除[first、last)区间的元素
	//返回区间的下一个元素迭代器
	iterator erase(iterator first, iterator last)
	{
		//将last到finish区间的元素拷贝到first开始的区间
		iterator i = copy(last, finish, first);
		//将后面的元素删除
		destory(i, end());
		//重置finish位置
		finish = finish - (last - first);
		return first;
	}

	//将vector元素长度设置为new_size
	//如果new_size长度比原长度小，则从后删除多余元素
	//如果new_size长度比原长度大，则添加元素，并用初值x初始化
	void resize(size_type new_size, const T &x)
	{
		if (new_size < size())
			erase(begin() + new_size, end());
		else
			insert(end(), new_size - size(), x);
	}

	//删除start到finish区间所有元素
	void clear()
	{
		erase(begin(), end());
	}

	//将vector内存长度重置为n，如果n小于原vector内存长度，则什么也不做
	//如果n大于原vector内存，则将原元素拷贝到新内存上.
	void reserve(size_type n)
	{
		//如果n小于当前内存长度，即需要释放内存的话，则reserve什么也不做
		if (capacity() < n)
		{
			//重新申请长度为n的内存，将原内存元素拷贝到新内存上
			const size_type old_size = size();
			pointer new_start = allocate_and_copy(n, start, finish);
			//释放原内存
			destory(start, finish);
			data_allocator::deallocate(start, end_of_storage - start);
			//重新设置内存标志位
			start = new_start;
			finish = start + old_size;
			end_of_storage = start + n;
		}
	}

	//删除所有vector元素，替换为n个初值为value的元素
	//如果n小于vector内存长度，则不会改变现有内存
	void assign(size_type n, const T &value)
	{
		//如果n大于vector的内存长度，
		if (n > capacity())
		{
			//申请一个新的长度为n的vector，与当前vector执行swap操作
			vector<T, Alloc> tmp(n, value);
			tmp.swap(*this);
		}
		else if (n > size())
		{
			//如果n小于内存长度但大于元素长度，对于start到finish，可以直接fill，
			//但是对于finish后的内存，应该调用未初始化的fill函数.
			fill(begin(), end(), value);
			finish = uninitialized_fill_n(finish, n - size(), value);
		}
		else
		{
			//finish在erase函数中被更改
			erase(fill_n(begin(), n, value), end());
		}
	}

	//删除所有元素，替换为区间first,last的元素
	template <class InputIterator>
	void assign(InputIterator first, InputIterator last)
	{
		//对于参数是(size_type, value)的类型，直接调用assign(size_type n, const T& value)；
		using Integer = typename Is_Integer<InputIterator>::Integer;
		assign_dispatch(first, last, Integer());
	}

protected:
	//对于assign(size_type n,T& value)类型的调用
	template <class Integer>
	void assign_dispatch(Integer n, Integer value, __true_type)
	{
		assign(n, value);
	}

	template <class InputIterator>
	void assign_dispatch(InputIterator first, InputIterator last, __false_type)
	{
		//对于不同的迭代器类型，选择不同的辅助函数来达到最高的效率
		assign_aux(first, last, iterator_category(first));
	}

	template <class InputIterator>
	void assign_aux(InputIterator first, InputIterator last, input_iterator_tag)
	{
		//对于输入迭代器，以==判断循环终止，通过累加遍历
		iterator cur = begin();
		for (; first != last && cur != end(); ++cur, ++first)
			*cur = *first;
		//如果first,last区间结束，而cur仍未到达finish，删除之后的元素
		if (first == last)
			erase(cur, end());
		//如果cur已经结束，而first,last区间仍未结束，则调用insert函数将剩余区间元素插入到finish位置。
		else
			insert(end(), first, last);
	}

	template <class ForwardIterator>
	void assign_aux(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
	{
		//对于前向迭代器。可以多次遍历获取元素，因此可以先求得first,last区间的长度
		size_type len = distance(first, last);
		//如果长度大于vector内存长度
		if (len > capacity())
		{
			//申请一个新内存，将first,last区间拷贝到新内存上，并释放原内存
			pointer tmp = allocate_and_copy(len, first, last);
			destory(start, finish);
			data_allocator::deallocate(start, end_of_storage - start);
			start = tmp;
			end_of_storage = finish = start + len;
		}
		//如果n小于元素长度，将first,last区间覆盖原元素，删除多余的元素
		else if (size() >= len)
		{
			pointer new_finish = copy(first, last, start);
			destory(new_finish, finish);
			finish = new_finish;
		}
		//如果n大于元素长度，则对于已经存在的元素，调用赋值运算符覆盖，对于未初始化的部分，调用未初始化的拷贝方式
		else
		{
			ForwardIterator mid = first;
			advance(mid, size());
			copy(first, mid, start);
			finish = uninitialized_copy(mid, last, finish);
		}
	}

	//检查n是否越界
	void range_check(size_type n) const
	{
		if (n >= this->size())
			throw;
	}

public:
	//在position位置前插入n个元素，初值为value
	void insert(iterator position, size_type n, const T &x)
	{
		fill_insert(position, n, x);
	}

	//在position迭代器插入元素x，返回指向该x的迭代器
	iterator insert(iterator position, const T &x)
	{
		size_type n = position - begin();
		//如果当前剩余内存足够插入一个元素且插入位置在finish
		if (finish != end_of_storage && position == end())
		{
			construct(finish, x);
			++finish;
		}
		else
			insert_aux(position, x);
		return begin() + n;
	}

	//检查输入是否为(pos, size, value)的形式，如果是，调用fill_insert，如果不是根据迭代器选择不同的版本
	template <class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last)
	{
		using Integer = typename Is_Integer<InputIterator>::Integer;
		insert_dispatch(pos, first, last, Integer());
	}

protected:
	void fill_insert(iterator position, size_type n, const T &x);

	void insert_aux(iterator position, const T &x);

	template <class Integer>
	void insert_dispatch(iterator position, Integer n, Integer value, __true_type)
	{
		fill_insert(position, (size_type)n, (T)value);
	}

	template <class InputIterator>
	void insert_dispatch(iterator pos, InputIterator first, InputIterator last, __false_type)
	{
		range_insert(pos, first, last, iterator_category(first));
	}

	//将first,last区间插入到pos位置
	template <class InputIterator>
	void range_insert(iterator pos, InputIterator first, InputIterator last, input_iterator_tag);

	template <class ForwardIterator>
	void range_insert(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag);

public:
	vector() : start(0), finish(0), end_of_storage(0) {}

	//构造函数，vector初始化为n个元素，初值为value
	vector(size_type n, const T &value)
	{
		fill_initialize(n, value);
	}

	//构造函数，vector初始化为n个元素，初值为value
	vector(int n, const T &value)
	{
		fill_initialize(n, value);
	}

	//构造函数，vector初始化为n个元素，初值为value
	vector(long n, const T &value)
	{
		fill_initialize(n, value);
	}

	//拷贝构造函数,构造为vector x的副本
	vector(const vector<T, Alloc> &x)
	{
		start = data_allocator::allocate(x.size());
		finish = uninitialized_copy(x.begin(), x.end(), start);
		end_of_storage = finish;
	}

	//接受一对迭代器区间作为初始值初始化
	template <class InputIterator>
	vector(InputIterator first, InputIterator last)
	{
		//判断参数是否是整型，即是否是(size_type , value)这种形式的构造函数，正好出现了数据类型与size类型一致的情况
		using Integer = typename Is_Integer<InputIterator>::Integer;
		unitialized_aux(first, last, Integer());
	}

	vector(std::initializer_list<value_type> l)
	{
		range_initialized(l.begin(), l.end(), random_access_iterator_tag());
	}

protected:
	template <class Integer>
	void unitialized_aux(Integer n, Integer value, __true_type)
	{
		fill_initialize(n, value);
	}

	template <class InputIterator>
	void unitialized_aux(InputIterator first, InputIterator last, __false_type)
	{
		range_initialized(first, last, iterator_category(first));
	}

	//分配适当的内存并使用first到last区间初始化
	template <class InputIterator>
	void range_initialized(InputIterator first, InputIterator last, input_iterator_tag)
	{
		//对于输入迭代器，使用依次调用push_back的方式初始化
		//因为输入迭代器只能进行一次遍历,无法在一开始就知道长度
		for (; first != last; ++last)
			push_back(*first);
	}

	//对于前向迭代器，直接获取长度，调用uninitialized_copy
	template <class ForwardIterator>
	void range_initialized(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
	{
		size_type n = distance(first, last);
		start = data_allocator::allocate(n);
		end_of_storage = start + n;
		finish = uninitialized_copy(first, last, start);
	}

public:
	//赋值函数,将vector x的属性赋给当前vector，赋值过后vector原本的元素被覆盖
	//如果新长度大于原vector的内存长度，则更新end_of_storage,否则只更新finish。
	vector<T, Alloc> &operator=(const vector<T, Alloc> &x)
	{
		//赋值函数应该保证自赋值不会出现问题
		if (&x != this)
		{
			const size_type xlen = x.size();
			//如果x的长度大于vector当前的内存长度
			//构造新的内存，长度为x.size()，并释放vector原来的内存空间
			if (xlen > capacity())
			{
				pointer new_start = allocate_and_copy(xlen, x.begin(), x.end());
				destory(start, finish);
				data_allocator::deallocate(start, end_of_storage - start);
				start = new_start;
				end_of_storage = start + xlen;
			}
			//如果xlen小于vector的size，直接将x的元素拷贝到当前vector内，并销毁多余的元素
			else if (size() >= xlen)
			{
				pointer i = copy(x.begin(), x.end(), start);
				destory(i, finish);
			}
			//如果x的长度大于size小于capacity，直接将x的元素拷贝过去
			//注意未初始化的内存调用uninitialized_copy，已经初始化的内存调用copy。
			else
			{
				copy(x.begin(), x.begin() + size(), start);
				uninitialized_copy(x.begin() + size(), x.end(), finish);
			}
			finish = start + xlen;
		}
		return *this;
	}

	//析构函数，销毁元素，释放start到end_of_storage的内存
	~vector()
	{
		destory(start, finish);
		deallocate();
	}
};

//在position指代的迭代器位置前，插入元素x，后面的元素依次向后移动
template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T &x)
{
	//如果内存足够插入元素
	if (finish != end_of_storage)
	{
		//在finish位置构造一个新元素，并将finish位置向后移动一位
		construct(finish, *(finish - 1));
		++finish;
		T x_copy = x;
		//将区间position到finish-2从后开始拷贝到finish-1开始的区间
		copy_backward(position, end() - 2, finish - 1);
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
		pointer new_start = data_allocator::allocate(len);
		pointer new_finish = new_start;

		try
		{
			//将原vector中start到position区间元素拷贝到new_start开始的区间
			new_finish = uninitialized_copy(begin(), position, new_start);
			//用x作为初值在下一位置构造元素
			construct(new_finish, x);
			++new_finish;
			//再讲posotion到finish区间内元素拷贝到新vector内。
			new_finish = uninitialized_copy(position, end(), new_finish);
		}
		catch (...)
		{
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
void vector<T, Alloc>::fill_insert(iterator position, size_type n, const T &x)
{
	if (n != 0)
	{
		//如果剩余内存足够插入n个元素
		if (size_type(end_of_storage - finish) >= n)
		{
			//因为x是引用类型，需要先记录x的值，避免在函数调用时值被修改。
			T x_copy = x;
			//elems_after为插入位置之后的元素
			const size_type elems_after = finish - position;
			//记录之前finish位置
			pointer old_finish = finish;
			//如果插入位置之后的元素多于要插入的元素
			if (elems_after > n)
			{
				uninitialized_copy(end() - n, end(), finish);
				finish += n;
				copy_backward(position, iterator(old_finish - n), old_finish);
				fill(position, position + n, x_copy);
			}
			else
			{
				//对于elems_after小于n的情况，插入元素会出现在finish位置后
				//因此直接在finish位置后构造对应数目的元素，避免先移动后拷贝
				uninitialized_fill_n(finish, n - elems_after, x_copy);
				finish += n - elems_after;
				uninitialized_copy(position, iterator(old_finish), finish);
				finish += elems_after;
				fill(position, iterator(old_finish), x_copy);
			}
		}
		//如果剩余内存不足以插入元素
		else
		{
			const size_type old_size = size();
			//如果插入元素的长度大于vector原长度，则新长度为old+n，否则为两倍原长度
			const size_type len = old_size + max(old_size, n);
			pointer new_start = data_allocator::allocate(len);
			pointer new_finish = new_start;
			try
			{
				//先将start到position的元素拷贝到新内存
				//再在新内存构造要插入的元素
				//最后将position到finish拷贝到新内存
				new_finish = uninitialized_copy(begin(), position, new_start);
				new_finish = uninitialized_fill_n(new_finish, n, x);
				new_finish = uninitialized_copy(position, end(), new_finish);
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

template <class T, class Alloc>
template <class InputIterator>
void vector<T, Alloc>::range_insert(iterator pos, InputIterator first, InputIterator last, input_iterator_tag)
{
	//对于输入迭代器，没办法事先知道要插入元素的长度，因此只能依次调用insert。
	for (; first != last; ++first)
	{
		pos = insert(pos, *first);
		++pos;
	}
}

template <class T, class Alloc>
template <class ForwardIterator>
void vector<T, Alloc>::range_insert(iterator position, ForwardIterator first, ForwardIterator last, forward_iterator_tag)
{
	if (first != last)
	{
		size_type n = distance(first, last);
		//如果剩余内存足够插入元素
		if (size_type(end_of_storage - finish) >= n)
		{
			const size_type elems_after = finish - position;
			pointer old_finish = finish;
			//如果插入位置之后的元素大于要插入的元素
			if (elems_after > n)
			{
				uninitialized_copy(end() - n, end(), finish);
				finish += n;
				copy_backward(position, iterator(old_finish - n), old_finish);
				copy(first, last, position);
			}
			else
			{
				ForwardIterator mid = first;
				advance(mid, elems_after);
				uninitialized_copy(mid, last, finish);
				finish += (n - elems_after);
				uninitialized_copy(position, iterator(old_finish), finish);
				finish += elems_after;
				copy(first, mid, position);
			}
		}
		//如果内存不足以插入元素
		else
		{
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			pointer new_start = data_allocator::allocate(len);
			pointer new_finish = new_start;

			try
			{
				new_finish = uninitialized_copy(begin(), position, new_start);
				new_finish = uninitialized_copy(first, last, new_finish);
				new_finish = uninitialized_copy(position, end(), new_finish);
			}
			catch (...)
			{
				destory(new_start, new_finish);
				data_allocator::deallcate(start, len);
			}
			destory(start, finish);
			data_allocator::deallocate(start, end_of_storage - start);
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}

template <class T, class Alloc = alloc>
inline bool operator==(const vector<T, Alloc> &x, const vector<T, Alloc> &y)
{
	return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc = alloc>
inline bool operator<(const vector<T, Alloc> &x, const vector<T, Alloc> &y)
{
	return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}
