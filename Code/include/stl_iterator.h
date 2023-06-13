//
// Created by 梁磊磊 on 2023/4/20.
//

/*stl_iterator.h 是 C++ 标准库中的一个头文件，它定义了一些通用的迭代器类和相关的辅助函数，
 * 用于支持对不同类型的容器进行迭代操作。这个头文件中定义的迭代器类和函数是 STL 中算法和容器实现的基础。*/

#ifndef IMPLTMENT_STL_STL_ITERATOR_H
#define IMPLTMENT_STL_STL_ITERATOR_H


#include "cstddef"
#include "type_traits.h"


namespace mystl {


    //五个作为标记用的型别,只作为标记用
    struct input_iterator_tag {
    };
    struct output_iterator_tag {
    };
    struct forward_iterator_tag : public input_iterator_tag {
    };
    struct bidirectional_iterator_tag : public forward_iterator_tag {
    };
    struct random_access_iterator_tag : public bidirectional_iterator_tag {
    };

    /*******************************************************************************************************************/
    //迭代器模版,只需传入两个类型
    template<class Category,
            class T,
            class Distance = ptrdiff_t,
            class Pointer = T *,
            class Reference = T &>
    struct iterator {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Distance difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
    };

//萃取出容器的迭代器相应的型别：必须对pointer类型和poniter-to-const设计特化版本
    template<class I>
    struct iterator_traits {
        typedef typename I::iterator_category iterator_category;
        typedef typename I::value_type value_type;
        typedef typename I::difference_type difference_type;
        typedef typename I::pointer pointer;
        typedef typename I::reference reference;
    };


//针对原生指针设计的traits偏特化版本
    template<class T>
    struct iterator_traits<T *> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T *pointer;
        typedef T &reference;
    };

//针对原生指针 pointer-to-const而设计的traits偏特化版
    template<class T>
    struct iterator_traits<const T *> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef const T *pointer;
        typedef const T &reference;
    };

/**这个函数可以很方便地萃取某个迭代器的类型*/
    template<class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator &) {
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();         //这个是struct random_access_iterator_tag 类的构造函数
    }


/**萃取某种迭代器**/
//模版元编程:检查给定类型T是否具有iterator_category成员类型。如果 `U` 具有 `iterator_category` 类型，则返回 `char`，否则返回 `two`。
    template<class T>
    struct has_iterator_cat {
    private:
        struct two {
            char a;
            char b;
        };

        template<class U>
        static two test(...);       //...接受任何参数

        template<class U>
        static char test(typename U::iterator_category * = 0);

    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    //该模版类用于检查给定类型T的迭代器类型是否可以转换为另一种类型U。
    template<class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of : public m_bool_constant<std::is_convertible<
            typename iterator_traits<T>::iterator_category, U>::value> {

    };


    template<class T, class U>
    struct has_iterator_cat_of<T, U, false> : public m_false_type {
    };

    template<class Iter>
    struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {
    };

    template<class Iter>
    struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {
    };

    template<class Iter>
    struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {
    };

    template<class Iter>
    struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {
    };

    template<class Iter>
    struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {
    };

    template<class Iterator>
    struct is_iterator : public m_bool_constant<is_input_iterator<Iterator>::value ||
                                                is_output_iterator<Iterator>::value> {
    };


//这个函数返回输入迭代器的distance type
    template<class Iterator>
    inline typename iterator_traits<Iterator>::difference_type *
    distance_type(const Iterator &) {
        //返回一个指向类型为difference_type的指针
        return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
    }


//这个函数可以决定某个迭代器的value type
//这个函数的实际作用是提供一个编译时的类型检查，可以用来确定迭代器的值类型。
    template<class Iterator>
    inline typename iterator_traits<Iterator>::value_type *
    value_type(const Iterator &) {
        return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
    }

/**以下的整组的distance函数*/
    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, random_access_iterator_tag) {
        return last - first;
    }


    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        typedef typename iterator_traits<InputIterator>::iterator_category category;
        return __distance(first, last, category());
    }

