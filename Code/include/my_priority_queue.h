//
// Created by 梁磊磊 on 2023/5/24.
//

#ifndef IMPLTMENT_STL_MY_PRIORITY_QUEUE_H
#define IMPLTMENT_STL_MY_PRIORITY_QUEUE_H

#include "my_vector.h"
#include "functional.h"
#include "my_heap.h"

namespace mystl {
    template<class T, class Sequence = vector<T>,
            class Compare = less<typename Sequence::value_type>>
    class priority_queue {
    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;

    protected:
        Sequence c;     //底层容器
        Compare comp;

    public:
        priority_queue() : c() {}

        explicit priority_queue(const Compare &x) : c(), comp(x) {}

        //任何一个构造函数都立刻于底层容器内产生一个隐式堆
        template<class InputIterator>
        priority_queue(InputIterator first, InputIterator last, const Compare &x)
                :c(first, last), comp(x) {
            make_heap(c.begin(), c.end(), comp);        //再实现一个可以指定比较表达式的heap
        }

        template<class InputIterator>
        priority_queue(InputIterator first, InputIterator last)
                :c(first, last) {
            make_heap(c.begin(), c.end(), comp);
        }

        bool empty() const { return c.empty(); }

        size_type size() const { return c.size(); }

        reference top() { return c.front(); }

        const_reference top() const { return c.front(); }

        void push(const value_type &x) {
            try {
                //push_back是泛型算法，先利用底层容器的push_back()将新元素推入末端，再重排heap
                c.push_back(x);
                push_heap(c.begin(), c.end(), comp);
            }
            catch (...) {
                c.clear();
            }
        }

        void pop() {
            try {
                pop_heap(c.begin(), c.end(), comp);
                c.pop_back();
            }
            catch (...) {
                c.clear();
            }
        }
    };

}


#endif //IMPLTMENT_STL_MY_PRIORITY_QUEUE_H
