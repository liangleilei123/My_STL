/*
 * @Descripttion:My_vector.h
 * @version: 1.0
 * @Author: liangleilei
 * @Date: 2023-03-15 12:24:41
 * @LastEditTime: 2023-03-28 01:48:37
 */
#ifndef IMPLTMENT_STL_INCLUDE_MY_VECTOR_H_
#define IMPLTMENT_STL_INCLUDE_MY_VECTOR_H_

#include "allocator.h"
#include "stl_constructor.h"
#include "type_traits.h"
#include "stl_alloc.h"

template<typename T, class Alloc = alloc>
class vector {
public:
    /***************嵌套类型名****************/
    // 起别名是为了增加代码的可读性

    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;

    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    /***************嵌套类型名完*******************/

protected:
    // 用T* 类型表示vector的头尾和可用空间的最大值
    pointer start;          // 表示当前空间的头
    pointer finish;         // 表示当前使用空间的尾
    pointer end_of_storage; // 表示当前可用空间的尾
    typedef allocator<T> data_allocator;

    // 辅助函数，作用：用于vector空间不够时扩容
    // 可用空间为0时，申请更大的空间，并把元素都复制过去
    void allocate_and_copy() {
        // std::cout << "调用allocate_and_copy" << std::endl;

        size_type len =
                (capacity() != 0 ? capacity() * 2 : 1); // 以容量的2倍为规则扩容
        // pointer temp = (pointer) ::operator new(len * sizeof(T)); //
        // 创建更大的空间,temp指向可用空间的头
        pointer temp = data_allocator::allocate(len);
        for (size_type i = 0; i < size(); ++i) {
            // typename 的作用是把后面的内容当作一个类型
            if (__type_bool<
                    typename __type_traits<T>::has_trivial_copy_constructor>::sign)
                temp[i] = start[i];
            else {
                // new(temp + i) T(start[i]);
                // 在指定空间上构造对象，用的是placement new
                construct(temp + i, start[i]);
                destroy(start + i); // 析构原对象
            }

            // placement new 在已分配的特定内存创建对象
            // 这个语法的作用是在 temp+i 这个位置上构造一个类型为 T
            // 的对象，并且该对象的初始值是 start[i]
            /*在手动分配内存并构造对象时，我们需要使用 placement new 语法来构造对象。
            placement new
            语法相当于在已经分配的内存中构造对象，它不会再次分配内存，而是直接在指定的内存地址上构造对象。
            在使用 placement new 时，需要传递一个指向已经分配内存的指针，并且不能使用
            delete 运算符来释放这块内存，
            而是需要先手动调用对象的析构函数，再释放内存*/
        }

        end_of_storage = temp + len;
        finish = temp + size();

        // delete start; // 回收旧数组的头节点，实际上整个旧数组都被释放了
        //        if (!__type_bool<typename
        //        __type_traits<T>::has_trivial_destructor>::sign) {
        //            start->~T(); //析构函数直接销毁整个旧数组并回收内存
        //        }
        //        delete[] start;

        // 通过空间配置器回收空间
        data_allocator::deallocate(start);
        start = temp;
    }

public:
    // 构造函数:这里用成员初始化列表进行初始化
    vector()
            : start(nullptr), finish(nullptr), end_of_storage(nullptr) { // 默认构造
        // std::cout << "vector的默认构造" << std::endl;
    }

    // 有参构造:初始化vector的长度为n
    vector(size_type n) {

        // std::cout << "vector的有参构造vector(n)" << std::endl;

        // start = (pointer) ::operator new(n * sizeof(T)); //
        // 创建完空间后强制类型转换
        start = data_allocator::allocate(n);
        finish = start + n;
        end_of_storage = start + n;
        // 进行默认初始化
        for (size_type i = 0; i < n; ++i) {
            if (__type_bool<
                    typename __type_traits<T>::has_trivial_default_constructor>::sign)
                start[i] = 0;
            else
                // new(start + i) T(); // 调用T类型的默认构造,placement new
                construct(start + i);
        }
    }

