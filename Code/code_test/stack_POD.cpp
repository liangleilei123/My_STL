//
// Created by 梁磊磊 on 2023/5/22.
//
#include "my_list.h"
#include "my_stack.h"
#include "iostream"

template<class T = int>
void print_value(T x) {
    std::cout << x << std::endl;
}

int main() {
    //mystl::stack<int, mystl::list<int>> dstack;       //使用list为底层容器
    mystl::stack<int> dstack;                           //默认使用deque为底层容器
    dstack.push(1);
    dstack.push(3);
    dstack.push(5);
    dstack.push(7);

    print_value(dstack.size());         //4
    print_value(dstack.top());          //7

    dstack.pop();
    print_value(dstack.top());          //5
    dstack.pop();
    print_value(dstack.top());          //3
    dstack.pop();
    print_value(dstack.top());          //1
    print_value(dstack.size());         //1

}
