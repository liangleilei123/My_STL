//
// Created by 梁磊磊 on 2023/5/24.
//
//单向链表

#ifndef IMPLTMENT_STL_MY_SLIST_H
#define IMPLTMENT_STL_MY_SLIST_H


#include <cstdio>
#include <cstddef>
#include "stl_iterator.h"
#include "stl_alloc.h"
#include "stl_constructor.h"
#include "stl_algobase.h"

namespace mystl {

    //slist的节点:基类定义指针，子类定义数据域
    //基类不是模版类，子类是模版类
    struct __slist_node_base {
        __slist_node_base *next;
    };
    template<class T>
    struct __slist_node : public __slist_node_base {
        T data;
    };


    ///全局函数：已知某一个节点，插入新节点于其后
    inline __slist_node_base *__slist_make_link(
            __slist_node_base *prev_node,
            __slist_node_base *new_node) {
        new_node->next = prev_node->next;
        prev_node->next = new_node;
        return new_node;
    }

    ///全局函数：单向链表的大小（元素个数）
    inline size_t __slist_size(__slist_node_base *node) {
        size_t result = 0;
        for (; node != 0; node = node->next) {
            ++result;
        }
        return result;
    }

    ///全局函数，找到node的前一个节点
    inline __slist_node_base *__slist_previous(__slist_node_base *head, const __slist_node_base *node) {
        while (head && head->next != node) {
            head = head->next;
        }
        return head;
    }

    inline const __slist_node_base *
    __slist_previous(const __slist_node_base *head, const __slist_node_base *node) {
        while (head && head->next != node) {
            head = head->next;
        }
        return head;
    }

    ///全局函数，接合两个链表
    //参数是要插入链表首节点的前一个节点和要插入链表最后一个节点的前一个节点
    inline void __slist_splice_after(
            __slist_node_base *pos,
            __slist_node_base *before_first,
            __slist_node_base *before_last) {
        if (pos != before_first && pos != before_last) {
            __slist_node_base *first = before_first->next;          //标记要移除的一段链表的首尾
            __slist_node_base *after = pos->next;
            before_first->next = before_last->next;                 //移除链表
            pos->next = first;                                      //插入到新位置
            before_last->next = after;

        }
    }

    //传入本链表的插入位置和待插入链表的头节点
    inline void __slist_splice_after(__slist_node_base *pos, __slist_node_base *head) {
        __slist_node_base *__before_last = __slist_previous(head, 0);
        if (__before_last != head) {
            __slist_node_base *after = pos->next;
            pos->next = head->next;
            head->next = 0;
            __before_last->next = after;
        }
    };

    ///全局函数，头插法反转链表
    inline __slist_node_base *__slist_reverse(__slist_node_base *node) {
        __slist_node_base *__result = node;
        node = node->next;
        __result->next = 0;
        while (node) {
            __slist_node_base *next = node->next;
            node->next = __result;
            __result = node;
            node = next;
        }
        return __result;
    }


    //slist的迭代器：基类使用节点的基类定义指针，指向节点结构，完成指针的移动和比较操作
    // 子类使用节点子类指针构造，完成节点数据域的取值操作和++操作
    struct __slist_iterator_base {
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef forward_iterator_tag iterator_category;

        __slist_node_base *node;        //指向节点基本结构

        __slist_iterator_base(__slist_node_base *x) : node(x) {}

        void incr() { node = node->next; }         //前进一个节点

        //两个slist迭代器是否等同，视其__slist_node_base* node是否等同而定
        bool operator==(const __slist_iterator_base &x) const {
            return node == x.node;
        }

        bool operator!=(const __slist_iterator_base &x) const {
            return node != x.node;
        }
    };

    //单向链表的迭代器结构
    template<class T, class Ref, class Ptr>
    struct __slist_iterator : public __slist_iterator_base {
        typedef __slist_iterator<T, T &, T *> iterator;
        typedef __slist_iterator<T, const T &, const T *> const_iterator;
        typedef __slist_iterator<T, Ref, Ptr> self;

        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef __slist_node<T> list_node;

        //构造函数
        __slist_iterator(list_node *x) : __slist_iterator_base(x) {}

        __slist_iterator() : __slist_iterator_base(0) {}

        __slist_iterator(const iterator &x) : __slist_iterator_base(x.node) {}

        reference operator*() const { return ((list_node *) node)->data; }

        pointer operator->() const { return &(operator*()); }

        self &operator++() {
            incr();         //前进一个节点
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            incr();
            return tmp;
        }
    };


    template<class T, class Alloc = alloc>
    class slist {
    public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef __slist_iterator<T, T &, T *> iterator;
        typedef __slist_iterator<T, const T &, const T *> const_iterator;

