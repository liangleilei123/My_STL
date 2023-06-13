//
// Created by 梁磊磊 on 2023/4/2.
//

#ifndef IMPLTMENT_STL_MEMORY_STL_UNINITIALIZED_H_
#define IMPLTMENT_STL_MEMORY_STL_UNINITIALIZED_H_

/**这里定义了一些全局函数，作用域未初始化空间上，用来填充(fill)或复制(copy)大块内存数据**/

#include "type_traits.h"
#include "stl_constructor.h"
#include "cstring"
#include "stl_algobase.h"
#include "stl_iterator.h"

/**如果作为输出目的地的[result,result+(last-first)）范围内的每一个迭代器都指向未初始化的区域
 * 则uninitialized_copy()会使用copy constructor,给身为输入来源的[first,last)范围（注意是左闭右开）内的每个对象产生一份复制品
 * 放进输出范围中
 * */
namespace mystl {
    template<class InputIterator, class ForwardIterator>
    ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
        return copy(first, last, result);             //在头文件<algorithm>中
    }

    template<class InputIterator, class ForwardIterator>
    ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
        ForwardIterator cur = result;

        //这里有异常处理

        for (; first != last; ++first, ++cur) {
            construct(&*cur, *first);
        }
        return cur;         //返回的复制完的结束位置
    }

    template<class InputIterator, class ForwardIterator, class T>
    ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T *) {
        typedef typename __type_traits<T>::is_POD_type is_POD;
        return __uninitialized_copy_aux(first, last, result, is_POD());
    }


    template<class InputIterator, class ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
        return __uninitialized_copy(first, last, result, value_type(result));           //value_type返回的是迭代器的值类型
        //type_traits可以萃取原生指针的类型
    };


//针对char* 和wchar_t*两种型别，可以采用最有效率的做法memove(直接移动内存内容)来执行复制行为
//char为1个字节，wchar_t采用双字节编码，通常为两个字节，


    inline char *uninitialized_copy(const char *first, const char *last, char *result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

/******************************************* uninitialized_fill ***************************************************/





    template<class ForwardIterator, typename T>
    void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __false_type) {
        ForwardIterator cur = first;
        //这里有异常处理

        for (; cur != last; ++cur) {
            construct(&*cur, x);
        }

    }

    template<class ForwardIterator, typename T>
    void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __true_type) {
        fill(first, last, x);             //在头文件<algorithm>中
    }

    template<class ForwardIterator, typename T, typename T1>
    void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x, T1 *) {
        typedef typename __type_traits<T1>::is_POD_type is_POD;
        __uninitialized_fill_aux(first, last, x, is_POD());
    }

    template<class ForwardIterator, typename T>
    void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x) {
        __uninitialized_fill(first, last, x, value_type(first));
    }

/******************************************* uninitialized_fill_n ***************************************************/

    template<class ForwardIterator, typename Size, typename T>
    ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type) {
        ForwardIterator cur = first;
        //这里有异常处理
        for (; n > 0; --n, ++cur) {
            construct(&*cur, x);            //在头文件stl_constructor.h中，construct的参数是指针，cur是迭代器，&*的操作是先取迭代器指向对象，再把地址传递给指针
        }
        return cur;
    }

    template<class ForwardIterator, typename Size, typename T>
    ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type) {
        return fill_n(first, n, x);             //在头文件<algorithm>中
    }


    template<class ForwardIterator, typename Size, typename T, typename T1>
    ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T &x, T1 *) {
        typedef typename __type_traits<T1>::is_POD_type is_POD;
        return __uninitialized_fill_n_aux(first, n, x, is_POD());          //传入的是类的构造函数
    }


    template<class ForwardIterator, typename Size, typename T>
    ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x) {
        return __uninitialized_fill_n(first, n, x, value_type(first));          //这个value_type用来萃取迭代器的类型

    }


}


#endif //IMPLTMENT_STL_MEMORY_STL_UNINITIALIZED_H_
