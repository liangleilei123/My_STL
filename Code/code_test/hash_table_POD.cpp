//
// Created by 梁磊磊 on 2023/6/12.
//

#include "hashtable.h"
#include "functional.h"
#include "iostream"
#include "stl_hash_fun.h"

int main() {
    mystl::hashtable<int, int, mystl::hash<int>,
            mystl::identity<int>, mystl::equal_to<int>>
            iht(50, mystl::hash<int>(), mystl::equal_to<int>());

    std::cout << iht.size() << std::endl;
    std::cout << iht.bucket_count() << std::endl;
    std::cout << iht.max_bucket_count() << std::endl;

    iht.insert_unique(59);
    iht.insert_unique(63);
    iht.insert_unique(108);
    iht.insert_unique(2);
    iht.insert_unique(53);
    iht.insert_unique(55);
    std::cout << iht.size() << std::endl;

    mystl::hashtable<int, int, mystl::hash<int>,
            mystl::identity<int>, mystl::equal_to<int>>
    ::iterator ite = iht.begin();
    for (int i = 0; i < iht.size(); ++i, ++ite) {
        std::cout << *ite << ' ';
    }
    std::cout << std::endl;

    for (int i = 0; i <= 47; i++) {
        iht.insert_equal(i);
    }

    iht.erase(2);


    std::cout << iht.size() << std::endl;
    std::cout << iht.bucket_count() << std::endl;

    if (iht.find(2) == iht.end())
        std::cout << "not found" << std::endl;
    std::cout << iht.count(2) << std::endl;

}
