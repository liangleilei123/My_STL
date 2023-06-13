//
// Created by 梁磊磊 on 2023/5/14.
//

#include "iostream"
#include "my_list.h"


class test {
public:
    test() : i(0) {}

    test(double x) : i(x) {}

    double i;

    bool operator==(const test &x) const {
        return i == x.i;
    }

    bool operator<(const test &x) const {
        return i < x.i;
    }
};

template<class T = test>
void print_list(mystl::list<T> &x) {
    typename mystl::list<T>::iterator it = x.begin();
    while (it != x.end()) {
        std::cout << (*it).i << ' ';
        ++it;
    }
    std::cout << std::endl;
}

template<class T = test>
typename mystl::list<T>::iterator find_node(
        typename mystl::list<T>::iterator first,
        typename mystl::list<T>::iterator last,
        typename mystl::list<T>::const_reference x) {
    for (typename mystl::list<T>::iterator i = first; i != last; ++i) {
        if ((*i).i == x.i) {
            return i;
        }
    }
    return last;
}

int main() {
    mystl::list<test> l;
    test t = test(1.1);

    l.push_back(t);
    l.push_back(test(2.2));
    l.push_back(test(3.3));
    l.push_back(test(4.4));
    std::cout << l.begin()->i << std::endl;                     //1.1
    std::cout << "size() = " << l.size() << std::endl;          //4
    std::cout << l.front().i << std::endl;                      //1.1
    print_list(l);                                          //1.1 2.2 3.3 4.4

    mystl::list<test>::iterator it = find_node(l.begin(), l.end(), 2.2);
    std::cout << it->i << std::endl;                            //2.2
    l.insert(it, 2.2);        //有隐式类型转换
    l.push_front(test(0.2));
    print_list(l);                                          //0.2 1.1 2.2 2.2 3.3 4.4

    mystl::list<test>::iterator it1 = find_node(l.begin(), l.end(), 3.3);
    it1 = l.erase(it1);
    std::cout << it1->i << std::endl;                              //4.4
    print_list(l);                                              //0.2 1.1 2.2 2.2 4.4

    l.pop_back();
    l.pop_front();
    print_list(l);                                              //1.1 2.2 2.2

    l.remove(test(2.2));
    print_list(l);

    mystl::list<test> l1;
    l1.push_back(test(3.5));
    l1.push_back(test(2.5));
    l1.push_back(test(3.7));
    l1.push_back(test(1.1));
    l1.push_back(test(2.8));
    l1.push_back(test(4.9));
    l1.push_back(test(2.5));
    print_list(l1);
    l1.sort();
    print_list(l1);

    l.merge(l1);
    print_list(l);

    l.unique();
    print_list(l);

    l.reverse();
    print_list(l);

    l.clear();
    std::cout << "size() = " << l.size() << std::endl;


}
