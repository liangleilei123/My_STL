//
// Created by 梁磊磊 on 2023/5/17.
//

#include "my_deque.h"
#include "iostream"

template<class T = int>
void print_deque(mystl::deque<T> &dq) {
    for (auto it = dq.begin(); it != dq.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;

}


template<class T = int>
void print_value(T x) {
    std::cout << x << std::endl;
}

int main() {
    mystl::deque<int> dq(2, 1);
    mystl::deque<int>::iterator it;


    for (int i = 2; i < 7; ++i) {
        dq.push_back(i);
        dq.push_front(i);
    }

    print_deque(dq);

    std::cout << "size = " << dq.size() << std::endl;
    it = dq.begin();
    print_value(*it);
    print_value(dq[5]);
    print_value(dq.front());
    print_value(dq.back());
    if (!dq.empty())
        std::cout << "not empty" << std::endl;
    else
        std::cout << "empty" << std::endl;

    dq.pop_front();
    dq.pop_back();
    print_deque(dq);

    std::cout << "---------------" << std::endl;
    it = dq.begin();
    mystl::deque<int>::iterator ite = dq.end();
    std::cout << ite - it << std::endl;

    print_value(*it);
    //it = dq.erase(it);
    //dq.erase(dq.begin(), it);
    dq.insert(it, 9);
    print_deque(dq);

//    dq.clear();
    std::cout << "**************" << std::endl;
    int *f = dq.first();
    int *l = dq.last();

    print_value(l - f);
}

