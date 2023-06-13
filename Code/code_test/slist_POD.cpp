//
// Created by 梁磊磊 on 2023/5/25.
//

#include "my_slist.h"
#include "iostream"
#include "my_vector.h"

template<class T>
void print_slist(mystl::slist<T> &sl) {
    typename mystl::slist<T>::iterator ite = sl.begin();
    typename mystl::slist<T>::iterator ite2 = sl.end();
    for (; ite != ite2; ++ite) {
        std::cout << *ite << ' ';
    }
    std::cout << std::endl;
}

template<class T>
typename mystl::slist<T>::iterator find_slist(mystl::slist<T> &l, T x) {
    for (auto it = l.begin(); it != l.end(); ++it) {
        if (*it == x)
            return it;
    }
    return l.end();

}

int main() {

    //默认构造
    mystl::slist<int> islist;
    //有参构造
    mystl::slist<int> islist1(7, 2);
    mystl::slist<int> islist2(3);

    int ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
    mystl::vector<int> vec(ia, ia + 6);
    mystl::slist<int> islist3(ia, ia + 9);
    mystl::slist<int> islist4(vec.begin(), vec.end());
    //拷贝构造
    mystl::slist<int> islist5(islist4);


    print_slist(islist1);                                      //2 2 2 2 2 2 2
    print_slist(islist2);                                       //0 0 0
    print_slist(islist3);                                       //0 1 2 3 4 8 9 3 5
    print_slist(islist4);                                       //0 1 2 3 4 8
    print_slist(islist5);                                       //0 1 2 3 4 8

    std::cout << islist.empty() << std::endl;                       //1,是空的，还没插入元素


    std::cout << "size = " << islist.size() << std::endl;           //size = 0


    islist.push_front(9);
    islist.push_front(1);
    islist.push_front(2);
    islist.push_front(3);
    islist.push_front(4);
    std::cout << "size = " << islist.size() << std::endl;           //size = 5

    print_slist(islist);                                        //4 3 2 1 9

    islist.pop_front();
    print_slist(islist);                                        //3 2 1 9

    std::cout << islist.front() << std::endl;                       //3

    mystl::slist<int> sl;
    sl.push_front(6);

    islist.swap(sl);
    print_slist(islist);                                        //6

    sl.clear();
    std::cout << "size = " << sl.size() << std::endl;               //size = 0
    std::cout << islist.front() << std::endl;                       //6
    islist.push_front();
    print_slist(islist);                                        //0 6

    //pervious  测试
    print_slist(islist3);                                       //0 1 2 3 4 8 9 3 5
    mystl::slist<int>::iterator it, it_pre;
    it = find_slist(islist3, 3);
    if (it != islist3.end()) {
        it_pre = islist3.previous(it);
    }
    std::cout << *it_pre << std::endl;                              //2


    //insert_after测试
    //it的位置指向的是链表中第一个3
    islist3.insert_after(it);
    print_slist(islist3);                                       //0 1 2 3 0 4 8 9 3 5

    islist3.insert_after(it, 7);
    print_slist(islist3);                                       //0 1 2 3 7 0 4 8 9 3 5

    islist3.insert_after(it, 3, 6);
    print_slist(islist3);                                       //0 1 2 3 6 6 6 7 0 4 8 9 3 5

    islist3.insert_after(it, vec.begin(), vec.begin() + 3);
    print_slist(islist3);                                       //0 1 2 3 0 1 2 6 6 6 7 0 4 8 9 3 5


    //insert
    islist3.insert(it);
    islist3.insert(it, 4);
    print_slist(islist3);                                       //0 1 2 0 4 3 0 1 2 6 6 6 7 0 4 8 9 3 5

    islist3.insert(it, 2, 66);
    print_slist(islist3);                                       //0 1 2 0 4 66 66 3 0 1 2 6 6 6 7 0 4 8 9 3 5

    islist3.insert(it, vec.begin(), vec.begin() + 2);
    print_slist(islist3);                                       //0 1 2 0 4 66 66 0 1 3 0 1 2 6 6 6 7 0 4 8 9 3 5

    it = islist3.erase_after(it);
    std::cout << *it << std::endl;                                  //删除后迭代器指向删除节点的下一个节点
    print_slist(islist3);                                       //0 1 2 0 4 66 66 0 1 3 1 2 6 6 6 7 0 4 8 9 3 5

    islist3.erase_after(it, islist3.end());
    print_slist(islist3);                                       //0 1 2 0 4 66 66 0 1 3 1

    it = find_slist(islist3, 4);
    it = islist3.erase(it);                                    //erase之后，指向节点的迭代器失效，返回指向节点下一个节点的迭代器
    print_slist(islist3);                                       //0 1 2 0 66 66 0 1 3 1

    islist3.erase(it, islist3.end());
    print_slist(islist3);                                       //0 1 2 0

    islist3.splice_after(islist3.begin(), islist4.begin());             //在外部是访问不到单链表的head节点的，要自己定义
    mystl::slist<int>::iterator head;
    head = islist4.previous(islist4.begin());
    islist3.splice_after(islist3.begin(), head);
    std::cout << *islist4.begin() << std::endl;                     //2
    print_slist(islist3);                                       //0 0 1 1 2 0

    mystl::slist<int>::iterator it1 = find_slist(islist4, 8);
    std::cout << *it1 << std::endl;                                 //8
    islist3.splice_after(islist3.begin(), islist4.begin(), it1);
    print_slist(islist3);                                       //0 3 4 8 0 1 1 2 0

    islist3.splice_after(islist3.begin(), islist5);
    print_slist(islist3);                                       //0 0 1 2 3 4 8 3 4 8 0 1 1 2 0

    it1 = find_slist(islist3, 8);
    islist3.splice(it1, islist2, islist2.begin());
    print_slist(islist3);                                       //0 0 1 2 3 4 0 8 3 4 8 0 1 1 2 0

    islist3.splice(it1, islist2, islist2.begin(), islist2.end());
    print_slist(islist3);                                       //0 0 1 2 3 4 0 0 0 8 3 4 8 0 1 1 2 0

    islist2 = islist3;
    print_slist(islist3);                                       //0 0 1 2 3 4 0 0 0 8 3 4 8 0 1 1 2 0
    print_slist(islist2);                                       //0 0 1 2 3 4 0 0 0 8 3 4 8 0 1 1 2 0

    islist2.assign(3, 6);
    print_slist(islist2);                                       //6 6 6

    islist3.reverse();
    print_slist(islist3);                                       //0 2 1 1 0 8 4 3 8 0 0 0 4 3 2 1 0 0

    islist3.resize(5);
    print_slist(islist3);                                       //0 2 1 1 0

    islist3.remove(1);
    print_slist(islist3);                                       //0 2 0

    islist2.unique();
    print_slist(islist2);                                       //6

    islist3.sort();
    islist2.merge(islist3);
    print_slist(islist2);                                       //0 0 2 6


    return 0;
}