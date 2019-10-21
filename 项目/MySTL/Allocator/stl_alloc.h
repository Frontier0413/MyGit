#include <new>
#include <stdlib.h>
#define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)

//first level allocator
template <int inst>
class __malloc_alloc_template
{
private:

    //the out of memory malloc function
    static void* oom_malloc(size_t n)
    {
        void (* my_malloc_handler)();
        void* result;

        for( ; ; )
        {
            //get the handler
            my_malloc_handler = __malloc_alloc_oom_handler;

            //if the handler is null,throw bad alloc
            if(0 == my_malloc_handler) { __THROW_BAD_ALLOC; }

            //if not,call the handler
            (*my_malloc_handler)();

            //assign memory again
            result = malloc(n);

            //if assignment is successful, return
            //if not, do the for loop again
            if(result) return result;
        }
    }

    //do as the same way as function oom_malloc
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

    //the out of memory handler
    static void (* __malloc_alloc_oom_handler)();

public:

    static void* allocate(size_t n)
    {
        //if fail to alloc
        //call the oom_alloc
        void* result = malloc(n);
        if(0 == result) result = oom_malloc(n);
        return result;
    }

    static void* deallocate(void* p, size_t)
    {
        free(p);
    }

    //call the c function realloc(p,n)
    //release the memory where p pointed, realloc a new memory and return the pointer.
    //when call the function realloc
    //there is no direct connection between the new memory and the old one.
    static void* reallocate(void* p, size_t, size_t new_sz)
    {
        void* result = realloc(p, new_sz);
        if(0 == result) result = oom_realloc(p, new_sz);
        return result;
    }

    //accept a void(*)() type pointer, set the oom_handler and return the old handler.
    static void (*set_malloc_handler ( void (*f)() ))()
    {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old;
    }

};
template<int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

typedef __malloc_alloc_template<0> malloc_alloc;


//the second level allocator

enum { __ALIGN = 8 };//atomic unit
enum { __MAX_BYTES = 128 };//max bytes,if bigger than 128,call the first level allocator
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

template <int inst>
class __default_alloc_temlpate
{
private:
    //round up to multiple of 8
    static size_t ROUND_UP(size_t bytes)
    {
        return ((bytes + __ALIGN -1) & ~(__ALIGN - 1));
    }

private:
    union obj{
        //point to next memory
        union obj* free_list_link;
        //used for client access memory
        char client_data[1];
    };

private:

    //16 free-lists
    static obj* volatile free_list[__NFREELISTS] = 
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    //choose the serial number
    static size_t FREELIST_INDEX(size_t bytes)
    {
        return ((bytes + __ALIGN - 1) / (__ALIGN - 1));
    }

    //refill the number n/8 list
    static void* refill(size_t n)
    {
        //default number is 20
        int nobjs = 20;

        //assgin the memory
        char* chunk = chunk_alloc(n, nobjs);

        obj* volatile * my_free_list;
        obj* result;
        obj* current_obj, * next_boj;
        int i;

        //if the last memory is only enough for one object, return directly.
        if( 1 == nobjs ) return chunk;

        //if the last memory is enough
        //get the number n/8 list pointer
        my_free_list = free_list + FREELIST_INDEX(n);

        //the first memory for one object will return to client
        result = (obj*)chunk;
        //get the second memory pointer
        *my_free_list = next_obj = (obj*)(chunk + n);
        
        //fill the list
        for(i = 1; ; i++)
        {
            current_obj = next_boj;

            //get the next memory pointer
            next_obj = (obj*)((char*)next_obj + n);

            //save to link list
            //begin from 1,because the number 0 has to return to client
            if(nobjs - 1 == i)
            {
                current_obj -> free_list_link = 0;
                break;
            }
            else
            {
                current_obj -> free_list_link = next_boj;
            }
        }
        return result;
    }

