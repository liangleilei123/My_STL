//
// Created by 梁磊磊 on 2023/4/15.
//

#ifndef IMPLTMENT_STL_STL_ALGOBASE_H
#define IMPLTMENT_STL_STL_ALGOBASE_H


#include <cstring>
#include "type_traits.h"
#include "stl_iterator.h"
#include "util.h"

namespace mystl {

/*****************************************************************************************/
// min
// 取二者中的较小值，语义相等时保证返回第一个参数
/*****************************************************************************************/
    template<class T>
    inline const T &min(const T &lhs, const T &rhs) {
        return rhs < lhs ? rhs : lhs;
    }

    template<class T, class Compare>
    inline const T &min(const T &lhs, const T &rhs, Compare comp) {
        return comp(rhs, lhs) ? rhs : lhs;
    }

/*****************************************************************************************/
// max
// 取二者中的较大值，语义相等时保证返回第一个参数
/*****************************************************************************************/
    template<class T>
    inline const T &max(const T &lhs, const T &rhs) {
        return lhs < rhs ? rhs : lhs;
    }

    template<class T, class Compare>
    inline const T &max(const T &lhs, const T &rhs, Compare comp) {
        return comp(lhs, rhs) ? rhs : lhs;
    }



    /********************************* fill_n *******************************************/
//将[first,last]内的前n个元素改填新值，返回的迭代器指向被填入的最后一个元素的下一个位置
    template<class OutputIterator, typename Size, typename T>
    OutputIterator fill_n(OutputIterator first, Size n, const T &value) {
        for (; n > 0; ++first, --n) {
            *first = value;
        }
        return first;
    }


/******************************** fill *******************************************/
//将[first,last]内所有元素改填新值
    template<class ForwardIterator, typename T>
    void fill(ForwardIterator first, ForwardIterator last, const T &value) {
        for (; first != last; ++first) {
            *first = value;
        }
    }



/******************************* copy *********************************************/
//根据迭代器的种类不同，调用不同的__copy(),不同版本的迭代器所使用的循环条件不同，有快慢之分

//InputIterator 版本
    template<typename InputIterator, typename OutputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
        //以迭代器等同与否，决定循环是否继续。速度慢
        for (; first != last; ++result, ++first) {
            *result = *first;               //赋值运算符
        }
        return result;
    }

/**RandomAccessIterator是C++标准库中的一种迭代器类型，它具有随机访问元素的能力。与其他类型的迭代器相比，RandomAccessIterator提供了最全面和最灵活的访问能力。

 * RandomAccessIterator可以像指针一样进行算术运算，包括加减和大小比较。此外，它还可以使用[]运算符来访问特定索引处的元素，以及使用+=和-=运算符来实现迭代器的移动。

 * RandomAccessIterator通常用于需要随机访问序列元素的算法，例如排序、二分查找和快速查找。标准容器如std::vector和std::array的迭代器都是RandomAccessIterator类型。
 * */

//RandomAccessIterator 版本
    template<typename RandomAccessIterator, typename OutputIterator>
    inline OutputIterator
    __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result,
           mystl::random_access_iterator_tag) {
        return __copy_d(first, last, result, distance_type(first));     //distance_type是什么
    }


    template<typename RandomAccessIterator, typename OutputIterator, typename Distance>
    inline OutputIterator
    __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance *) {
        //以n决定循环的次数，速度快
        for (Distance n = last - first; n > 0; --n, ++result, ++first) {
            *result = *first;
        }
        return result;
    }

//原始类型有有trivial assignment operator，可以通过位拷贝的方式进行拷贝和赋值

    template<class T>
    inline T *__copy_t(const T *first, const T *last, T *result, __true_type) {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }


    template<class T>
    inline T *__copy_t(const T *first, const T *last, T *result, __false_type) {
        return __copy_d(first, last, result, (std::ptrdiff_t) 0);
    }