    private:
        typedef __slist_node<T> list_node;
        typedef __slist_node_base list_node_base;
        typedef __slist_iterator_base iterator_base;
        typedef simple_alloc<list_node, Alloc> list_node_allocator;

    private:
        static list_node *create_node(const value_type &x) {
            list_node *node = list_node_allocator::allocate();
            try {
                construct(&node->data, x);
                node->next = 0;
            }
            catch (...) {
                list_node_allocator::deallocate(node);
            }
            return node;
        }

        static list_node *create_node() {
            list_node *node = list_node_allocator::allocate();
            try {
                construct(&node->data);
                node->next = 0;
            }
            catch (...) {
                list_node_allocator::deallocate(node);
            }
            return node;
        }


        static void destroy_node(list_node *node) {
            destroy(&node->data);
            list_node_allocator::deallocate(node, 1);
        }


    private:
        list_node_base head;            //头节点。是节点基类，只有指针，没有数据域。相当于哨兵，方便访问单链表的第一个节点

    public:
        ///构造和析构函数
        //默认构造
        slist() { head.next = 0; }

        //有参构造
        slist(size_type n, const value_type &x) {
            head.next = 0;              //没有用继承，就先初始化头节点
            insert_after_fill(&head, n, x);
        }

        slist(size_type n) {
            head.next = 0;
            insert_after_fill(&head, n, value_type());
        }

        slist(const_iterator first, const_iterator last) {
            head.next = 0;
            insert_after_range(&head, first, last);
        }

        slist(const value_type *first, const value_type *last) {
            head.next = 0;
            insert_after_range(&head, first, last);
        }

        //拷贝构造
        slist(const slist &x) {
            head.next = 0;
            insert_after_range(&head, x.begin(), x.end());
        }

        slist &operator=(const slist &x);


        ~slist() {
            clear();
        }

        ///assign替换本容器的内容
    private:
        void _m_fill_assign(size_type n, const value_type &val);

    public:
        void assign(size_type n, const value_type &val) {
            _m_fill_assign(n, val);
        }

    public:
        ///迭代器操作
        iterator begin() { return iterator((list_node *) head.next); }     //用头节点的下一个节点构造迭代器
        const_iterator begin() const { return iterator((list_node *) head.next); }

        iterator end() { return iterator(0); }

        const_iterator end() const { return iterator(0); }

        size_type size() const { return __slist_size(head.next); }

        bool empty() const { return head.next == 0; }

        //返回直接head的迭代器
        iterator before_begin() { return iterator((list_node *) &head); }       //子类指针指向基类对象
        const_iterator before_begin() const { return iterator((list_node *) &head); }       //子类指针指向基类对象
        size_type max_size() const { return size_type(-1); }        //size_type(-1) 把-1传给无符号类型size_t,得到该类型的最大值

        //两个slist互换：只要将head交换互指即可
        void swap(slist &L) {
            list_node_base *tmp = head.next;
            head.next = L.head.next;
            L.head.next = tmp;
        }

        //取头部元素
        reference front() { return ((list_node *) head.next)->data; } //用头节点的下一个节点构造节点子类用来取数据域
        const_reference front() const { return ((list_node *) head.next)->data; } //用头节点的下一个节点构造节点子类用来取数据域

        ///插入删除

        //从头部插入元素
        void push_front(const value_type &x) {
            __slist_make_link(&head, create_node(x));
        }

        void push_front() {
            __slist_make_link(&head, create_node());
        }


        void pop_front() {
            list_node *node = (list_node *) head.next;
            head.next = node->next;
            destroy_node(node);
        }

        void clear() { _m_erase_after(&head, 0); }

        //找到pos的前一个节点，时间复杂度最差为O(n)
        iterator previous(const_iterator pos) {
            return iterator((list_node *) __slist_previous(&head, pos.node));
        }

        const_iterator previous(const_iterator pos) const {
            return iterator((list_node *) __slist_previous(&head, pos.node));
        }

        ///插入
    private:

        list_node *_M_insert_after(list_node_base *pos, const value_type &x) {
            return (list_node *) (__slist_make_link(pos, create_node(x)));
        }

        list_node *_M_insert_after(list_node_base *pos) {
            return (list_node *) (__slist_make_link(pos, create_node()));
        }

        void insert_after_fill(list_node_base *pos, size_type n, const value_type &x) {
            for (size_type i = 0; i < n; ++i) {
                __slist_make_link(pos, create_node(x));
            }
        }

        void insert_after_range(list_node_base *pos, const_iterator first, const_iterator last) {
            while (first != last) {
                pos = __slist_make_link(pos, create_node(*first));
                ++first;
            }
        }

