//
// Created by 梁磊磊 on 2023/4/2.
//

#ifndef IMPLTMENT_STL_MEMORY_STL_ALLOC_H_
#define IMPLTMENT_STL_MEMORY_STL_ALLOC_H_

/*这里定义了一、二级配置器，彼此合作。配置器名为alloc
 * 负责对象构造前的空间配置和对象析构后的空间释放
 * 直接用malloc()和free()完成内存的配置与释放
 * 考虑到小型区块缩可能造成的内存破碎问题，SGI设计了双层级配置器，第一级配置器直接使用malloc()，和free(）函数，
 * 第二级配置器则视情况采用不同的策略；当配置区超过128bytes时，视之为过小，为了降低额外负担，便采用负责的memory pool整理方式，
 * 而不再求助于第一级配置器。整个设计究竟只开放第一级配置器或是同时开放第二级配置器，取决于__use_malloc是否被定义*/




//---------------------------------------------------------------------------------
//定义内存越界的处理方式


#include <new>
#include <cassert>          //assert
#include <cstring>          //memcpy

#define __THROW_BAD_ALLOC throw std::bad_alloc()




//无论alloc被定义为第一级或第二级配置器，SGI还为它再包装一个接口如下，使配置器的接口能够符合STL规格
/*其内部四个成员函数都是单纯地转调用，调用传递给配置器（可能是第一级也可能是第二级）的成员函数，这个接口配置器的配置单位从
 * bytes转为个别元素的大小（sizeof(T)）.*/
template<typename T, class Alloc>
class simple_alloc {
public:
    static T *allocate(size_t n) {
        return 0 == n ? 0 : (T *) Alloc::allocate(n * sizeof(T));
    }

    static T *allocate(void) {
        return (T *) Alloc::allocate(sizeof(T));      //为T类型的指针分配对应字节空间
    }

    static void deallocate(T *p, size_t n) {
        if (0 != n) {
            Alloc::deallocate(p, n * sizeof(T));
        }
    }

    static void deallocate(T *p) {
        Alloc::deallocate(p, sizeof(T));
    }
};

/**第一级配置器*/
/*将一个整数常量作为模板参数来实例化一个类或函数模板
 * 再实例化模版时，必须将一个整数常量作为参数模版传递
 * 模板参数必须是编译时常量，因为模板的实例化发生在编译时而不是运行时
 */
template<int __inst>
class __malloc_alloc_template {
private:
    //用于 在设置了__malloc_alloc_oom_handler 的情况下循环分配内存，直到成功分配
    //这三个都是函数指针，前两个返回void*类型，第三个没有返回值void
    static void *oom_malloc(size_t);

    static void *oom_realloc(void *, size_t);

    static void (*__malloc_alloc_oom_handler)();

    //如果编译器支持模版类静态成员，则使用错误处理函数，类似C++的set_new_handler()
    // 默认值为0，如果不设置，则内存分配失败时直接__THROW_BAD_ALLOC
    /* __malloc_alloc_oom_handler是一个函数指针，用于设置处理内存分配失败的错误处理函数。
     * 在STL中，如果没有足够的内存来分配一个对象，就会调用__malloc_alloc_oom_handler函数指针所指向的函数来处理错误。
     * 默认情况下，该函数指针指向一个内部处理函数，该函数会尝试释放已分配的内存并重新分配内存，以便满足内存需求。
     * 如果仍然无法分配足够的内存，则会抛出std::bad_alloc异常。
     */
#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
    static void (*__malloc_alloc_oom_handler)();
#endif

public:
    //分配指定大小的内存（size_t n）,如果分配失败，则进入循环分配阶段
    //循环分配的前提是要保证正确设置了__malloc_alloc_oom_handler
    static void *allocate(size_t n) {
        void *result = malloc(n);      //第一级配置器直接用malloc分配内存
        if (0 == result) result = oom_malloc(n);
        return result;
    }

    //释放内存
    static void deallocte(void *p, size_t /* n */) {
        free(p);        //第一级配置器直接用free释放内存
    }

    static void *reallocate(void *p, size_t /*old_size*/, size_t new_size) {
        void *result = realloc(p, new_size);        //包含于头文件malloc中
        if (0 == result) result = oom_realloc(p, new_size);
        return result;
    }

    //内存溢出处理函数
    //该函数的参数和返回值都是一个不接收任何参数并且返回值为void的函数指针。
    //f是指定的内存越界处理函数
    static void (*set_malloc_handler(void(*f)()))() {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return (old);
    }

};

