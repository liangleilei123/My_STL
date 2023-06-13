//
// Created by 梁磊磊 on 2023/5/11.
//

#ifndef IMPlTMENT_STl_MY_lIST1_H
#define IMPlTMENT_STl_MY_lIST1_H
//双向链表

#include <iostream>
#include "cstddef"
#include "stl_iterator.h"
#include "stl_alloc.h"
#include "stl_constructor.h"
#include "exceptdef.h"

namespace mystl {
/**********************************************************************************************************************/
//先设计链表结点
/**********************************************************************************************************************/

    template<class T>
    struct _list_node {

        _list_node<T> *prev;
        _list_node<T> *next;
        T data;
    };
/**********************************************************************************************************************/
//再设计迭代器
/**********************************************************************************************************************/
    template<class T, class Ref, class Ptr>
    struct _list_iterator {
        typedef _list_iterator<T, T &, T *> iterator;
        typedef _list_iterator<T, const T &, const T *> const_iterator;
        typedef _list_iterator<T, Ref, Ptr> self;

        typedef T value_type;
        typedef Ref reference;
        typedef Ptr pointer;
        typedef bidirectional_iterator_tag iterator_category;
        typedef _list_node<T> *link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        link_type node;

        //构造函数
        _list_iterator() {}

        _list_iterator(link_type x) : node(x) {}

        _list_iterator(const iterator &x) : node(x.node) {}

        bool operator==(const self &x) const { return x.node == node; }

        bool operator!=(const self &x) const { return x.node != node; }

        reference operator*() const {
            MYSTL_DEBUG(node != nullptr);
            return node->data;
        }      //异常处理

        pointer operator->() const { return &(operator*()); }

        self &operator++() {
            node = node->next;
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self &operator--() {
            node = node->prev;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }


    };

/**********************************************************************************************************************/
//设计list容器
/**********************************************************************************************************************/
    template<class T, class Alloc=alloc>
    class list {
    protected:
        typedef _list_node<T> list_node;
        //空间分配器分配的是以节点为单位分配空间的
        typedef simple_alloc<list_node, Alloc> list_node_allocate;

    public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef list_node *link_type;               //节点指针类型
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        //迭代器
        typedef _list_iterator<T, T &, T *> iterator;
        typedef _list_iterator<T, const T &, const T *> const_iterator;

    protected:
        //只要一个指针，便可表示整个环状双向链表.刻意让node指向置于尾端的空白节点，node便能符合stl对于前闭后开区间的要求，成为last迭代器
        //node为last，node->next为头节点
        link_type node;

    protected:
        //配置一个节点并返回
        link_type get_note() {
            return list_node_allocate::allocate();
        }

        void put_node(link_type p) {
            list_node_allocate::deallocate(p);
        }

        link_type create_node(const_reference x) {
            link_type p = get_note();
            construct(&p->data, x);
            p->next = nullptr;
            p->prev = nullptr;
        }

        void destroy_node(link_type p) {
            destroy(&p->data);
            put_node(p);
        }


        void empty_initialize() {
            node = get_note();
            node->next = node;
            node->prev = node;
        }

        //list的迁移操作。将[fitst,last)内的所有元素移动到position之前
        //先把[first,last)从原来的链表中断开，在插入到position之前
        void transfer(const_iterator position, iterator first, iterator last) {
            if (position != last) {                     //position==last,就不用移动了
                last.node->prev->next = position.node;
                first.node->prev->next = last.node;
                position.node->prev->next = first.node;
                link_type tmp = position.node->prev;
                position.node->prev = last.node->prev;
                last.node->prev = first.node->prev;
                first.node->prev = tmp;

            }
        }

    public:

        //构造函数
        //默认构造，产生一个链表
        list() { empty_initialize(); }


        //迭代器操作
        iterator begin() const { return node->next; }

        iterator end() const { return node; }

        const_iterator cbegin() const { return begin(); }

        const_iterator cend() const { return end(); }

        bool empty() const { return node->next == node; }

        size_type size() const {
            size_type result = 0;
            result = static_cast<size_type>(distance(begin(), end()));
            return result;
        }

        reference front() { return *begin(); }

        reference back() { return *(--end()); }

        //节点的插入和删除操作
        //插入position之前
        iterator insert(iterator position, const_reference x) {
            link_type tmp = create_node(x);
            tmp->prev = position.node->prev;
            tmp->next = position.node;
            (link_type(position.node->prev))->next = tmp;
            position.node->prev = tmp;
            return tmp;     //tmp是一个link_type类型，返回类型是iterator类型，这里调用了_list_iterator<>的有参构造_list_iterator(link_type x) : node(x) {}

        }

        void push_back(const_reference x) { insert(end(), x); }