        void insert_after_range(list_node_base *pos, const value_type *first, const value_type *last) {
            while (first != last) {
                pos = __slist_make_link(pos, create_node(*first));
                ++first;
            }
        }


    public:

        //在pos之后插入一个节点
        iterator insert_after(iterator pos, const value_type &x) {
            return iterator(_M_insert_after(pos.node, x));
        }

        //在pos之后插入一个默认节点
        iterator insert_after(iterator pos) {
            return iterator(_M_insert_after(pos.node, value_type()));
        }

        //插入完成后，迭代器还是指向pos
        void insert_after(iterator pos, size_type n, const value_type &x) {
            insert_after_fill(pos.node, n, x);
        }

        void insert_after(iterator pos, const_iterator first, const_iterator last) {
            insert_after_range(pos.node, first, last);
        }

        void insert_after(iterator pos, const value_type *first, const value_type *last) {
            insert_after_range(pos.node, first, last);
        }


        //在pos之前插入
        iterator insert(iterator pos, const value_type &x) {
            return iterator(_M_insert_after(__slist_previous(&head, pos.node), x));
        }

        iterator insert(iterator pos) {
            return iterator(_M_insert_after(__slist_previous(&head, pos.node), value_type()));
        }

        void insert(iterator pos, size_type n, const value_type &x) {
            insert_after_fill(__slist_previous(&head, pos.node), n, x);
        }

        void insert(iterator pos, const_iterator first, const_iterator last) {
            insert_after_range(__slist_previous(&head, pos.node), first, last);
        }

        void insert(iterator pos, const value_type *first, const value_type *last) {
            insert_after_range(__slist_previous(&head, pos.node), first, last);
        }

        ///erase

    private:
        list_node_base *_m_erase_after(list_node_base *pos) {
            list_node *next = (list_node *) (pos->next);
            list_node_base *next_next = next->next;
            pos->next = next_next;
            list_node_allocator::deallocate(next);
            return next_next;
        }

        list_node_base *_m_erase_after(list_node_base *before_first, list_node_base *last_node) {
            list_node *cur = (list_node *) (before_first->next);
            while (cur != last_node) {
                list_node *tmp = cur;
                cur = (list_node *) cur->next;
                destroy_node(tmp);
            }
            before_first->next = last_node;
            return last_node;
        }

    public:
        iterator erase_after(iterator pos) {
            return iterator((list_node *) _m_erase_after(pos.node));
        }

        iterator erase_after(iterator before_first, iterator last) {
            return iterator((list_node *) _m_erase_after(before_first.node, last.node));
        }

        iterator erase(iterator pos) {
            return iterator((list_node *) _m_erase_after(__slist_previous(&head, pos.node)));
        }

        iterator erase(iterator first, iterator last) {
            return iterator((list_node *) _m_erase_after(__slist_previous(&head, first.node), last.node));
        }

        void resize(size_type new_size, const value_type &x);

        void resize(size_type new_size) { resize(new_size, value_type()); }

        //把两个单链表接合起来
        void splice_after(iterator pos, iterator before_first, iterator before_list) {
            if (before_first != before_list) {
                __slist_splice_after(pos.node, before_first.node, before_list.node);
            }
        }

        //接合一个节点
        void splice_after(iterator pos, iterator prev) {
            __slist_splice_after(pos.node, prev.node, prev.node->next);
        }

        void splice_after(iterator pos, slist &x) {
            __slist_splice_after(pos.node, &x.head);
        }

        void splice(iterator pos, slist &x) {
            if (x.head.next) {
                __slist_splice_after(__slist_previous(&head, pos.node), &x.head, __slist_previous(&x.head, 0));
            }
        }

        //把x链表的it所指的元素插入到本链表的pos之前
        void splice(iterator pos, slist &x, iterator i) {
            __slist_splice_after(__slist_previous(&head, pos.node), __slist_previous(&x.head, i.node), i.node);
        }

        void splice(iterator pos, slist &x, iterator first, iterator last) {
            if (first != last) {
                __slist_splice_after(__slist_previous(&head, pos.node),
                                     __slist_previous(&x.head, first.node),
                                     __slist_previous(&x.head, last.node));
            }
        }

    public:
        void reverse() {
            if (head.next) {
                head.next = __slist_reverse(head.next);
            }
        }

        void remove(const value_type &val);

        void unique();

        void merge(slist &x);

        void sort();

    };


