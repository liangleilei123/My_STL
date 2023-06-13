//
// Created by 梁磊磊 on 2023/5/10.
//

#ifndef IMPLTMENT_STL_FUNCTIONAL_H
#define IMPLTMENT_STL_FUNCTIONAL_H

//这个头文件包含了mystl的函数对象与哈希函数
//函数对象可以看作是一个重载了operator()运算符的类对象，它可以封装一个函数或者一个函数调用表达式，并在需要时被调用执行。
#include "cstddef"

namespace mystl {
    //定义二元函数的参数型别和返回值型别
    template<class Arg1, class Arg2, class Result>
    struct binary_function {
        typedef Arg1 first_argument_type;
        typedef Arg2 second_argumen_type;
        typedef Result result_type;
    };


    template<class T>
    struct equal_to : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x == y; }
    };

    template<class T>
    struct less : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x < y; }
    };

    /**********************************************************************************************************************/
//identity()
/**********************************************************************************************************************/
    template<class Arg, class Result>
    struct unary_function {
        typedef Arg argument_type;
        typedef Result result_type;
    };

    template<class T>
    struct identity : public unary_function<T, T> {
        const T &operator()(const T &x) const {
            return x;
        }
    };


    template<class _Pair>
    struct _Select1st : public unary_function<_Pair, typename _Pair::first_type> {
        const typename _Pair::first_type &operator()(const _Pair &_X) const {
            return (_X.first);
        }
    };

    template<class _Pair>
    struct select1st : public _Select1st<_Pair> {
    };


}


#endif //IMPLTMENT_STL_FUNCTIONAL_H
