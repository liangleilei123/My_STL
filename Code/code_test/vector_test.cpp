/*
 * @Descripttion:My_vector.h的测试文件
 * @version: 1.0
 * @Author: liangleilei
 * @Date: 2023-03-21 11:56:45
 * @LastEditTime: 2023-03-28 01:50:15
 */
#include <iostream>
#include "../include/_vector.h"


using std::cout;
using std::endl;

// 遍历数组
// void traverse(include vec) // 如果不使用引用传递，则函数运行时调用拷贝构造函数
void traverse(const vector<char> &vec) {
    for (int i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << ' ';
    }
    std::cout << std::endl;
}

int main() {
    // 对构造函数的测试
    vector<char> vec;           // 默认构造
    vector<char> vec1(6);       // 有参构造，6有容器长度
    vector<char> vec2(10, 'a'); // 有参构造，数组长度为10，都初始化为1
    vector<char> vec3(vec2);    // 拷贝构造

    for (int i = 0; i < 26; ++i) {
        vec.push_back(i + 'a');
    }
    traverse(vec);

    // 测试成员函数
    cout << *vec.begin() << endl;
    //cout << *vec.end() << endl; // 原则上vec.end()不能被访问
    cout << vec.front() << endl;
    cout << vec.back() << endl;
    cout << *vec.rbegin() << endl;
    // cout << *vec.rend() << endl;      //vec.rend()不能被访问
    cout << vec[20] << endl;
    cout << vec.size() << endl;
    cout << vec.capacity() << endl;
    cout << vec.max_size() << endl;
    cout << vec.empty() << endl;

    vec1.push_back('j');
    traverse(vec1);
    // 试一下遍历空数组
    vec1.pop_back();
    traverse(vec1);
    vec2.pop_back();
    traverse(vec2);

    vec.erase(vec.begin() + 5); // 参数为指针
    vec2.erase(vec2.begin() + 1, vec2.begin() + 4);

    // resize
    cout << vec.size() << endl;
    vec.resize(50, 'b');
    traverse(vec);
    vec.resize(10);
    traverse(vec);

    vec.clear();
    traverse(vec);

    return 0;
}