    template<class T, class Alloc>
    slist<T, Alloc> &slist<T, Alloc>::operator=(const slist<T, Alloc> &x) {
        if (&x != this) {
            list_node_base *p1 = &head;
            list_node *n1 = (list_node *) head.next;
            const list_node *n2 = (const list_node *) x.head.next;              //x节点不能改变
            while (n1 && n2) {
                n1->data = n2->data;    //只赋值，不插入节点
                p1 = n1;                //p1是用来表示本链表操作节点的上一个节点，用于之后插入删除的标识位
                n1 = (list_node *) n1->next;
                n2 = (const list_node *) n2->next;
            }
            if (n2 == 0) {
                _m_erase_after(p1, 0);
            } else {
                insert_after_range(p1, const_iterator((list_node *) n2), const_iterator(0));
            }
        }
        return *this;
    }

    template<class T, class Alloc>
    void slist<T, Alloc>::_m_fill_assign(size_type n, const value_type &val) {
        list_node_base *prev = &head;
        list_node *node = (list_node *) head.next;
        for (; node != 0 && n > 0; --n) {
            node->data = val;
            prev = node;
            node = (list_node *) node->next;
        }
        if (n > 0) {
            insert_after_fill(prev, n, val);
        } else {}
        _m_erase_after(prev, 0);
    }

    //每个节点的值都比较
    template<class T, class Alloc>
    inline bool
    operator==(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
        typedef typename slist<T, Alloc>::const_iterator const_iterator;
        const_iterator end1 = sl1.end();
        const_iterator end2 = sl2.end();

        const_iterator i1 = sl1.begin();
        const_iterator i2 = sl2.begin();

        while (i1 != end1 && i2 != end2 && *i1 == *i2) {
            ++i1;
            ++i2;
        }
        return i1 == end1 && i2 == end2;
    }

    template<class T, class Alloc>
    inline bool
    operator<(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
        return lexicograpical_compare(sl1.begin(), sl1.end(), sl2.begin(), sl2.end());
    }

    template<class T, class Alloc>
    inline bool
    operator!=(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
        return !(sl1 == sl2);
    }

    template<class T, class Alloc>
    inline bool
    operator>(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
        return sl2 < sl1;
    }

    template<class T, class Alloc>
    inline bool
    operator<=(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
        return !(sl2 < sl1);
    }

    template<class T, class Alloc>
    inline bool
    operator>=(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
        return !(sl1 < sl2);
    }

    template<class T, class Alloc>
    void slist<T, Alloc>::resize(size_type len, const value_type &x) {
        list_node_base *cur = &head;        //从头开始遍历
        while (cur->next != 0 && len > 0) {
            --len;
            cur = cur->next;
        }
        if (cur->next) {
            _m_erase_after(cur, 0);
        } else {
            insert_after_fill(cur, len, x);
        }
    }

    //移除data域值为val的节点
    template<class T, class Alloc>
    void slist<T, Alloc>::remove(const value_type &val) {
        list_node_base *cur = &head;
        while (cur && cur->next) {
            if (((list_node *) cur->next)->data == val) {
                _m_erase_after(cur);
            } else {
                cur = cur->next;
            }
        }
    }

    //这个unique只针对有序链表
    template<class T, class Alloc>
    void slist<T, Alloc>::unique() {
        list_node_base *cur = head.next;
        if (cur) {
            while (cur->next) {
                if (((list_node *) cur)->data ==
                    ((list_node *) cur->next)->data) {
                    _m_erase_after(cur);
                } else {
                    cur = cur->next;
                }
            }
        }
    }

    //合并两个有序数组，默认从小到大
    template<class T, class Alloc>
    void slist<T, Alloc>::merge(slist &x) {
        list_node_base *n1 = &head;
        while (n1->next && x.head.next) {
            if (((list_node *) x.head.next)->data <
                ((list_node *) n1->next)->data) {
                __slist_splice_after(n1, &x.head, x.head.next);
            }
            n1 = n1->next;
        }
        if (x.head.next) {
            n1->next = x.head.next;
            x.head.next = 0;
        }
    }

    template<class T, class Alloc>
    void slist<T, Alloc>::sort() {
        if (head.next && head.next->next) {         //链表为空或只有一个节点，则不需要排序
            slist carry;
            slist counter[64];
            int fill = 0;
            while (!empty()) {
                __slist_splice_after(&carry.head, &head, head.next);             //把头节点插入carry
                int i = 0;
                while (i < fill && !counter[i].empty()) {
                    counter[i].merge(carry);
                    carry.swap(counter[i]);
                    ++i;
                }
                carry.swap(counter[i]);
                if (i == fill) {
                    ++fill;
                }
            }
            for (int i = 1; i < fill; ++i) {
                counter[i].merge(counter[i - 1]);
            }
            this->swap(counter[fill - 1]);
        }
    }

}

#endif //IMPLTMENT_STL_MY_SLIST_H


