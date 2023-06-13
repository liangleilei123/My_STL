/*
 * @Descripttion:
 * @version: 1.0
 * @Author: liangleilei
 * @Date: 2023-03-27 12:48:08
 * @LastEditTime: 2023-03-28 01:35:06
 */

 #include <iostream>
 #include "../include/type_traits.h"

 template <typename T>
 class vector
 {
 protected:
     T *start;          // 表示当前空间的头
     T *finish;         // 表示当前使用空间的尾
     T *end_of_storage; // 表示当前可用空间的尾
 public:
     vector(int n)
     {

         std::cout << "vector的有参构造vector(n)" << std::endl;

         start = new T[n]; // 先创建空间
         finish = start;
         end_of_storage = start + n;
         for (int i = 0; i < n; ++i) // 初始值设为
         {
             if (__type_bool<typename __type_traits<T>::has_trivial_default_constructor>::sign)
                 start[i] = T();
             else
                 new (start + i) T(); // 调用T类型的默认构造
         }
     }

     vector(int n, const T &value)
     {
         start = new T[n];
         finish = start + n;
         end_of_storage = finish;
         for (int i = 0; i < n; i++)
         {
             if (__type_bool<typename __type_traits<T>::has_trivial_copy_constructor>::sign)
             {
                 start[i] = value;
             }
             else
             {
                 new (start + i) T(value); // 调用T类的构造函数
             }
         }
     }

     T *begin() const
     {
         return start;
     }

     T &operator[](int n) // 返回引用，便于修改
     {                    // 不考虑n越界的情况
         return *(begin() + n);
     }

     T &operator[](int n) const // 返回引用，便于修改
     {                          // 不考虑n越界的情况
         return *(begin() + n);
     }
 };

 int main()
 {
     // include<char> vec(10, 'a');
     // include<int> vec(10, 6);
     // include<std::string> vec(10, "addd");
     vector<int> vec(10);
     for (int i = 0; i < 10; ++i)
     {
         std::cout << vec[i] << ' ';
     }
     std::cout << std::endl;

     return 0;
 }