//完全泛化版本
//__copy_dispatch函数模版的泛化版本和特化版本
    template<class InputIterator, class OutputIterator>
    struct __copy_dispatch {
        OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
            return __copy(first, last, result, iterator_category(first));          //iterator_category
        }
    };

//偏特化版本，两个参数都是T*指针形式
    template<typename T>
    struct __copy_dispatch<T *, T *> {
        T *operator()(T *first, T *last, T *result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;           //是否无需考虑赋值运算符
            return __copy_t(first, last, result, t());
        }
    };


//偏特化版本，一个参数是const T*指针类型，一个是T*类型
    template<typename T>
    struct __copy_dispatch<const T *, T *> {
        T *operator()(const T *first, const T *last, T *result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;           //是否无需考虑赋值运算符
            return __copy_t(first, last, result, t());
        }
    };


    template<class InputIterator, class OutputIterator>
    inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);           //泛化
    }

//针对char和wchar_t的特化版本
    inline char *copy(const char *first, const char *last, char *result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t *copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
        memmove(result, first, sizeof(last - first));
        return result + (last - first);
    }

/**********************************************************************************************************************/
//copy_backward()
//将[first,last)区间内的元素拷贝到[result-(last-first),result)内
/**********************************************************************************************************************/
//双向指针
    template<class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, mystl::bidirectional_iterator_tag) {
        while (last != first) {
            *--result = *--last;
        }
        return result;
    }

//随机存取指针
    template<class RandomIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward_cat(RandomIter1 first, RandomIter1 last,
                                BidirectionalIter2 result, mystl::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n) {
            *--result = *--last;
        }
        return result;
    }

    template<class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        return unchecked_copy_backward_cat(first, last, result, iterator_category(first));        //萃取迭代器类型
    }


//对于 trivially_copy_assignable类型的偏特化
    template<class Tp, class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&         //检查去掉const修饰符后的Tp类型是否与Up相同。
            std::is_trivially_copy_assignable<Up>::value,                           //检查Up是否是平凡复制可分配的。
            Up *>::type      //如果两个条件都满足，则该表达式的类型为Up*。否则，由于SFINAE的原因，该表达式没有类型，编译器将尝试实例化另一个可能匹配参数的函数或类模板。
    unchecked_copy_backward(Tp *first, Tp *last, Up *result) {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) {
            result -= n;
            std::memmove(result, first, n);
        }
        return result;
    }

    template<class Bidirectional1, class Bidirectional2>
    Bidirectional2 copy_backward(Bidirectional1 first, Bidirectional1 last, Bidirectional2 result) {
        return unchecked_copy_backward(first, last, result);
    }



/**********************************************************************************************************************/
//move()
//将[first,last)区间内的元素移动到[result,result+(last - first))内
/**********************************************************************************************************************/
//迭代器是双向迭代器类型
    template<class InputIter, class OutputIter>
    OutputIter
    unchecked_move_cat(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag) {
        for (; first != last; ++first, ++result) {
            *result = mystl::move(
                    *first);              //将一个左值*first类型转换为右值引用，并将其复制给*result.这意味着 *first 的资源会被移动到 *result，而不是被复制。

        }
        return result;

    }


//迭代器是随机存取类型的
    template<class RandomIter, class OutputIter>
    OutputIter
    unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n, ++first, ++result) {
            *result = mystl::move(*first);
        }
        return result;
    }


    template<class InputIter, class OutputIter>
    OutputIter
    unchecked_move(InputIter first, InputIter last, OutputIter result) {
        return unchecked_move_cat(first, last, result, iterator_category(first));
    }


