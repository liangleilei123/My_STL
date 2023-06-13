//
// Created by 梁磊磊 on 2023/5/24.
//
#include "my_vector.h"
#include "iostream"
#include "my_heap.h"

void printVector(mystl::vector<int> &iv) {
    for (int i = 0; i < iv.size(); i++) {
        std::cout << iv[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    int ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};

    mystl::vector<int> ivec(ia, ia + 9);
    printVector(ivec);          //0 1 2 3 4 8 9 3 5

    mystl::make_heap(ivec.begin(), ivec.end());
    printVector(ivec);          //9 5 8 3 4 0 2 3 1

    ivec.push_back(7);          //把7放在最尾端
    mystl::push_heap(ivec.begin(), ivec.end());                  //插入大根堆的合适位置。
    printVector(ivec);          //9 7 8 3 5 0 2 3 1 4

    mystl::pop_heap(ivec.begin(), ivec.end());
    std::cout << ivec.back() << std::endl;
    ivec.pop_back();               //9
    printVector(ivec);          //8 7 4 3 5 0 2 3 1

    mystl::sort_heap(ivec.begin(), ivec.end());
    printVector(ivec);          //0 1 2 3 3 4 5 7 8

}
