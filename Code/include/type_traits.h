/*
 * @Descripttion:
 * @version: 1.0
 * @Author: liangleilei
 * @Date: 2023-03-27 08:00:11
 * @LastEditTime: 2023-03-27 12:41:36
 */

/**
 * 萃取器运行机理：
 */



#ifndef IMPLTMENT_STL_INCLUDE_TYPE_TRAITS_H_
#define IMPLTMENT_STL_INCLUDE_TYPE_TRAITS_H_

#include "type_traits"

namespace mystl {

    //模板元编程工具类
    template<class T, T v>
    struct m_integral_constant {
        static constexpr T value = v;
    };

    template<bool b>
    using m_bool_constant = m_integral_constant<bool, b>;

    typedef m_bool_constant<true> m_true_type;
    typedef m_bool_constant<false> m_false_type;


    // 命名规范，双横杠表示是STL内部所用的东西，不在STL标准规范之内
    struct __true_type {
    };
    struct __false_type {
    };


    template<typename T>
    struct __type_traits {
        typedef __true_type this_dummy_member_must_be_first;
        /*不要移除这个成员。它通知“有能力将__type_traits特化”的编译器说，我们现在所看到的这个__type_traits template是特殊的。
        这是为了确保万一编译器也使用一个名为__type_traits 而其实与此处定义并无任何关联的template时，这个类模板能正常工作*/

        /*为了安全起见，所有型别都默认为__false_type,再针对每一个标量型别进行适当的模版偏特化*/
        typedef __false_type has_trivial_default_constructor; // 是否无需考虑默认构造器
        typedef __false_type has_trivial_copy_constructor;    // 是否无需考虑拷贝构造器
        typedef __false_type has_trivial_assignment_operator; // 是否无需考虑赋值操作符
        typedef __false_type has_trivial_destructor;          // 是否无需考虑析构器，
        typedef __false_type is_POD_type;                     // 是否是POD类型
    };

// 对内置类型进行模版偏特化,内置类型是POD类型
/*c++的基本类型包括包括 bool、char、signed char、unsigned char、short、unsigned short、
int、unsigned int、long、unsigned long、long long 和 unsigned long long。*/
/*浮点型（floating-point types）：包括 float、double 和 long double。*/
/*指针类型*/

    template<>
    struct __type_traits<bool> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<char> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<signed char> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<unsigned char> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<short> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<unsigned short> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<int> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<unsigned int> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<long> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<unsigned long> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<long long> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<unsigned long long> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<float> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<double> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

    template<>
    struct __type_traits<long double> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };

// 对于原生指针类型进行偏特化
    template<typename T>
    struct __type_traits<T *> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operator;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
}


#endif //IMPLTMENT_STL_INCLUDE_TYPE_TRAITS_H_