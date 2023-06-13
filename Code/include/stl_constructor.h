//
// Created by 梁磊磊 on 2023/3/30.
//

#ifndef IMPLTMENT_STL_MEMORY_STL_CONSTRUCTOR_H_
#define IMPLTMENT_STL_MEMORY_STL_CONSTRUCTOR_H_

#include "new"
#include "type_traits.h"
#include "util.h"
#include "stl_iterator.h"

//定义了全局函数，construct() 和destroy()，负责对象的构造和析构

namespace mystl {

    /**      construct      **/

    template<typename T1, typename T2>
    void construct(T1 *p, const T2 &value) {       //参数不发生改变，用const修饰,value没有参数会自动调用默认构造
        new(p) T1(value);        //placement new ,在指针P构造T类型数据，如果Value_type类型也是T类型的，调用拷贝构造
        //如果Value_type不是T类型的，调用T的有参构造
    }

    template<typename T>
    void construct(T *p) {
        new(p) T();             //调用默认构造
    }

    template<class Ty, class... Args>
    void construct(Ty *ptr, Args &&... args) {
        new(ptr) Ty(mystl::forward<Args>(args)...);
    }

    /**         destroy             **/

    //第一个版本，接收一个指针
    template<typename T>
    void destroy(T *p) {
        //std::cout << "调用对象析构函数" << std::endl;
        p->~T();
    }

    template<typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, mystl::__true_type) {
        //has_trivial_destructor，说明用户不定义析构函数，用默认的析构函数，destroy不需要执行操作
    }

    template<typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, size_t n, mystl::__true_type) {
        //has_trivial_destructor，说明用户不定义析构函数，用默认的析构函数，destroy不需要执行操作
    }

    template<typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, mystl::__false_type) {
        for (; first != last; ++first) {
            destroy(&*first);
            //destroy()接受的是指针类型，first是迭代器类型，先用*取迭代器指的对象，再用&传对象的地址
        }
    }

    template<typename ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, size_t n, mystl::__false_type) {
        for (size_t i = 0; i < n; ++first, ++i) {
            destroy(&*first);
            //destroy()接受的是指针类型，first是迭代器类型，先用*取迭代器指的对象，再用&传对象的地址
        }
    }

    //用户不指定类型，内部自动推导
    template<typename ForwardIterator, class T>
    inline void __destroy(ForwardIterator first, ForwardIterator last, T *) {
        typename mystl::__type_traits<T>::has_trivial_destructor trivial_destructor;
        __destroy_aux(first, last, trivial_destructor);
    }

    template<typename ForwardIterator, class T>
    inline void __destroy(ForwardIterator first, size_t n, T *) {
        typename mystl::__type_traits<T>::has_trivial_destructor trivial_destructor;
        __destroy_aux(first, n, trivial_destructor);
    }


//destroy的第二个版本，接受两个迭代器，此函数设法找出元素的数值型别
    template<typename ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last) {
        __destroy(first, last, value_type(first));                      //value_type应该是萃取迭代器类型
    }

    //destroy的第三个版本，接受一个迭代器和一个size_t类型数值n，析构从这个迭代器开始的n个对象
    template<typename ForwardIterator>
    inline void destroy(ForwardIterator first, size_t n) {
        __destroy(first, n, value_type(first));                      //value_type应该是萃取迭代器类型
    }

    //接受的迭代器为内置类型的指针时，destroy不需要进行任何操作，为了优化程序性能，针对内置类型进行特化
    inline void destroy(int *, int *) {}

    inline void destroy(char *, char *) {}

    inline void destroy(long *, long *) {}

    inline void destroy(float *, float *) {}

    inline void destroy(double *, double *) {}

#ifdef __STL_HAS_WCHAR_T
    inline void destory(wchar_t*,wchar_t*);
#endif
}


#endif //IMPLTMENT_STL_MEMORY_STL_CONSTRUCTOR_H_
