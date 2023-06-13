## 实现list的思路

1. 实现节点
   1. 基类：包含指向`prev`和`next`的指针
   2. 子类：包含一个数据成员用来存储节点的内容
2. 实现list迭代器
   1. 基类中定义节点基类的指针，使用`prev`和`next`指针实现迭代器的前进和后退操作。
   2. 子类中重载`*`和`->`用于读取节点子类中保存的数据成员。重载`++`,`--`实现迭代器的移动
3. 实现list
   1. 





## 异常处理

```c++
#define MYSTL_DEBUG(expr)\
    assert(expr)
```

这是一个用于 C 或 C++ 程序的预处理器宏，用于定义调试工具。当该宏在代码中使用时，它会评估表达式 `expr` 并使用 C 标准库中的 `assert` 宏检查其是否为真。如果 `expr` 是 false，则 `assert` 将导致程序终止并打印一个错误消息，指示断言失败的文件和行号。

使用这个宏可以帮助捕捉编程错误，比如无效的输入值、意外的行为或其他可能导致程序崩溃或产生不正确结果的问题。通过在程序中自由使用断言，开发人员可以确保关键的假设始终为真，并在开发过程的早期捕捉错误。



## 可变参数模版

`template<class ...Args>
node_ptr create_node(Args &&...args);`

函数模板的参数列表中使用了可变参数模板，即 `class ...Args`，表示该函数可以接受任意数量和类型的参数。

`Args &&...args` 是一个带有可变参数模板的模板函数参数列表，表示该函数可以接受任意数量和类型的参数。

其中，`Args` 是一个模板参数包，它可以扩展成任意数量的模板参数，例如 `Args<T1, T2, ..., TN>`。`&&` 表示参数是右值引用类型，即可以接受左值或右值。`...args` 表示函数参数列表中的可变参数，它可以接受任意数量的参数，且这些参数可以是任意类型。



## `std::enable_if`

[原文](https://en.cppreference.com/w/cpp/types/enable_if)

```c++
template<bool, typename _Tp = void>
  struct enable_if
  { };

// Partial specialization for true.
template<typename _Tp>
  struct enable_if<true, _Tp>		//第一个模板参数为true
  { typedef _Tp type; };			//type才有定义，否则使用默认模板参数会产生编译错误。
```

**`enable_if`允许在编译时根据某些条件启用或禁用函数模板或类模板**



## 类中的node_

在这段代码中，`node_`是一个指向当前`list`对象的尾节点的指针。尾节点是一个特殊的哨兵节点，它不存储任何元素值，只用于辅助实现双向循环链表的操作。

具体来说，`node_`指针指向的是尾节点的后继节点。尾节点的前驱节点是列表的最后一个实际元素节点，而尾节点的后继节点则是头节点。这样，通过尾节点的前驱节点和后继节点，就可以构成一个双向循环链表。

在`list`对象的构造函数中，会创建一个尾节点，并将其作为头节点的前驱节点和尾节点的后继节点。同时，将`node_`指针指向尾节点的后继节点，即头节点。这样，`node_`指针就指向了当前`list`对象的尾节点。

在`pop_back`函数中，首先需要找到尾节点的前一个节点，然后将其从链表中删除。因此，在函数中使用了`node_->prev`语句获取尾节点的前一个节点。



## lambda表达式

lambda 表达式定义了一个匿名函数，并且可以捕获一定范围内的变量。lambda 表达式的语法形式可简单归纳如下：

[ capture ] ( params ) opt -> ret { body; };

其中 capture 是捕获列表，params 是参数表，opt 是函数选项，ret 是返回值类型，body是函数体。

因此，一个完整的 lambda 表达式看起来像这样：

```
auto f = [](int a) -> int { return a + 1; };std::cout << f(1) << std::endl;  // 输出: 2
```

可以看到，上面通过一行代码定义了一个小小的功能闭包，用来将输入加 1 并返回。



## 前置++和后置++

在 C++ 中，前置++和后置++的行为是不同的，其中前置++返回一个引用，而后置++则不返回引用。

前置++是指在变量名前面使用++运算符，例如 `++i`，它将变量的值加1，并返回加1后的变量的引用。这意味着，我们可以在表达式中使用前置++的结果，并且这个结果将影响原来的变量。例如，我们可以这样写：

```
int i = 0;
int& ref = ++i; // ref is now a reference to i, which has the value 1
```

在这个例子中，`++i` 返回 `i` 的引用，这个引用存储在 `ref` 中，而 `i` 的值也被加1了。

后置++是指在变量名后面使用++运算符，例如 `i++`，它也将变量的值加1，但是返回的不是引用，而是变量的旧值。这意味着，我们不能在表达式中使用后置++的结果来修改原来的变量。例如，我们可以这样写：

```
int i = 0;
int j = i++; // j has the value 0, i has the value 1
```

在这个例子中，`i++` 返回 `i` 的旧值 0，并将 `i` 的值加1。这个旧值被赋给了 `j`，而 `i` 的值变成了 1。

需要注意的是，返回引用的前置++和返回值的后置++这种行为规定是固定的，不能更改。如果在后置++中返回引用，则可能会导致一些不可预测的行为，因为后置++返回的是一个临时对象的副本，而不是原对象的引用。



## list::sort

```c++
template<class T, class Alloc>
    void list<T, Alloc>::sort() {
        //链表为空或只有一个节点，就不需要排序
        if (node->next == node || link_type(node->next)->next == node) return;

        list<T, Alloc> carry;
        list<T, Alloc> counter[64];          //list类型的数组
        int fill = 0;
        while (!empty()) {
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty()) {
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill) ++fill;
        }
        for (int i = 1; i < fill; ++i) {
            counter[i].merge(counter[i - 1]);
        }
        swap(counter[fill - 1]);
    };
```

解析：函数中定义了一个链表`carry`和一个链表类型的数组`counter[64]`。实现排序的逻辑是每次把

1. 待排链表的头节点取出，插入`carry`
2. i = 0 
3. `while:`如果`i<fill && counter[i]`不为空，
   1. 把`carry`和`counter[i]`合并。（`carry`和`counter[0]`进行合并的时候永远是两个数的合并，方便排序。当`i > 0`时，也会有序合并，以保证`counter`中的链表都是有序的）
   2. 交换`carry`和`counter[i]`
   3. `i++`
4. 如果`counter[i]`为空，则放入`counter[i]`
5. 如果`i == fill`（说明`counter[0~fill-1]`保存的链表都是有序的）,`++fill`。
6. 直到待排序链表为空，依次合并`counter`的链表。合并调用的是`merge`，是有序合并。

