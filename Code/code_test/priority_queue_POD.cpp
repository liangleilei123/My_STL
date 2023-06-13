//
// Created by 梁磊磊 on 2023/5/24.
//

#include "my_priority_queue.h"
#include "iostream"


int main() {
    int ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
    mystl::priority_queue<int> ipq(ia, ia + 9);
    std::cout << "size = " << ipq.size() << std::endl;      //size = 9

    for (int i = 0; i < ipq.size(); ++i) {
        std::cout << ipq.top() << ' ';
    }                                   //9 9 9 9 9 9 9 9 9
    std::cout << std::endl;

    ipq.push(7);

    while (!ipq.empty()) {
        std::cout << ipq.top() << ' ';
        ipq.pop();
    }                                   //9 8 7 5 4 3 3 2 1 0
    std::cout << std::endl;

    std::cout << "size = " << ipq.size() << std::endl;      //size = 0

    mystl::priority_queue<int> ipq1;
    ipq1.push(1);
    std::cout << ipq1.top() << std::endl;

    return 0;
}