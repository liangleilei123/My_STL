//
// Created by 梁磊磊 on 2023/5/22.
//

#ifndef IMPLTMENT_STL_MY_STACK_H
#define IMPLTMENT_STL_MY_STACK_H

#include "my_deque.h"

#ifndef __STL_NULL_TMPL_ARGS
#define __STL_NULL_TMPL_ARGS <>
#endif

namespace mystl {
    template<class T, class Sequence = deque<T>>
    class stack {
        friend bool operator==(const stack &, const stack &);

        friend bool operator<(const stack &, const stack &);

    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;
    protected:
        Sequence c; //底层容器
    public:
        //以下完全利用Sequence c的操作，完成stack的操作
        bool empty() const { return c.empty(); }

        size_type size() const { return c.size(); }

        reference top() { return c.back(); }

        const_reference top() const { return c.back(); }

        //stack()是后进先出
        void push(const value_type &x) { c.push_back(x); }

        void pop() { c.pop_back(); }
    };


    template<class T, class Sequence>
    bool operator==(const stack<T, Sequence> &x, const stack<T, Sequence> &y) {
        x.c == y.c;
    }

    template<class T, class Sequence>
    bool operator<(const stack<T, Sequence> &x, const stack<T, Sequence> &y) {
        return x.c < y.c;
    }

}


#endif //IMPLTMENT_STL_MY_STACK_H