//定义三个内存越界处理函数
/* malloc_alloc out-of-memory handling
 * 初值为0，有待客端确定
 * 声明为纯虚函数，创建成抽象类。抽象类是不能被实例化的类，只能用作其他类的基类，其纯虚函数必须在派生类中实现。
 * 设置"内存不足处理例程"是客端的责任
 */
#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG

template<int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

#endif


// 如果设置了__malloc_alloc_oom_handler, 则首先执行错误处理函数, 然后循环分配直到成功
// 如果未设置__malloc_alloc_oom_handler, __THROW_BAD_ALLOC
template<int inst>
void *__malloc_alloc_template<inst>::oom_malloc(size_t n) {
    void (*my_malloc_handler)();
    void *result;

    for (;;) {    //不断尝试释放、配置、再释放、再配置。。。。
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }      // 如果未设置__malloc_alloc_oom_handler, __THROW_BAD_ALLOC
        (*my_malloc_handler)();                             // 执行错误处理函数，企图释放内存或其他操作
        result = malloc(n);                            // 再次尝试配置内存
        if (result) return (result);
    }
}

template<int inst>
void *__malloc_alloc_template<inst>::oom_realloc(void *p, size_t n) {
    void (*my_malloc_handler)();
    void *result;

    for (;;) {             //不断尝试释放、配置、再释放、再配置。。。。
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*my_malloc_handler)();
        result = realloc(p, n);
        if (result) return (result);

    }
}

//直接将参数inst指定为0
typedef __malloc_alloc_template<0> malloc_alloc;


//分配器适配器，用于检查调试的大小参数。
template<class Alloc>
class debug_alloc {
private:
    enum {
        extra = 8       //用来存储分配空间的大小
    };

public:
    // extra 保证不会分配为0的内存空间, 而且要保证内存对齐
    // 把分配内存的最前面设置成n的大小, 用于后面校验
    // 内存对齐的作用就是保护前面extra大小的数据不被修改

    //从堆上分配一块内存空间，并且将这块内存空间的大小写入到该内存空间的起始位置。
    static void *allocate(size_t n) {        //n是分配内存的字节数
        char *result = (char *) Alloc::allocate(n + extra);     /*将void *类型的指针强制转换成一个指向char类型的指针result，
 * 这是因为char类型的指针可以用来访问任何类型的数据，因此可以方便地将指针类型转换成char类型的指针进行处理。*/
        *(size_t *) result = n;         //它的作用是将一个无符号整型数n赋值给一个指向size_t类型的指针result所指向的内存空间。
        return result + extra;          //返回result + extra指向的内存空间的起始位置，这里加上extra是为了跳过额外分配的空间，直接指向分配的内存空间的起始位置。
    }

    //如果*(size_t *)real_p != n则肯定发生向前越界
    static void deallocate(void *p, size_t n) {
        char *real_p = (char *) p - extra;      //跳过额外分配的空间，直接指向分配的内存空间的起始位置
        assert(*(size_t *) real_p == n);
        Alloc::deallocate(real_p, n);
    }

    static void *reallocate(void *p, size_t old_sz, size_t new_sz) {
        char *real_p = (char *) p - extra;
        assert(*(size_t *) real_p == old_sz);
        char *result = (char *) Alloc::reallocate(real_p, old_sz + extra, new_sz + extra);
        *(size_t *) result = new_sz;
        return result + extra;
    }

};


//第二级编译器
//无template型别参数
template<bool threads, int inst>
class __default_alloc_template {
private:

    enum {
        __ALIGN = 8
    };          //增长间隔
    enum {
        __MAX_BYTES = 128
    };    //小型区块的上限
    enum {
        __NFREELISTS = __MAX_BYTES / __ALIGN
    };       //free_lists个数

    /* 向上舍入操作
    * 解释一下, __ALIGN - 1指明的是实际内存对齐的粒度
    * 例如__ALIGN = 8时, 我们只需要7就可以实际表示8个数(0~7)
    * 那么~(__ALIGN - 1)就是进行舍入的粒度
    * 我们将(bytes) + __ALIGN-1)就是先进行进位, 然后截断
    * 这就保证了我是向上舍入的
    * 例如byte = 100, __ALIGN = 8的情况
    * ~(__ALIGN - 1) = (1 000)B
    * ((bytes) + __ALIGN-1) = (1 101 011)B
    * (((bytes) + __ALIGN-1) & ~(__ALIGN - 1)) = (1 101 000 )B = (104)D
    * 104 / 8 = 13, 这就实现了向上舍入
    * 对于byte刚好满足内存对齐的情况下, 结果保持byte大小不变
    * 记得《Hacker's Delight》上面有相关的计算
    * 这个表达式与下面给出的等价
    * ((((bytes) + _ALIGN - 1) * _ALIGN) / _ALIGN)
    * 但是SGI STL使用的方法效率非常高       */