    // 有参构造:初始化vector的长度为n，并将这些位置上的数据全部赋值为value
    vector(size_type n, const T &value) {

        // std::cout << "vector的有参构造vector(n,value)" << std::endl;

        // start = (pointer) ::operator new[](n * sizeof(T));   // 先创建空间
        start = data_allocator::allocate(n);
        finish =
                start +
                n; // 因为对数组的数组是从0开始存放的，start+n表示的是使用空间的下一个位置
        end_of_storage = start + n;
        for (size_type i = 0; i < n; ++i) // 初始值设为0
        {
            if (__type_bool<
                    typename __type_traits<T>::has_trivial_copy_constructor>::sign)
                start[i] = value;
            else
                // new(start + i) T(value);
                construct(start + i, value);
        }
    }

    // 拷贝构造
    vector(const vector &another) {
        // std::cout << "vector的拷贝构造函数" << std::endl;

        // 创建一个新空间存放another的数据
        size_type len = another.size();
        // start =(pointer) ::operator new[](len * sizeof(T));      //函数原型：void
        // *operator new[](size_t);    size_t是存储数组中所有元素所需的空间
        start = data_allocator::allocate(len);
        for (size_type i = 0; i < len; ++i) {
            // start[i] = *(another.begin() + i); //const对象不能调用非const成员函数
            // start[i] = another[i];
            construct(start + i, another[i]);
        }
        finish = start + len;
        end_of_storage = finish;
    }

    // 析构函数
    ~vector() {
        std::cout << "vector的析构函数" << std::endl;
        if (!__type_bool<typename __type_traits<T>::has_trivial_destructor>::sign) {
            for (int i = size() - 1; i >= 0; --i) {
                //(start + i)->~T();      //调用析构函数会销毁对象，但不会释放内存
                destroy(start + i);
            }
        }
        //::operator delete[](start);
        data_allocator::deallocate(start);
    }

    // 常用接口
    /**********************首尾******************************/

    // 给出vcetor当前使用空间的起始位置
    pointer begin() const { return start; }

    // 给出vector当前使用空间的下一个位置
    pointer end() const { return finish; }

    // 逆序遍历时，使用空间的起始位置
    pointer rbegin() { return end() - 1; }

    // 逆序遍历时，使用空间的结束位置
    pointer rend() { return begin() - 1; }

    /**********************随机访问******************************/

    // vector中第一个数据
    T &front() {
        return *begin(); // 尽量避免直接使用和暴露成员变量，保证类的封装性
    }

    // vector中最后一个数据
    T &back() { return *(end() - 1); }

    T &operator[](size_type n) // 返回引用，便于修改
    {                          // 不考虑n越界的情况
        return *(begin() + n);
    }

    T &operator[](size_type n) const // 返回引用，便于修改
    {                                // 不考虑n越界的情况
        return *(begin() + n);
    }

    /**********************大小******************************/

    // 此处const的作用：
    // 一、将成员函数声明为const,表明该函数不会对对象进行修改
    // 二、可以允许在const对象上调用该函数
    // vector中当前有多少个数据
    size_type size() const { return finish - start; }

    // 容量:vector中当前最多能存放多少个数据
    size_type capacity() const { return int(end_of_storage - begin()); }

    size_type max_size() const { return end_of_storage - start; }

    // vector当前是否为空
    bool empty() const { return begin() == end(); }

    /**********************  增删改  ******************************/
    // 在vector的末尾插入一个数据
    void push_back(const T &x) {
        // 容器内空间不够，就扩容
        if (finish == end_of_storage) { // 如果长度为零，初始空间设为1
            // std::cout << "capacity()的值为：" << capacity() << std::endl;
            allocate_and_copy();
        }
        if (__type_bool<
                typename __type_traits<T>::has_trivial_copy_constructor>::sign)
            *finish = x;
        else {
            // new(finish) T(x); // placement new
            //*end() = x;       //ERROR:因为end()是静态成员函数，不能修改对象
            construct(finish, x);
        }
        ++finish;
    }

    // 在position位置插入x
    // 区分POD类型和非POD类型
    void insert(size_type position, const_reference x) {
        if (position >= size()) {
            std::cerr << "Memory access out of bounds" << std::endl;
            return;
        }
        if (size() == capacity()) {
            allocate_and_copy();
        }

        // 非POD类型，先把原来的往后移，再构造析构
        if (!__type_bool<typename __type_traits<T>::is_POD_type>::sign) {
            for (size_type i = size(); i > position; --i) {
                construct(start + i, *(start + i - 1));
                destroy(start + i - 1);
            }
            construct(start + position, x);
        } else {
            for (size_type i = size(); i > position; --i) {
                start[i] = start[i - 1];
            }
            start[position] = x;
        }

    }