        void push_front(const_reference x) { insert(begin(), x); }

        //移除迭代器position所指的节点
        iterator erase(iterator position) {
            link_type next_node = link_type(position.node->next);
            link_type prev_node = link_type(position.node->prev);

            next_node->prev = prev_node;
            prev_node->next = next_node;
            destroy_node(position.node);
            return iterator(next_node);
        }

        //移除头节点
        void pop_front() { erase(begin()); }

        //移除尾节点
        void pop_back() {
            iterator tmp = end();
            erase(--tmp);
        }

        //清除所有节点（链表）
        void clear();

        //将数值为value之所有元素移除
        void remove(const_reference value);

        //移除数值相同的连续元素，
        void unique();

        //接合操作
        //将x接合于position所指位置之前。x必须不同于*this
        void splice(const_iterator position, list &x) {
            if (!x.empty()) {
                transfer(position, x.begin(), x.end());
            }
        }

        //将i所指元素接合于position所指位置之前。position和i可指向同一个list
        void splice(const_iterator position, list &, iterator i) {
            iterator j = i;
            ++j;
            if (position == i || position == j) return;        //自己没法插到自己前面
            transfer(position, i, j);

        }

        void splice(const_iterator position, list &, iterator first, iterator last) {
            if (first != last) {
                transfer(position, first, last);
            }
        }

        //this的内容和x的内容交换:只要交换哨兵节点的前后节点
        void swap(list &x) {
            link_type tmp = node;
            node = x.node;
            x.node = tmp;
        }

        void merge(list &);       //通常用于两个有序链表，merge()将x合并到*this上，两个lists的内容默认经过递增排序
        //可以设计merge()的排序接收lambda表达式
        template<class Compare>
        void merge(list &, Compare);
        //c++11加入左值引用

        void reverse();

        void sort();


    };

    //从头节点开始依次移除
    template<class T, class Alloc>
    void list<T, Alloc>::clear() {
        link_type cur = (link_type) node->next;
        while (cur != node) {
            link_type tmp = cur;
            cur = (link_type) cur->next;
            destroy_node(tmp);          //因为要全部删除节点，不需要链接前后节点，所以用destroy_node就行，但remove不行
        }
        node->next = node;
        node->prev = node;
    }

    template<class T, class Alloc>
    void list<T, Alloc>::remove(const_reference value) {
        iterator first = begin();
        iterator last = end();
        while (first != last) {
            iterator next = first;    //记录要删除节点的下一个节点。迭代器只能加加减减，不能用->next移动
            ++next;
            if (*first == value) erase(first);
            first = next;
        }
    }

    template<class T, class Alloc>
    void list<T, Alloc>::unique() {
        iterator first = begin();
        iterator last = end();
        if (first == last) return;
        iterator next = first;
        while (++next != last) {

            if (*next == *first) {
                erase(next);
            } else {
                first = next;
            }
            next = first;   //修正区段范围
        }
    }

    //默认是递增排序
    template<class T, class Alloc>
    void list<T, Alloc>::merge(list<T, Alloc> &x) {
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();
        while (first1 != last1 && first2 != last2) {
            if (*first2 < *first1) {
                auto next = first2;
                ++next;
                for (; next != last2 && *next < *first1; ++next);
                transfer(first1, first2, next);
                first2 = next;
            } else {
                ++first1;
            }
        }
        if (first2 != last2) {
            transfer(last1, first2, last2);
        }
    }

    //将*this的内容逆向重置:头插法，不是很高效。
    //试试交换节点前驱和后继的方法
    template<class T, class Alloc>
    void list<T, Alloc>::reverse() {
        //如果是空链表，或仅有一个元素，就不进行任何操作
        if (node->next == node || link_type(node->next)->next == node) {
            return;
        }
        iterator first = begin();
        ++first;
        while (first != end()) {
            iterator old = first;
            ++first;
            transfer(begin(), old, first);
        }
    }


    //默认从小到大排序，且用的是快速排序,确实有点像归并排序
    //归并排序，部分排序，然后按照顺序把部分有序合并，时间复杂度为nlogn
    template<class T, class Alloc>
    void list<T, Alloc>::sort() {
        //链表为空或只有一个节点，就不需要排序
        if (node->next == node || link_type(node->next)->next == node) return;

        list<T, Alloc> carry;
        list<T, Alloc> counter[64];          //list类型的数组
        int fill = 0;
        while (!empty()) {
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty()) {
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill) ++fill;
        }
        for (int i = 1; i < fill; ++i) {
            counter[i].merge(counter[i - 1]);
        }
        swap(counter[fill - 1]);
    };


}

#endif //IMPlTMENT_STl_MY_lIST1_H
