//
// Created by 梁磊磊 on 2023/6/4.
//

#include "RB-tree.h"
#include "iostream"

template<class Arg, class Result>
struct unary_function {
    typedef Arg argument_type;
    typedef Result result_type;
};

template<class T>
struct identity : public unary_function<T, T> {
    const T &operator()(const T &x) const {
        return x;
    }
};

template<class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

template<class T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T &x, const T &y) const {
        return x < y;
    }
};

template<class T=mystl::_rb_tree<int, int, identity<int>, less<int>>>
void print_tree(T &tree) {
    typename T::iterator ite1 = tree.begin();
    typename T::iterator ite2 = tree.end();

    std::cout << "node.value: ";
    for (; ite1 != ite2; ++ite1) {
        std::cout << *ite1 << ':' << ite1.get_color() << ' ';
    }
    std::cout << std::endl;

}

int main() {
    int ia[5] = {0, 1, 2, 3, 4};
    mystl::_rb_tree<int, int, identity<int>, less<int> > itree;
    itree.insert_unique(ia, ia + 5);
    std::cout << itree.size() << std::endl;
    //std::cout << "root = " << *itree.rt() << std::endl;
    print_tree(itree);
    itree.erase(1);
    print_tree(itree);

//    itree.insert_equal(30);
//    itree.insert_equal(20);
//    itree.insert_equal(40);
//    itree.insert_equal(10);
//    itree.insert_equal(35);
//    itree.insert_equal(50);
//    itree.insert_equal(10);     //插入相同的值会插入到左子树。
////    itree.insert_equal(20);
////    itree.insert_equal(35);
////    itree.insert_equal(45);
//    print_tree(itree);


//    itree.insert_unique(30);
//    itree.insert_unique(10);
//    itree.insert_unique(50);
//    itree.insert_unique(5);
//    itree.insert_unique(20);
//    //itree.insert_unique(50);
//    itree.insert_unique(15);
//    itree.insert_unique(25);
//
//    print_tree(itree);
//
//    itree.erase(50);
//    print_tree(itree);
//
//    mystl::_rb_tree<int, int, identity<int>, less<int> >::iterator it;
//    it = itree.find(20);
//    std::cout << *it << std::endl;


}