    void insert(size_type position, size_type n, const_reference x) {
        if (position >= size()) {
            std::cerr << "Memory access out of bounds" << std::endl;
            return;
        }
        while (size() >= capacity()) {
            allocate_and_copy();
        }

        // 非POD类型，先把原来的往后移，再构造析构
        if (!__type_bool<typename __type_traits<T>::is_POD_type>::sign) {
            for (size_type i = size() + n; i > position + n; --i) {
                construct(start + i, *(start + i - n));
                destroy(start + i - 1);
            }
            for (int count = 0; count < n; ++count) {
                construct(start + position + count, x);
            }

        } else {
            for (size_type i = size() + n; i > position + n; --i) {
                start[i] = start[i - n - 1];
            }
            for (int count = 0; count < n; ++count) {
                start[position + count] = x;
            }

        }
    }

    // 在vector的末尾删除一个数据
    void pop_back() {
        // if (__type_bool<typename
        // __type_traits<T>::has_trivial_destructor>::sign)
        --finish;
        if (!__type_bool<
                typename __type_traits<T>::has_trivial_destructor>::sign) {
            // finish->~T(); //
            // 在finish指向的对象上调用析构函数,应该是先--finish再析构吧
            destroy(finish);
        }
    }

    // 删除position所指向的数据
    void erase(pointer position) {
        // 把指针后面的全部前移
        for (pointer i = position; (i + 1) != end(); ++i) {
            if (__type_bool<
                    typename __type_traits<T>::has_trivial_destructor>::sign)
                *i = *(i + 1);
            else {
                // i->~T();
                // new(i) T(*(i + 1));
                destroy(i);
                construct(i, *(i + 1));
                destroy(i + 1);
            }
        }
        --finish;
    }

    // 删除从first到last这一段数据，不释放内存
    void erase(pointer first, pointer last) {
        difference_type diff = last - first;
        size_type count = 0; // 计数器
        for (
                pointer
                        i = last;
                i != end(); ++i,
                        ++count) // 如果i等于end(),就不用移动后面的数据了，直接更新finish就行
        {
            if (__type_bool<
                    typename __type_traits<T>::has_trivial_destructor>::sign)
                *(first + count) =
                        *(first + diff +
                          count); // 这里等号右边必须是first +
                // diff,刚好是last的下一个位置,计数器从0开始遍历正好
            else {
                //                (first + count)->~T();
                //                new(first + count) T(*(first + diff + count));
                //                (first + diff + count)->~T();
                destroy(first + count);
                construct(first + count, *(first + diff + count));
                destroy(first + diff + count);
            }
        }
        finish = finish - diff; // 指针向前移动
    }

    // 调整vetor的容量，如果调整后的容量大于调整前，则用数据x填充空余部分；
    // 如果调整后容量小于调整前，则只保留前new_size位数据
    void resize(size_type new_size, const T &x) {
        if (new_size < size()) {
            erase(begin() + new_size, end());
        } else {
            // std::cout << "capacity()的值为：" << capacity() << std::endl;
            //  考虑需要扩容的情况
            while (new_size > capacity()) { // 数组是从0开始存储的，所以如果new_size
                // = capacity()的情况刚好能存放

                allocate_and_copy();
                // std::cout << "capacity()的值为：" << capacity() << std::endl;
            }
            for (size_type i = 0; i < new_size - size(); ++i) {
                if (__type_bool<typename __type_traits<
                        T>::has_trivial_copy_constructor>::sign)
                    *(finish + i) = x;
                else {
                    // new(finish + i) T(x);
                    construct(finish + i, x);
                }
            }
            finish += new_size - size();
        }
    }

    // 调整后的容量大于调整前，则用默认数据填充空余部分,其余上同
    void resize(size_type new_size) {
        // 不管是不是POD,都要用默认构造
        resize(new_size, 0);
    }

    // 清空vector中所有数据
    void clear() { erase(begin(), end()); }
};

#endif // #ifndef IMPLTMENT_STL_INCLUDE_MY_VECTOR_H_