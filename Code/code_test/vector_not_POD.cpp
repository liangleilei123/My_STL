//
// Created by 梁磊磊 on 2023/5/1.
//
#include "iostream"
#include "my_vector.h"


using namespace mystl;

//用非POD类型来测试
class myclass {
private:
    int myvalue;
public:
    myclass() : myvalue(0) {}

    myclass(int value) : myvalue(value) {}

    //拷贝构造
    myclass(const myclass &m) {
        this->myvalue = m.getter();
    }

    ~myclass() {}

    int getter() const { return myvalue; }
};

int main() {
    //以下是测试内容和应有结果

    vector<int> &a = *(new vector<int>(60000000, 1));    //初始长度六千万，默认值为1
    std::cout << a.size() << std::endl;    //60000000
    delete &a;

    //vector<myclass> &vec = *(new vector<myclass>(4));
    vector<myclass> vec;
    for (int i = 1; i < 10; i++) {
        myclass &temp = *(new myclass(i * 100));
        vec.push_back(temp);
    }

    vector<myclass> vec_copy(vec);

    std::cout << vec.size() << std::endl;                               //13
    std::cout << vec.capacity() << std::endl;                           //16
    std::cout << vec.front().getter() << std::endl;                     //0
    std::cout << vec.back().getter() << std::endl;                      //900

    vec.insert(vec.begin() + 5, myclass(666));
    std::cout << vec[5].getter() << std::endl;                          //666

    vec.insert(vec.begin() + 2, 3, 123);
    std::cout << vec[2].getter() << std::endl;
    std::cout << vec[3].getter() << std::endl;

    vec.erase(vec.begin() + 2);                                 //删去的值是300

    std::cout << vec[2].getter() << std::endl;                          //0
    std::cout << vec.size() << std::endl;                               //8

    vec.erase(vec.begin() + 4, vec.end() - 2);                    //

    std::cout << vec.size() << std::endl;                               //6
    std::cout << vec[4].getter() << std::endl;                          //800

    return 0;
}