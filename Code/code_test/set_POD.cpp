//
// Created by 梁磊磊 on 2023/6/9.
//

#include "my_set.h"
#include "iostream"


int main() {
    int i;
    int ia[5] = {0, 1, 2, 3, 4};
    mystl::set<int> iset(ia, ia + 5);

    std::cout << "size = " << iset.size() << std::endl;
    std::cout << "3 count = " << iset.count(3) << std::endl;
    iset.insert(3);
    std::cout << "size = " << iset.size() << std::endl;
    std::cout << "3 count = " << iset.count(3) << std::endl;

    iset.insert(5);
    std::cout << "size = " << iset.size() << std::endl;
    std::cout << "3 count = " << iset.count(3) << std::endl;

    iset.erase(1);
    std::cout << "size = " << iset.size() << std::endl;
    std::cout << "3 count = " << iset.count(3) << std::endl;
    std::cout << "1 count = " << iset.count(1) << std::endl;

    mystl::set<int>::iterator ite1 = iset.begin();
    mystl::set<int>::iterator ite2 = iset.end();

    for (; ite1 != ite2; ++ite1) {
        std::cout << *ite1 << ' ';
    }
    std::cout << std::endl;

    ite1 = iset.find(3);
    if (ite1 != iset.end()) {
        std::cout << "3 found" << std::endl;
    }


}