    //这个函数是向上舍入，把内存向上补齐到8的倍数
    static size_t ROUND_UP(size_t bytes) {
        return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
    }

private:
    // 管理内存链表用
    // 为了尽最大可能减少内存的使用, 这里使用一个union
    // 如果使用第一个成员, 则指向另一个相同的union obj
    // 而如果使用第二个成员, 则指向实际的内存区域
    // 这样就实现了链表结点只使用一个指针的大小空间, 却能同时做索引和指向内存区域

    /*`client_data`数组的长度为1，这是因为在C++中，联合体中的所有成员变量都共享同一段内存空间。
     * 因此，为了保证`obj`对象的实际数据可以存储任意长度的数据，通常使用一个长度为1的字符数组来作为`client_data`成员变量，
     * 并通过指针运算来访问实际的数据。*/
    union obj {
        union obj *free_list_link;
        char client_data[1];
    };


private:
    //这里分配的free_list为16
    //对应的内存链容量分别为8，16，32 ... 128
    /*声明了一个名为free_list的静态指针数组
     * 数组中每个元素都是一个指向obj类型指针的volatile指针。*/
    static obj *volatile free_list[__NFREELISTS];

    //根据待分配的空间大小，在free_list中选择合适的大小
    //列表下标从0开始
    static size_t FREELIST_INDEX(size_t bytes) {
        return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
    }

    //返回一个大小为n的对象，并可能加入大小为n的其他区块到freelist
    static void *refill(size_t n);

    //配置一大块空间，可容纳n*objs个大小为size的区块
    static char *chunk_alloc(size_t size, int &nobjs);


    //内存池
    static char *start_free;        //内存池起始点。只在chunk_alloc()中变化
    static char *end_free;          //内存池结束点。只在chunk_alloc()中变化
    static size_t heap_size;        //已经在堆上分配的空间大小

    //先不写多线程
public:
    static void *allocate(size_t n) {
        //声明了一个名为my_free_list的volatile指针，指向obj类型的指针
        obj *volatile *my_free_list;
        obj *result;

        //大于128就直接调用第一级配置器
        if (n > (size_t) __MAX_BYTES) {
            return (malloc_alloc::allocate(n));
        }

        //这里的free_list指向维护自由链表的首地址
        //寻找16个free_list中适当的一个
        //my_free_list等于n字节对应的*obj
        my_free_list = free_list + FREELIST_INDEX(n);
        //result是对my_free_list指向的对象取值，
        result = *my_free_list;
        //如果result为空，说明没找到可用的free_list
        if (result == 0) {
            //没找到可用的free_list,准备重新填充free_list
            void *r = refill(ROUND_UP(n));
            return r;
        }
        //result不为空，找到了可用的free_list，从内存池中拨出这个区块，然后维护free_list数组，使其指向下一个可分配的空间
        *my_free_list = result->free_list_link;
        //返回分配空间的地址
        return (result);

    }

    // 释放内存，该函数首先判断区块大小，大于128bytes就调用第一级配置器，小于128bytes就找出对应的free_list，将区块回收
    static void deallocate(void *p, size_t n) {
        obj *q = (obj *) p;
        obj *volatile *my_free_list;

        if (n > (size_t) __MAX_BYTES) {
            malloc_alloc::deallocte(p, n);
            return;
        }
        //寻找对应的free_list
        my_free_list = free_list + FREELIST_INDEX(n);
        //调整free_list,回收区块
        q->free_list_link = *my_free_list;
        *my_free_list = q;

    }

    static void *reallocate(void *p, size_t old_sz, size_t new_sz) {
        void *result;
        size_t copy_sz;

        // 如果old_size和new_size均大于__MAX_BYTES, 则直接调用realloc()
        // 因为这部分内存不是经过内存池分配的
        if (old_sz > (size_t) __MAX_BYTES && new_sz > (size_t) __MAX_BYTES) {
            return realloc(p, new_sz);
        }

        //如果ROUND_UP(old_sz) == ROUND_UP(new_sz), 内存大小没变化, 不进行重新分配
        if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) {
            return (p);
        }

        //进行重新分配并拷贝数据

        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz);
        return result;


    }

};


/**                             类型重命名，方便容器使用                        **/
//先不考虑多线程
typedef __default_alloc_template<false, 0> alloc;
typedef __default_alloc_template<false, 0> single_client_alloc;


