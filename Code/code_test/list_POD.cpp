//
// Created by 梁磊磊 on 2023/5/11.
//

#include "my_list.h"
#include "iostream"
#include "algorithm"

//遍历list
template<class T>
void print_list(mystl::list<T> &x) {
    mystl::list<int>::iterator it = x.begin();
    while (it != x.end()) {
        std::cout << *it << ' ';
        ++it;
    }
    std::cout << std::endl;
}

template<class T>
typename mystl::list<T>::iterator find_node(
        typename mystl::list<T>::iterator first,
        typename mystl::list<T>::iterator last,
        typename mystl::list<T>::const_reference x) {
    for (typename mystl::list<T>::iterator i = first; i != last; ++i) {
        if (*i == x) {
            return i;
        }
    }
    return last;
}

int main() {
    mystl::list<int> l;
    mystl::list<int>::iterator it;

    mystl::list<int>::const_iterator cit;


    l.push_back(0);
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    l.push_back(4);

    cit = l.cbegin();
    std::cout << *cit << std::endl;


    std::cout << l.front() << std::endl; // 0
    std::cout << "size = " << l.size() << std::endl;      //5


    it = find_node<int>(l.begin(), l.end(), 3);

    l.insert(it, 99);
    l.push_back(66);
    print_list<int>(l);               //0 1 2 99 3 4 66
    std::cout << "size = " << l.size() << std::endl;      //7

    it = find_node<int>(l.begin(), l.end(), 4);
    l.erase(it);
    print_list<int>(l);               //0 1 2 99 3 66

    l.push_front(0);
    l.push_back(66);

    l.remove(2);
    print_list<int>(l);             //0 0 1 99 3 66 66

    l.unique();
    print_list<int>(l);             //0 1 99 3 66

//    l.clear();
//    print_list<int>(l);

    mystl::list<int> l1;
    l1.push_back(11);
    l1.push_back(12);

    it = find_node<int>(l.begin(), l.end(), 3);
    l.splice(it, l1);           //splice的操作不是拷贝复制，是剪切复制
    print_list<int>(l);                //0 1 99 11 22 3 66

    mystl::list<int>::iterator it1 = it;
    ++it1;
    l.splice(it, l, it1);
    print_list<int>(l);                 //0 1 99 11 22 66 3


    l1.push_back(13);
    l1.push_back(14);
    print_list(l1);                     //13 14
    l.splice(it, l1, l1.begin(), l1.end());
    print_list(l);                      //0 1 99 11 12 66 13 14 3

    l.reverse();
    print_list(l);                      //3 14 13 66 12 11 99 1 0

    l1.push_back(15);
    l1.push_back(16);
    l.swap(l1);
    print_list(l);                      //15 16
    std::cout << "size = " << l.size() << std::endl;        //2
    print_list(l1);                     //3 14 13 66 12 11 99 1 0
    std::cout << "size = " << l1.size() << std::endl;       //9

    l1.sort();
    print_list(l1);                     //0 1 3 11 12 13 14 66 99

    l1.merge(l);
    print_list(l1);                     //0 1 3 11 12 13 14 15 16 66 99




}