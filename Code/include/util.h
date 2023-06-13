//
// Created by 梁磊磊 on 2023/5/3.
//

#ifndef IMPLTMENT_STL_UTIL_H
#define IMPLTMENT_STL_UTIL_H

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等

#include "cstddef"
#include "type_traits"

namespace mystl {
/**********************************************************************************************************************/
//move()
/**********************************************************************************************************************/
    template<class T>
    typename std::remove_reference<T>::type &&move(T &&arg) noexcept {
        return static_cast<typename std::remove_reference<T>::type &&>(arg);
    }
/**********************************************************************************************************************/
//forward,参数的完美转发
/**********************************************************************************************************************/
    template<class T>
    T &&forward(typename std::remove_reference<T>::type &arg) noexcept {
        return static_cast<T &&>(arg);
    }

    template<class T>
    T &&forward(typename std::remove_reference<T>::type &&arg) noexcept {
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");      //如果参数是左值引用，则报错
        return static_cast<T &&>(arg);
    }

/**********************************************************************************************************************/
//swap()
/**********************************************************************************************************************/
    template<class Tp>
    void swap(Tp &lhs, Tp &rhs) {
        auto tmp(mystl::move(lhs));
        lhs = mystl::move(rhs);
        rhs = mystl::move(tmp);
    }

    template<class ForwardIter1, class ForwardIter2>
    ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) {
        for (; first1 != last1; ++first1, (void) ++first2) {
            mystl::swap(*first1, *first2);
        }
        return first2;
    }

    template<class Tp, size_t N>
    void swap(Tp (&a)[N], Tp (&b)[N]) {
        mystl::swap_range(a, a + N, b);
    }


}


#endif //IMPLTMENT_STL_UTIL_H