//每次分配一大块内存，防止多次分配小内存带来的内存碎片。
//
template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &nobjs) {
    char *result;
    size_t total_bytes = size * nobjs;          //请求的空间
    size_t bytes_left = end_free - start_free;  //内存池中剩余的空间

    if (bytes_left >= total_bytes) {
        result = start_free;
        start_free += total_bytes;      //把内存分配出去后更新start_free
        return (result);
    } else if (bytes_left >= size) {        //内存池里可以分配一个或多个objs
        nobjs = bytes_left / size;          //会改变传入参数nobjs
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return (result);
    }
        //内存池剩余内存连一个节点也不够分配，需要从堆内存从重新开辟空间
    else {
        //新内存池的大小是请求空间的两倍,
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(
                heap_size >> 4);       //heap_size>>4是将"heap_size"的值右移四位，相当于除以16，这是为了将堆分配对齐到16字节边界。
        //将剩余的内存分配给指定的free_list[FREELIST_INDEX(bytes_left]
        if (bytes_left > 0) {
            obj *volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
            ((obj *) start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj *) start_free;
        }
        start_free = (char *) malloc(bytes_to_get);
        //分配失败，搜索原来已经分配的内存块，看是否有大于等于当前请求的内存块
        if (0 == start_free) {
            size_t i;
            obj *volatile *my_free_list, *p;
            for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                //找到一个，将其加到内存池中
                if (0 != p) {
                    *my_free_list = p->free_list_link;
                    start_free = (char *) p;
                    end_free = start_free + i;
                    //内存池更新完毕，重新分配内存池
                    return (chunk_alloc(size, nobjs));
                }
            }
            //再次失败，直接调用一级配置器分配，期待异常处理函数能提供帮助


            end_free = 0;
            start_free = (char *) malloc_alloc::allocate(bytes_to_get);
        }

        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        //内存池更新完毕，重新分配内存
        return (chunk_alloc(size, nobjs));
    }

}

// 返回一个大小为n的对象, 并且加入到free_list[FREELIST_INDEX(n)]
/*当它发现free list 中没有可用区块了时，就调用 refill（)，淮备为free sisr 重新填充空间。
 * 新的空问將取自内存池（经由chunk_a11oc (）完成〉。缺省取得20 个新节点（新区 块），但万一内存池空间不
 * 足，获得的节点数（区块数）可能小于 208 */
template<bool threads, int inst>
void *__default_alloc_template<threads, inst>::refill(size_t n) {
    int nobjs = 20;         //默认分配20个所请求的内存的节点，使用时只使用第一个节点，后序的节点连接到free_list对应的节点my_free_list下
    char *chunk = chunk_alloc(n, nobjs);    //在经过chunk_alloc后，nobjs可能会发生变化，如果分配的一大块内存小于20个n字节的内存时，nobjs会变成相应的个数
    obj *volatile *my_free_list;
    obj *result;
    obj *current_obj, *next_obj;
    int i;

    //如果内存池仅仅只够分配一个对象的空间，直接返回即可
    if (1 == nobjs) return (chunk);         //在chunk_alloc中内存不够分配20个nobjs会改变nobjs的值

    //内存池能分配更多空间,就调整free_list,纳入新节点
    my_free_list = free_list + FREELIST_INDEX(n);

    //在chunk的空间中建立free_list
    result = (obj *) chunk;
    //以下引导free_list指向新配置空间（取自内存池）
    *my_free_list = next_obj = (obj *) (chunk + n);
    //这段代码使用一个循环来将chunk_alloc函数分配的一块连续的大的内存空间划分为多个n字节的对象，并将它们链接在一起以形成一个空闲对象链表。用于后续的内存分配操作
    for (i = 1;; i++) {                     //从1开始，因为第零个要返回给客端
        current_obj = next_obj;             //变量current_obj表示当前正在处理的对象，而变量next_obj指向下一个对象的起始地址。
        next_obj = (obj *) ((char *) next_obj + n);
        if (nobjs - 1 == i) {               //当i等于nobjs -1时，表示已经处理了所有的对象，此时将当前对象的free_list_link设置为0，表示链表的末尾。
            current_obj->free_list_link = 0;
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }
    return (result);
}


//以下是静态数据成员的定义与初值的设定,初值都为0，且指针也全为空
template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0;

template<bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template<bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj *volatile __default_alloc_template<threads, inst>::free_list[__NFREELISTS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};

#endif //IMPLTMENT_STL_MEMORY_STL_ALLOC_H_
