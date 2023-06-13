## 问题

### explicit关键字

`explicit`是一个C++关键字，用于修饰构造函数，指示编译器不要自动执行隐式类型转换。通常情况下，当我们定义一个只有一个参数的构造函数时，该构造函数可以被用于执行隐式类型转换。这种隐式类型转换可能会导致错误或者不直观的行为，因此我们希望显式地指示编译器禁止这种隐式类型转换。

因此，使用`explicit`关键字可以防止编译器在以下情况下执行隐式类型转换：

1. 对象初始化时，如果构造函数只有一个参数，则必须显式地指定类型转换。例如：

```c++
class MyClass {
public:
    explicit MyClass(int x) {
        // ...
    }
};

void foo(MyClass obj) {
    // ...
}

int main() {
    foo(10);  // 编译错误：不能隐式地将int类型转换为MyClass类型
    foo(MyClass(10));  // 正确：需要显式地创建MyClass对象
    return 0;
}
```

2.在赋值语句中，如果构造函数只有一个参数，则必须使用显式类型转换。例如：

```c++
class MyOtherClass {
public:
    explicit MyOtherClass(int x) {
        // ...
    }
};

void bar() {
    MyOtherClass obj = 10;  // 编译错误：不能隐式地将int类型转换为MyOtherClass类型
    MyOtherClass obj2(10);  // 正确：需要显式地调用构造函数
    MyOtherClass obj3 = MyOtherClass(10);  // 正确：需要显式地创建MyOtherClass对象
    MyOtherClass obj4 = static_cast<MyOtherClass>(10);  // 正确：需要显式地进行类型转换
}
```

总之，`explicit`关键字可以帮助我们更好地控制类型转换，避免一些隐式类型转换可能导致的问题。

### c++中vector的erase函数为什么要返回迭代器类型

C++中，`std::vector`的`erase()`函数的返回值为指向被删除元素之后的第一个元素的迭代器。这是为了方便在删除一个或多个元素后，能够方便地遍历容器中的剩余元素。

假设我们有一个`vector`对象`v`，其中包含一些元素。我们想删除其中的一些元素，并遍历剩余的元素。我们可以使用`erase()`函数来删除元素，然后使用返回的迭代器来遍历剩余的元素。例如：

```c++
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    
    // 删除前三个元素
    auto it = v.erase(v.begin(), v.begin() + 3);
    
    // 遍历剩余的元素
    for (; it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
```

在这个示例中，我们首先使用`erase()`函数删除了前三个元素，并将返回的迭代器存储在`it`变量中。然后，我们使用`it`变量来遍历剩余的元素，并打印它们的值。

因此，`erase()`函数返回迭代器类型是为了方便在删除元素后遍历剩余的元素。



## insert函数

功能描述：从position开始，加入n个元素x

- 先判断n的合法性，`n!=0`
- 备用空间(`elems_after = end_of_storge-finish`)是否够用
  - 够用，直接插入。
    - 判断插入点后到结尾的元素个数（`finish - position`）与n的大小。