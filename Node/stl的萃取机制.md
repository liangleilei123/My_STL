## POD数据类型

[链接](https://cloud.tencent.com/developer/article/1814242)

**POD（Plain Old Data）指的是C++定义的和C相兼容的数据结构，通过二进制拷贝后能保持不变。**

- c语言定义的基本数据类型（比如：int、char、float、枚举、指针、数组和结构体等都是POD类型。）
- 对于类类型，可以用c++11的`is_pod<T>`函数判断。
- 当某一类或结构同时为普通和标准布局时，该类或结构为 POD（简单旧数据）类型。
  - 普通类型
    - 具有默认的 构造函数/拷贝构造函数/复制赋值运算符/析构函数。
    - 没有具有非默认 构造/拷贝/赋值/析构函数的基类。
    - 没有具有非默认 构造/拷贝/赋值/析构函数的数据成员。
  - 标准布局类型
    - 所有非静态数据均为标准布局类型
    - 所有基类均为标准布局类型
    - 所有非静态成员具有相同的访问权限
    - 没有虚函数
    - 没有虚基类
    - 类中的第一个非静态成员与其任何基类的类型不同
    - 要么所有基类都没有非静态成员，要么最下层的子类没有非静态成员且最多只有基类有非静态数据成员（总之继承树中最多只能有一个类有非静态数据成员）



## 模板元编程

模板元编程的核心思想是**通过模板参数和参数类型推导进行编译时期的计算**。可以通过模版实例化、模版特化、模版元函数等技术实现。

好处是可以使STL中的容器适应不同的数据类型，并在编译期对代码进行类型检查和优化，避免运行时的复杂判断。



## SFINAE

SFINAE（Substitution Failure Is Not An Error）是C++中的一种编译时机制，用于**在模板的实例化过程中进行类型推导和对函数重载进行解析**。

通过使用SFINAE机制，可以在**编译时根据类型的特征进行条件判断**、类型推导和函数选择，从而实现更加通用和可扩展的模板库和应用程序。

## 萃取机制

简单一句话总结：stl定义了两个空类用于区分内置数据类型和用户自定义数据类型，在内存配置器在析构内存时，把这两个空类作为参数重载析构相关函数，并安排一步中间函数萃取类型，调用相应的函数进行处理。

- 优点一：空间不占用内存空间
- 优点二：内置数据类型的析构函数不需要执行操作，可以大大提高效率。



工作原理：

定义表示真假的两个空类：`struct __ture_type{};`和`struct __false_type{}`。空类不占用任何空间。

定义时：

定义一个模版类`__type_traits`。默认所有类型都是`__false_type`。然后对所有的内置类型进行模版特化：设定为`__true_type`。

<img src="stl的萃取机制.assets/截屏2023-04-22 21.50.56.png" alt="截屏2023-04-22 21.50.56" style="zoom:50%;" />

使用时，只需使用`__type_traits<T>::`调用相应的模版的成员还是即可返回`__false_type`和`__true_type`，从而实现在传入类型的判断。然后调用对用的模版特化即可实现对类型的萃取。

<img src="stl的萃取机制.assets/截屏2023-04-22 21.55.58.png" alt="截屏2023-04-22 21.55.58" style="zoom:50%;" />



​	SGI提供了一种机制，`__type_traits`。

- iterator_traits：负责萃取迭代器的特性
- __type_traits：负责萃取类型的特性，包括：
  - 该类型是否具备non-trivial default ctor
  - 该类型是否具备non-trivial copy ctor
  - 该类型是否具备non-trivial assignment operator
  - 该类型是否具备non-trivial dtor
  - 该类型是否是POD类型

通过使用__type_traits，在对某个类型进行构造、析构、拷贝、赋值等操作时，就可以采用最有效率的措施。这对于大规模而操作频繁的容器，有着显著的效率提升

萃取类型的特性时，我们希望得到一个”真“或”假“（以便决定采取什么策略），但其结果不应该只是个bool值，应该是个有着真/假性质的”对象”，因为我们希望利用响应的结果来进行参数推导，而编译器只有面对class object形式的参数，才会做**参数推导**，所以萃取类型的特性时，返回true_type或false_type：

```c++
struct __true_type { };
struct __false_type { };
```

**参数推导：**模板参数推导是指编译器根据函数调用时传递的参数类型推导出模板参数类型。

然后利用C++的模板参数特化，把内置数据类型返回`__true_type`。

**优点：**

- 在c++中，空结构体的大小为0；两个空白class没有任何成员，不占用任何内存空间。且能表示类型的真假性质。（使用bool类型最少占用一个字节的内存空间）
- C风格结构体也是POD，这样就不用管构造和析构
  - 结构体的创建和销毁都需要手动完成。常见的结构体创建和销毁方法包括通过结构体变量直接赋值、通过结构体指针动态分配内存和在函数中创建结构体实例并返回。
  - 和常规变量一样，当出了作用域之后，结构体的变量和数组会被自动销毁，但是，注意，结构体指针指向的空间，和结构体成员变量中的指针指向的空间，都不会被自动释放，需要手动释放。

```c++
/*
 * @Descripttion:
 * @version: 1.0
 * @Author: liangleilei
 * @Date: 2023-03-27 08:00:11
 * @LastEditTime: 2023-03-27 12:06:27
 */

// 命名规范，双横杠表示是STL内部所用的东西，不在STL标准规范之内
struct __true_type
{
};
struct __false_type
{
};


template <typename T>
struct __type_traits
{
    typedef __true_type this_dummy_member_must_be_first;
    /*不要移除这个成员。它通知“有能力将__type_traits特化”的编译器说，我们现在所看到的这个__type_traits template是特殊的。
    这是为了确保万一编译器也使用一个名为__type_traits 而其实与此处定义并无任何关联的template时，这个类模板能正常工作*/

    /*为了安全起见，所有型别都默认为__false_type,再针对每一个标量型别进行适当的模版特化*/
    typedef __false_type has_trivial_default_constructor; // 是否无需考虑默认构造器
    typedef __false_type has_trivial_copy_constructor;    // 是否无需考虑拷贝构造器
    typedef __false_type has_trivial_assignment_operator; // 是否无需考虑赋值操作符
    typedef __false_type has_trivial_destructor;          // 是否无需考虑析构器，
    typedef __false_type is_POD_type;                     // 是否是POD类型
};

// 对内置类型进行模版特化,内置类型是POD类型
/*c++的基本类型包括包括 bool、char、signed char、unsigned char、short、unsigned short、
int、unsigned int、long、unsigned long、long long 和 unsigned long long。*/
/*浮点型（floating-point types）：包括 float、double 和 long double。*/
/*指针类型*/

template <>
struct __type_traits<bool>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<char>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<signed char>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<unsigned char>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<short>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<unsigned short>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<int>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<unsigned int>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<long>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<unsigned long>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<long long>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<unsigned long long>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<float>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<double>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

template <>
struct __type_traits<long double>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

// 对于原生指针类型进行偏特化
template <typename T>
struct __type_traits<T *>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};

```