    //the memory pool assignment function
    static char* chunk_alloc(size_t size, int& nobjs)
    {
        char* result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;

        //if the memory pool is full enough to assign nobjs memory
        if( bytes_left >= total_bytes)
        {
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        //if the memory pool is not enough,but can allocate the memory needed for al least one object
        else if (bytes_left >= size)
        {
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        //if the left memory can not allocated for one object
        else
        {
            //allocate twice the demand memory and a additional number 
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

            //add the left memory to link list
            if(bytes_left > 0)
            {
                obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
                ((obj*)start_free) -> free_list_link = *my_free_list;
                *my_free_list = (obj*)start_free;
            }

            //call the malloc function to allocate memory
            start_free = (char*)malloc(bytes_to_get);
            
            //if failed to allocate
            if(0 == start_free)
            {
                int i;
                obj* volatile * my_free_list, * p;

                //check free list, see if there is some chunk wo can use
                for(i = size; i <= __MAX_BYTES; i += __ALIGN)
                {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if( 0 != p)
                    {
                        //add *p to memory pool
                        *my_free_list = p->free_list_link;
                        start_free = (char*)p;
                        end_free = start_free + i;
                        //now the bytes_left has changed,
                        //recursive call itself,to modify nobjs
                        return (chunk_alloc(size, nobjs));
                    }
                }

                //if we can`t find memory anywhere to use
                //call the malloc_alloc::allocate function,there is a handler
                end_free = 0;
                start_free = (char*)malloc_alloc::allocate(bytes_to_get);
            }

            //if the first allocate function does not throw a bad alloc
            //that menas memory has been allocated sucessfuly
            //call itself again ,to modify nobjs.
            heap_szie += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return (chunk_alloc(size, nobjs));
        }
    }

    static char* start_free = 0;
    static char* end_free = 0;
    static size_t heap_size = 0;

public:
    static void* allocate(size_t n)
    {
        obj* volatile* my_free_list;
        obj* result;
        //if n is greater than 128, call the first level allicator
        if( n > (size_t) __MAX_BYTES )
        {
            return (malloc_alloc::allocate(n));
        }

        my_free_lise = free_list + FREELIST_INDEX(n);
        result = *my_free_list;

        //if the list is null, refill it again
        if( result == 0 )
        {
            void* r = refill(ROUND_UP(n));
            return r;
        }
        
        //return the first block memory
        *my_free_list = result -> free_list_link;
        return result;
    }

    static void deallocate(void* p, size_t n)
    {
        obj* q = (obj*)p;
        obj* volatile* my_free_list;

        //if n is greater than 128, call the first level allocator
        if( n > (size_t)__MAX_BYTES )
        {
            malloc_alloc::deallocate(p, n);
            return;
        }

        //add this block memory to link list
        my_free_list = free_list + FREELIST_INDEX(n);
        q -> free_list_link = *my_free_list;
        *my_free_list = q;
    }

    static void* reallocate(void* p, size_t old_sz, size_t new_sz)
    {
        void* result;
        size_t copy_sz;

        //if the old size and the new size are greater than 128, call the realloc function directly
        if(old_sz > (size_t)__MAX_BYTES && new_sz > (size_t)__MAX_BYTES)
        {
            return realloc(p, new_sz);
        }

        //the second level allocator only allocate memory that multiple of 8
        //if their round up is the same, return p directly.
        if(ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;

        //allocate a new memory and copy old memory to the new one
        result = malloc(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        //deallocate the old memory
        deallocate(p, old_sz);
        return result;
    }

};

//if use the first level or the second one
#ifdef __USE_MALLOC
    typedef malloc_alloc alloc;
#else
    typedef __default_alloc_temlpate<0> alloc;
#endif

template<class T,class alloc>
class simple_alloc{
public:
    static T* allocate(size_t n)
    {
        return 0 == n ? 0 : (T*)alloc::allocate(n * sizeof(T));
    }
    static T* allocate(void)
    {
        return (T*)alloc::allocate(sizeof(T));
    }
    static void deallocate(T* p, size_t n)
    {
        if( 0 != n) alloc::deallocate(p, n * sizeof(T));
    }
    static void deallocate(T* p)
    {
        alloc::deallocate(p, sizeof(T));
    }
};