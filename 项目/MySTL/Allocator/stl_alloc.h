#pragma once
#include <new>
#include <stdlib.h>
#include <string.h>
#define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)

namespace hxl
{
//stl第一级配置器
template <int inst>
class __malloc_alloc_template
{
private:

    //内存不足时的内存分配函数
    //该函数会反复调用处理程序，直到malloc成功分配n字节内存时返回
    //或当处理程序为空时，直接抛出异常。
    static void* oom_malloc(size_t n)
    {
        void (* my_malloc_handler)();
        void* result;

        for( ; ; )
        {
            my_malloc_handler = __malloc_alloc_oom_handler;
            if(0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
            (*my_malloc_handler)();
            result = malloc(n);
            if(result) return result;
        }
    }

    //该函数会反复调用处理程序，直到realloc成功分配n字节内存时返回
    //或当处理程序为空时，直接抛出异常
    static void* oom_realloc(void* p, size_t n)
    {
        void (* my_malloc_handler)();
        void* result;

        for( ; ; )
        {
            my_malloc_handler = __malloc_alloc_oom_handler;
            if( 0 == my_malloc_handler ) { __THROW_BAD_ALLOC; }
            (*my_malloc_handler)();
            result = realloc(p, n);
            if(result) return result;
        }
    }

    //内存不足时的处理函数。
    static void (* __malloc_alloc_oom_handler)();

public:

    //分配n字节内存，如果分配失败，调用oom_malloc。
    static void* allocate(size_t n)
    {
        void* result = malloc(n);
        if(0 == result) result = oom_malloc(n);
        return result;
    }

    //释放指针p所指内存
    static void deallocate(void* p, size_t)
    {
        free(p);
    }

    //重新分配内存函数，释放p原本的内存，重新分配new_sz个字节的内存并返回指针。
    //如果分配失败，调用oom_realloc函数.
    static void* reallocate(void* p, size_t, size_t new_sz)
    {
        void* result = realloc(p, new_sz);
        if(0 == result) result = oom_realloc(p, new_sz);
        return result;
    }

    //设置处理函数
    static void (*set_malloc_handler ( void (*f)() ))()
    {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old;
    }

};

//处理程序默认为空
template<int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;


typedef __malloc_alloc_template<0> malloc_alloc;


//第二级配置器
//第二级配置器为解决内零头问题，只分配小于128字节的内存，如果大于128字节，调用第一配置器
//且当需要的内存不为8的倍数时，自动提升为8的整数倍
enum { __ALIGN = 8 };
enum { __MAX_BYTES = 128 };
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

template <int inst>
class __default_alloc_temlpate
{
private:
    //将bytes提升至8的整数倍
    static size_t ROUND_UP(size_t bytes)
    {
        return ((bytes + __ALIGN -1) & ~(__ALIGN - 1));
    }

private:
    //链表结构体，数组长度为1表示不定数组，即该结构体可以表示不同字节的内存
    //实际上，将下面代码的数组部分删除也不会有影响，但是client_data提供了一种访问该内存块保存数据的方式。
    union obj{
        union obj* free_list_link;
        char client_data[1];
    };

private:

    //16个自由链表头，分别管理8到128字节的内存单元
    static obj* volatile free_list[__NFREELISTS];

    //选择bytes字节对应的链表
    static size_t FREELIST_INDEX(size_t bytes)
    {
        return ((bytes + __ALIGN - 1) / (__ALIGN - 1));
    }

    //重新填充n字节对应的链表,默认每次分配20个内存单元
    static void* refill(size_t n)
    {
        int nobjs = 20;

        //从内存池获取内存
        char* chunk = chunk_alloc(n, nobjs);

        obj* volatile * my_free_list;
        obj* result;
        obj* current_obj, * next_obj;
        int i;

        //如果内存池分配的内存只够一个内存单元使用，直接返回
        if( 1 == nobjs ) return chunk;

        //如果内存池返回的内存单元足够，将剩下的加入到链表中
        my_free_list = free_list + FREELIST_INDEX(n);
        result = (obj*)chunk;
        //chunk + n即去除返回的一个内存单元外的第一个内存单元
        *my_free_list = next_obj = (obj*)(chunk + n);
    
        //从除返回的内存单元外的第一个内存单元开始，依次加入到链表中
        for(i = 1; ; i++)
        {
            current_obj = next_obj;
            //先将next_obj转化为char*,因为n对应的单位是字节。
            next_obj = (obj*)((char*)next_obj + n);

            if(nobjs - 1 == i)
            {
                current_obj -> free_list_link = 0;
                break;
            }
            else
            {
                current_obj -> free_list_link = next_obj;
            }
        }
        return result;
    }