/**下列是整组的advance函数,表示迭代器移动n个位置*/
    template<class InputIterator, class Distance>
    inline void __advance(InputIterator &i, Distance n, input_iterator_tag) {
        while (n--) ++i;
    }

    template<class Bidirectional_iterator, class Distance>
    inline void __advance(Bidirectional_iterator &i, Distance n, bidirectional_iterator_tag) {
        if (n >= 0)
            while (n--) ++i;
        else
            //n!=0,就会一直执行
            while (n++) --i;
    }

    template<class Random_access_iterator, class Distance>
    inline void __advance(Random_access_iterator &i, Distance n, random_access_iterator_tag) {
        i += n;
    }

    template<class InputIterator, class Distance>
    inline void advance(InputIterator &i, Distance n) {

        __advance(i, n, iterator_category(i));
    }

/*******************************************************************************************************************/
//模版类：reverse_iterator
//代表反向迭代器，使前进为后退，后退为前进
/*******************************************************************************************************************/
    template<class Iterator>
    class reverse_iterator {
    private:
        Iterator current;       //记录对应的正向迭代器

    public:
        //反向迭代器的五种相应型别
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        typedef typename iterator_traits<Iterator>::value_type value_type;
        typedef typename iterator_traits<Iterator>::difference_type difference_type;
        typedef typename iterator_traits<Iterator>::reference reference;
        typedef typename iterator_traits<Iterator>::pointer pointer;

        typedef Iterator iterator_type;
        typedef reverse_iterator<Iterator> self;

    public:
        //构造函数
        reverse_iterator() {}

        explicit reverse_iterator(iterator_type i) : current(i) {}

        reverse_iterator(const self &rhs) : current(rhs.current) {}

    public:
        //取出对应的正向迭代器
        iterator_type base() const {
            return current;
        }

        //重载操作符
        reference operator*() const {
            auto tmp = current;
            return *--tmp;
        }

        pointer operator->() const {
            return &(operator*());
        }

        //前进变为后退
        self &operator++() {
            --current;
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            --current;
            return tmp;
        }

        //后退变前进
        self &operator--() {
            ++current;
            return *this;
        }

        self &operator--(int) {
            self tmp = *this;
            ++current;
            return tmp;
        }

        self &operator+=(difference_type n) {
            current -= n;
            return *this;
        }

        //operator+不改变本迭代器的current，而是先将当前迭代器的指针 current 向前移动 n 个位置，
        //得到新的指针位置。然后使用 self 类型的构造函数创建一个新的迭代器对象，并将新指针位置作为参数传入构造函数。最后，将新的迭代器对象返回。
        //函数后面根const，就意味着不会修改任何成员变量的值
        self operator+(difference_type n) const {
            return self(current - n);
        }

        self &operator-=(difference_type n) {
            current += n;
            return *this;
        }

        self operator-(difference_type n) const {
            return self(current + n);
        }

        reference operator[](difference_type n) const {
            return *(*this + n);
        }


    };

    //为逆向迭代器重载操作符
    template<class Iterator>
    typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        return rhs.base() - lhs.base();
    }

    template<class Iterator>
    bool operator==(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        return lhs.base() == rhs.base();
    }

    template<class Iterator>
    bool operator!=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        return lhs.base() != rhs.base();
    }

    //逆序的左小于右等价于正序的右小于左
    template<class Iterator>
    bool operator<(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        return rhs.base() < lhs.base();
    }

    template<class Iterator>
    bool operator>(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        return rhs < lhs;
    }

    template<class Iterator>
    bool operator<=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        return !(rhs < lhs);
    }

    template<class Iterator>
    bool operator>=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        return !(lhs < rhs);
    }


}       //namespace mystl


#endif //IMPLTMENT_STL_STL_ITERATOR_H
