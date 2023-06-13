//
// Created by 梁磊磊 on 2023/5/22.
//

#ifndef IMPLTMENT_STL_MY_QUEUE_H
#define IMPLTMENT_STL_MY_QUEUE_H

#include "my_deque.h"
//先进先出队列

namespace mystl {
    template<class T, class Sequence = deque<T>>
    class queue {
        friend bool operator==(const queue &x, const queue &y);

        friend bool operator<(const queue &x, const queue &y);

    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;

    protected:
        Sequence c;

    public:
        //以下完全利用Sequence c的操作，完成stack的操作
        bool empty() const { return c.empty(); }

        size_type size() const { return c.size(); }

        reference front() { return c.front(); }

        reference back() { return c.back(); }

        const_reference back() const { return c.back(); }

        //stack()是后进先出
        void push(const value_type &x) { c.push_back(x); }

        void pop() { c.pop_front(); }
    };

    template<class T, class Sequence>
    bool operator==(const queue<T, Sequence> &x, const queue<T, Sequence> &y) {
        x.c == y.c;
    }

    template<class T, class Sequence>
    bool operator<(const queue<T, Sequence> &x, const queue<T, Sequence> &y) {
        return x.c < y.c;
    }

}


#endif //IMPLTMENT_STL_MY_QUEUE_H