    //内存池分配函数，分配nobjs个size字节的连续内存单元,并返回首地址指针。
    //如果内存池内存足够，直接分配内存并返回
    //如果内存池不够，但足够分配至少一个以上的内存单元，则将内存池中剩余内存都分配给链表。
    //如果内存池一点内存都没有了,则重新分配内存。
    //如果重新分配内存失败。则检查其他链表中，是否有多余的内存可用，如果有，先分配给当前所需的链表
    //（分配前将内存池中剩余内存加入到对应链表中，物尽其用），如果没有，调用第一级配置器（处理程序）。
    static char* chunk_alloc(size_t size, int& nobjs)
    {
        char* result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;

        //如果内存池剩余内存足够满足需求，直接从内存池中分配对应的内存出去。
        if( bytes_left >= total_bytes)
        {
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        
        //内存池不足以分配nobjs个内存单元，但可以分配一个以上的内存单元
        //将内存池剩余内存都分配出去，修改nobjs。
        else if (bytes_left >= size)
        {
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        }

        //内存池连分配一个内存单元的内存都没有了
        else
        {
            //重新分配的内存等于两倍的需求量加上一个递增的附加量。
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

            //将当前内存池剩余内存添加到对应链表中。
            if(bytes_left > 0)
            {
                obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
                //将剩余内存转化为obj结构体，存入链表中
                ((obj*)start_free) -> free_list_link = *my_free_list;
                *my_free_list = (obj*)start_free;
            }

            //调用malloc分配新内存，如果分配失败，检查其他链表，看是否有内存可以使用。
            start_free = (char*)malloc(bytes_to_get);
            if(0 == start_free)
            {
                int i;
                obj* volatile * my_free_list, * p;

                for(i = size; i <= __MAX_BYTES; i += __ALIGN)
                {
                    //从size对应的链表开始，依次检查链表中是否有内存可用
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if( 0 != p)
                    {
                        *my_free_list = p->free_list_link;
                        start_free = (char*)p;
                        end_free = start_free + i;
                        //如果其他链表中有内存可用，将其添加到内存池中，重新调用chunk_alloc函数
                        return (chunk_alloc(size, nobjs));
                    }
                }
                //如果链表中一点内存都没有了，调用第一级配置器（二级配置器没有处理程序）
                end_free = 0;
                start_free = (char*)malloc_alloc::allocate(bytes_to_get);
            }
            //如果代码执行到这里，说明内存已经成功分配
            //更新内存池开始与结束位置，重新调用chunk_alloc分配函数.
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return (chunk_alloc(size, nobjs));
        }
    }

    static char* start_free;//内存池开始位置
    static char* end_free;//内存池结束位置
    static size_t heap_size;//用于重新分配内存的附加量

public:
    //内存分配函数，分配n个字节的内存并返回指针
    static void* allocate(size_t n)
    {
        obj* volatile* my_free_list;
        obj* result;
        //大于128字节的内存转用第一级配置器
        if( n > (size_t) __MAX_BYTES )
        {
            return (malloc_alloc::allocate(n));
        }

        //从对应链表中取出内存单元
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        //如果链表空了，调用refill重新填充对应链表
        if( result == 0 )
        {
            void* r = refill(ROUND_UP(n));
            return r;
        }
        //返回第一块内存后，链表头指向下一块内存
        *my_free_list = result -> free_list_link;
        return result;
    }

    //内存释放函数，将p对应内存释放，并加入到对应链表中
    static void deallocate(void* p, size_t n)
    {
        obj* q = (obj*)p;
        obj* volatile* my_free_list;
        //std::cout << "deallocating..." << std::endl;
        //大于128字节的内存单元调用第一级配置器
        if( n > (size_t)__MAX_BYTES )
        {
            malloc_alloc::deallocate(p, n);
            return;
        }

        //将内存单元添加到对应链表中
        my_free_list = free_list + FREELIST_INDEX(n);
        //q指向当前链表头，链表头指向q
        q -> free_list_link = *my_free_list;
        *my_free_list = q;
    }

    //重新分配内存函数，将p
    static void* reallocate(void* p, size_t old_sz, size_t new_sz)
    {
        void* result;
        size_t copy_sz;

        //大于128字节调用第一级配置器
        if(old_sz > (size_t)__MAX_BYTES && new_sz > (size_t)__MAX_BYTES)
        {
            return realloc(p, new_sz);
        }
        
        //如果重新分配大小不变，则直接返回
        if(ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;

        //重新分配内存，并将原内存数据拷贝到新内存中
        result = malloc(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        //调用deallocate将原内存回收
        deallocate(p, old_sz);
        return result;
    }

};

template <int inst>
typename __default_alloc_temlpate<inst>::obj* volatile
__default_alloc_temlpate<inst>::free_list[__NFREELISTS] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,} ;

template <int inst>
char* __default_alloc_temlpate<inst>::start_free = 0;

template <int inst>
char* __default_alloc_temlpate<inst>::end_free = 0;

template <int inst>
size_t __default_alloc_temlpate<inst>::heap_size = 0;

//sgi默认使用第二级配置器
#ifdef __USE_MALLOC
    typedef malloc_alloc alloc;
#else
    typedef __default_alloc_temlpate<0> alloc;
#endif


//simple_alloc类为配置器标准接口
//封装第二级配置器使其符合std规范
template<class T,class alloc>
class simple_alloc{
public:
    //分配n个T类型所需的内存
    static T* allocate(size_t n)
    {
        return 0 == n ? 0 : (T*)alloc::allocate(n * sizeof(T));
    }
    //分配一个T类型所需的内存
    static T* allocate(void)
    {
        return (T*)alloc::allocate(sizeof(T));
    }
    //回收从p开始的n个元素的内存
    static void deallocate(T* p, size_t n)
    {
        if( 0 != n) alloc::deallocate(p, n * sizeof(T));
    }
    //回收指针p所指的内存
    static void deallocate(T* p)
    {
        alloc::deallocate(p, sizeof(T));
    }
};
}