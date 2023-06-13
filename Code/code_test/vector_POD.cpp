//
// Created by 梁磊磊 on 2023/4/25.
//
#include "iostream"
#include "my_vector.h"
#include "algorithm"

using namespace std;


inline void printInfo(mystl::vector<int> &iv) {
    cout << "size=" << iv.size() << " capacity=" << iv.capacity() << endl;

}

void printVector(mystl::vector<int> &iv) {
    for (int i = 0; i < iv.size(); i++) {
        cout << iv[i] << " ";
    }
    cout << endl;
}

int main() {


    mystl::vector<int> iv(3, 9);
    //mystl::vector<int> ivec = {1,2,3,4,5};
    printInfo(iv);
    printVector(iv);                //9 9 9

    for (int i = 1; i < 5; i++) {
        iv.push_back(i);
        printInfo(iv);
    }


    printVector(iv);            //9 9 9 1 2 3 4
    int i = 0;
    std::cout << "------------------------------------" << std::endl;
    iv.push_back(i);
    printInfo(iv);
    printVector(iv);            //9 9 9 1 2 3 4 0
    std::cout << "------------------------------------" << std::endl;
    iv.pop_back();
    iv.pop_back();
    printInfo(iv);

    iv.pop_back();
    printInfo(iv);
    printVector(iv);            //9 9 9 1 2
    std::cout << "------------------------------------" << std::endl;
    mystl::vector<int>::iterator ivite = std::find(iv.begin(), iv.end(), 1);
    if (ivite != iv.end())
        iv.erase(ivite);
    printInfo(iv);
    printVector(iv);            //9 9 9 2

    ivite = std::find(iv.begin(), iv.end(), 2);
    if (ivite != iv.end())
        iv.insert(ivite, 3, 7);
    printInfo(iv);
    printVector(iv);            //9 9 9 7 7 7 2
    std::cout << "------------------------------------" << std::endl;
    iv.clear();
    printInfo(iv);
}
