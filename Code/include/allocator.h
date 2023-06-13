//
// Created by 梁磊磊 on 2023/3/30.
//

#ifndef IMPLTMENT_STL_MEMORY_ALLOCATOR_H_
#define IMPLTMENT_STL_MEMORY_ALLOCATOR_H_

//只是对::operator new 和::operator delete 进行简单的封装，效率不高
#include <new>
#include <cstdlib>      //exit()
#include <iostream>
#include "unistd.h"     //getpagesize()


//修改size的类型为size_t，能够管理（目前只有分配和回收）更大的内存，int类型相对来说太小了。

//定义简单的空间分配和析构模版函数
template<typename T>
inline T *allocate(size_t size, T *) {
    std::set_new_handler(0);                            //错误处理函数，参数为零，分配失败直接返回
    T *tmp = (T *) ::operator new(size * sizeof(T));
    if (tmp == 0) {
        std::cerr << "Failed to allocate memory" << std::endl;      //cerr用于向标准错误输出流（通常是控制台）输出错误信息
        exit(1);
    }
    return tmp;
}

template<typename T>
inline void deallocate(T *buffer) {

    ::operator delete(buffer);

}

template<typename T>
class allocator {
public:

    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

public:

    static T *allocate(size_type n) {             //定义为类内静态成员函数，可以直接通过类名调用
        return ::allocate((difference_type) n, (pointer) 0);      //(T*)0是一个空指针，用于指定数组中元素的类型
    }

    static void deallocate(pointer buffer) {
        ::deallocate(buffer);
    }

    //返回地址：在C++中，可以使用指针来返回一个地址。一个函数可以返回一个指向某个变量或对象的指针，这个指针可以用于访问该变量或对象。
    pointer address(reference x) {
        return (pointer) &x;
    }

    const_pointer address(const_reference x) {
        return (const_pointer) &x;
    }

    //一个页面可以存放多少T类型对象
    //页面大小是操作系统中定义的一个常量，通常在编译时就已经确定。不同的操作系统和体系结构可能有不同的页面大小，但通常为4KB或者4MB。
    //在大多数现代的操作系统中，包括Windows、Linux、macOS等，页面大小通常为4KB（4096字节）。这意味着操作系统将内存分成了大小为4KB的块，每个块被称为一个页面。
    //但经过测试后，我这台机子的分页为16kb。即16384
    size_type init_page_size() {
        return std::max(size_type(1), size_type((size_type) getpagesize() / sizeof(T)));
    }

    //能存放的T类型个数
    size_type max_size() const {
        return std::max(size_type(1), size_type(UINT64_MAX) / sizeof(T));
    }

};

#endif //IMPLTMENT_STL_MEMORY_ALLOCATOR_H_
