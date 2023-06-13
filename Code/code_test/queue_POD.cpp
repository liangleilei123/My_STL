//
// Created by 梁磊磊 on 2023/5/22.
//
#include "my_list.h"
#include "my_queue.h"
#include "iostream"

template<class T = int>
void print_value(T x) {
    std::cout << x << std::endl;
}

int main() {
    mystl::queue<int, mystl::list<int>> q;       //使用list为底层容器
    //mystl::queue<int> q;                           //默认使用deque为底层容器
    q.push(1);
    q.push(3);
    q.push(5);
    q.push(7);

    print_value(q.size());         //4
    print_value(q.front());          //1

    q.pop();
    print_value(q.front());          //3
    q.pop();
    print_value(q.front());          //5
    q.pop();
    print_value(q.front());          //7
    print_value(q.size());         //1

}