//为trivially_copy_assignable 类型提供特化版本
    template<class Tp, class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
            std::is_trivially_copy_assignable<Up>::value,
            Up *>::type
    unchecked_move(Tp *first, Tp *last, Up *result) {
        const size_t n = static_cast<size_t>(last - first);
        if (n != 0) {
            std::memmove(result, first, n);
        }
        return result + n;        //返回移动后的结尾位置
    }

    template<class InputIter, class OutputIter>
    OutputIter move(InputIter first, InputIter last, OutputIter result) {
        return unchecked_move(first, last, result);
    }

    /*****************************************************************************************/
    // lexicographical_compare
    // 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
    // (1)如果第一序列的元素较小，返回 true ，否则返回 false
    // (2)如果到达 last1 而尚未到达 last2 返回 true
    // (3)如果到达 last2 而尚未到达 last1 返回 false
    // (4)如果同时到达 last1 和 last2 返回 false
    /*****************************************************************************************/

    //默认的比较方式的序列1小于序列2
    template<class InputIter1, class InputIter2>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                 InputIter2 first2, InputIter2 last2) {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (*first1 < *first2)
                return true;
            if (*first2 < *first1)
                return false;
        }
        return first1 == last1 && first2 != last2;
    }

    template<class InputIter1, class InputIter2, class Compare>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                 InputIter2 first2, InputIter2 last2, Compare comp) {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (comp(*first1, *first2)) {
                return true;
            }
            if (comp(*first2, *first1)) {
                return false;
            }
        }
        return first1 == last1 && first2 != last2;
    }

    bool lexicographical_compare(const unsigned char *first1,
                                 const unsigned char *last1,
                                 const unsigned char *first2,
                                 const unsigned char *last2) {
        const auto len1 = last1 - first1;
        const auto len2 = last2 - first2;
        const auto result = std::memcmp(first1, first2, mystl::min(len1, len2));
        return result != 0 ? result < 0 : len1 < len2;
    }

    /*****************************************************************************************/
    // equal
    // 比较两个容器是否相等
    /*****************************************************************************************/
    template<class InputIter1, class InputIter2>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
        for (; first1 != last1; ++first1, ++first2) {
            if (*first1 != *first2)
                return false;
        }
        return true;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class InputIter1, class InputIter2, class Compared>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp) {
        for (; first1 != last1; ++first1, ++first2) {
            if (!comp(*first1, *first2))
                return false;
        }
        return true;
    }

    /*****************************************************************************************/
    // lower_nouder
    // 找到第一个不小于val的值，返回其迭代器，查找方法相等于二分查找，且是左闭右开区间
    /*****************************************************************************************/
    template<class _ForwardIter, class _Tp, class _Distance>
    _ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                               const _Tp &__val, _Distance *) {
        _Distance __len = 0;
        __len = distance(__first, __last);
        _Distance __half;
        _ForwardIter __middle;

        while (__len > 0) {
            __half = __len >> 1;
            __middle = __first;
            advance(__middle, __half);      //找到中间位置
            if (*__middle < __val) {        //如果中间值小于所求值，继续向后查找
                __first = __middle;
                ++__first;
                __len = __len - __half - 1;
            } else                        //否则，把中间位置当作结尾
                __len = __half;
        }
        return __first;
    }

    template<class _ForwardIter, class _Tp>
    inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
                                    const _Tp &__val) {

        return __lower_bound(__first, __last, __val,
                             distance_type(__first));
    }

    //由客户端设计比较方法
    template<class _ForwardIter, class _Tp, class _Compare, class _Distance>
    _ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                               const _Tp &__val, _Compare __comp, _Distance *) {
        _Distance __len = 0;
        __len = distance(__first, __last);
        _Distance __half;
        _ForwardIter __middle;

        while (__len > 0) {
            __half = __len >> 1;
            __middle = __first;
            advance(__middle, __half);
            if (__comp(*__middle, __val)) {
                __first = __middle;
                ++__first;
                __len = __len - __half - 1;
            } else
                __len = __half;
        }
        return __first;
    }

    template<class _ForwardIter, class _Tp, class _Compare>
    inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
                                    const _Tp &__val, _Compare __comp) {

        return __lower_bound(__first, __last, __val, __comp,
                             distance_type(__first));
    }


}   //namespace mystl


#endif //IMPLTMENT_STL_STL_ALGOBASE_H
