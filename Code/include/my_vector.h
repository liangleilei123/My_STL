//
// Created by 梁磊磊 on 2023/4/24.
//



#ifndef IMPLTMENT_STL_MY_VECTOR_H
#define IMPLTMENT_STL_MY_VECTOR_H

#include "stl_alloc.h"
#include "cstddef"
#include "stl_uninitialized.h"
#include "stl_algobase.h"
#include "type_traits.h"

namespace mystl {
    template<class T, class Alloc=alloc>        //alloc直接调用第二级空间配置器，当要分配的内存大于128kb时，调用第一级配置器
    class vector {
    public:
        /**                       vector的嵌套类型定义                        **/
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type *iterator;           //定义vector类的迭代器
        typedef const value_type *const_iterator;
        typedef value_type &reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;


        typedef const value_type *const_pointer;
        typedef const value_type &const_reference;


    protected:
        /**                        设置空间配置器                              **/
        //这个是空间配置器的API
        typedef simple_alloc<value_type, Alloc> data_allocator;             //确定了分配空间的类型

        //定义迭代器
        iterator start;             //表示目前使用的空间的头
        iterator finish;            //表示目前使用的空间的尾
        iterator end_of_storage;     //表示目前可用空间的尾


        //配置空间并填满内容
        iterator allocate_and_copy(const_iterator first, const_iterator last, iterator result) {
            size_type len = size_type(last - first);
            result = data_allocator::allocate(len);
            uninitialized_copy(first, last, result);
            return result;
        }

        iterator allocate_and_copy(size_type len, const_iterator first, const_iterator last) {

            iterator result = data_allocator::allocate(len);
            uninitialized_copy(first, last, result);
            return result;
        }


        iterator allocate_and_fill(size_type n, const_reference x) {
            iterator result = data_allocator::allocate(n);    //先分配空间，已知分配空间的类型，只需传递要分配的个数即可
            uninitialized_fill_n(result, n, x);
            return result;
        }

        void insert_aux(iterator position, const_reference x);

        //销毁容器所占的空间
        void deallocate() {
            if (start) {
                data_allocator::deallocate(start, end_of_storage - start);
            }
        }

        void deallocate(T *p, size_type n) {
            data_allocator::deallocate(p, n);
        }


        void fill_initialize(size_type n, const_reference value) {
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }

        void copy_initialize(const vector<T, Alloc> &value) {
            start = allocate_and_copy(value.begin(), value.end(), start);
            finish = start + value.size();
            end_of_storage = finish;
        }

    public:

        /**                         构造                    **/
        //默认构造
        vector() : start(0), finish(0), end_of_storage(0) {

        }

        //有参构造,会调用T类型的默认构造
        explicit vector(size_type n) {          //explicit是一个C++关键字，用于修饰构造函数，指示编译器不要对 参数 自动执行隐式类型转换
            fill_initialize(n, value_type());
        }

        vector(size_type n, const_reference value) {
            fill_initialize(n, value);
        }

        vector(int n, const_reference value) {
            fill_initialize(n, value);
        }

        vector(long n, const_reference value) {
            fill_initialize(n, value);
        }

        vector(const value_type *first, const value_type *last) {
            start = allocate_and_copy(first, last, start);
            finish = start + (last - first);
            end_of_storage = finish;
        }


        /**                         拷贝构造                    **/
        vector(const vector<T, Alloc> &value) {
            copy_initialize(value);
        }


        /**                         析构                    **/
        ~vector() {
            destroy(start, finish);
            deallocate();           //释放空间
        }

        void reserve(size_type __n) {
            if (capacity() < __n) {
                const size_type __old_size = size();
                //iterator result = data_allocator::allocate(__n);
                iterator __tmp = allocate_and_copy(__n, start, finish);
                destroy(start, finish);
                deallocate(start, end_of_storage - start);
                start = __tmp;
                finish = __tmp + __old_size;
                end_of_storage = start + __n;
            }
        }

        /**                         常用接口                    **/
        /**                         首尾                    **/
        iterator begin() { return start; }

        const_iterator begin() const { return start; }

        iterator end() { return finish; }

        const_iterator end() const { return finish; }

        reference front() { return *begin(); }

        const_reference front() const { return *begin(); }

        reference back() { return *(end() - 1); }

        const_reference back() const { return *(end() - 1); }

        /**                         大小                    **/
        //容量
        size_type capacity() const { return end_of_storage - begin(); }

        size_type size() const { return size_type(end() - begin()); }

        bool empty() const { return begin() == end(); }


        /**                         增删改查                    **/
        //随机访问
        reference operator[](size_type n) {
            return *(begin() + n);
        }

        const_reference operator[](size_type n) const {
            return *(begin() + n);
        }

        //增
        //从position开始，插入n个元素
        //返回的是新插入元素的第一个
        iterator insert(iterator position, const_reference x) {
            size_type n = position - begin();
            if (finish != end_of_storage && position == end()) {
                construct(finish, x);
                ++finish;
            } else {
                insert_aux(position, x);
            }
            //return position;      //当position是end()可能出错
            return begin() + n;
        }

        iterator insert(iterator position, size_type n, const_reference x);

        void insert(iterator position, const_iterator first, const_iterator last);

        void push_back(const_reference value) {
            if (finish != end_of_storage) {
                construct(finish, value);
                ++finish;
            } else {
                insert_aux(end(), value);
            }
        }

        //删
        void pop_back() {
            --finish;
            destroy(finish);
        }

        //返回指向被删除元素之后的第一个元素的迭代器，方便在删除一个或多个元素后，遍历容器中的剩余元素
        iterator erase(iterator position) {      //清除某个位置上的元素
            if (position + 1 != end()) {
                //如果position指的不是最后一个对象，需要把后面的全部向前移动一个位置
                uninitialized_copy(position + 1, finish, position);
            }
            --finish;
            destroy(finish);
            return position;
        }

        //这里的移除是左闭右开
        iterator erase(iterator first, iterator last) {
            //这里要不要考虑last越界的问题
            //这个复制是左闭右开的
            iterator temp = uninitialized_copy(last, finish, first);
            destroy(temp, finish);
            finish = finish - (last - first);
            return first;
        }

        void resize(size_type new_size, const_reference x) {
            if (new_size < size()) {
                erase(begin() + new_size, end());
            } else
                insert(end(), new_size - size(), x);
        }

        void resize(size_type new_size) {
            resize(new_size, value_type());
        }

        void clear() {
            erase(begin(), end());
        }

        void swap(vector<T, Alloc> &x) {
            mystl::swap(start, x.start);
            mystl::swap(finish, x.finish);
            mystl::swap(end_of_storage, x.end_of_storage);
        }


    };

    template<class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const_reference x) {
        if (finish != end_of_storage) {         //这是随机插入用的
            construct(finish, *(finish - 1));

            ++finish;
            T x_copy = x;
            copy_backward(position, finish - 2, finish - 1);
            *position = x_copy;
        } else {   //这个是push_back用的
            const size_type old_size = size();
            const size_type len = old_size != 0 ? old_size * 2 : 1;

            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;

            //把旧容器的对象都复制到新分配的空间
            try {
                new_finish = uninitialized_copy(start, position, new_start);
                construct(new_finish, x);    //把对象x插入新分配的空间
                ++new_finish;
                new_finish = uninitialized_copy(position, finish, new_finish);    //这行代码在push_back中没用，应该是在insert中使用的
            }
            catch (...) {       //错误处理
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }

            //析构并释放原vector
            destroy(begin(), end());
            deallocate();

            //调整迭代器，指向新vector
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }

    //从position开始，插入n个元素x
    template<class T, class Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, size_type n, const_reference x) {
        if (n != 0) {     //先判断插入元素个数的合法性
            if (size_type(capacity() - size()) >= n) {      //再判断当然剩余空间是否满足n个元素的空间需求
                value_type x_copy = x;
                const size_type elems_after = finish - position;
                //            const size_type elems_after = distance(position, end());
                iterator old_finish = finish;
                if (elems_after > n) {      //插入点之后的现有元素个数大于新增元素个数
                    //                uninitialized_copy(position, finish, position + n);  不能这么搞，在从前往后复制的过程中会覆盖掉原来的元素导致出错
                    uninitialized_copy(finish - n, finish, finish);     //把finish之前的n个元素移动到finish之后
                    finish = old_finish + n;                            //更新finish
                    copy_backward(position, old_finish - n,
                                  old_finish);                 //再把position到old_finish-n范围内的元素从后往前依次复制到old_finish范围内
                    fill(position, position + n, x_copy);   //把填充到position,positon +n范围内，
                    /*这样做的好处是移动现有元素的过程中不会相互覆盖*/
                } else {
                    //插入点之后的现有元素个数小于等于先增元素个数
                    uninitialized_fill_n(finish, n - elems_after, x_copy);      //先把大于elems的个数插入到finish之后
                    finish = finish + n - elems_after;                                      //更新finish
                    uninitialized_copy(position, old_finish,
                                       finish);                       //再把position,finish范围内的元素填充到容器尾部
                    finish += elems_after;                                                  //更新finish
                    uninitialized_fill(position, old_finish, x_copy);           //最后把x填充到position,old-finish中

                }
            } else {
                //备用空间小于"新增元素个数"，需要配置额外的内存
                //首先决定新长度：旧长度的两倍或者旧长度+新增元素的个数
                const size_type old_size = size();
                const size_type len = old_size + max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try {
                    //以下首先将旧vector的插入点之前的元素复制到新空间
                    new_finish = uninitialized_copy(start, position, new_start);
                    //把要插入的元素插入新分配的空间
                    new_finish = uninitialized_fill_n(new_finish, n, x);
                    //把插入点之后的元素插入新空间
                    new_finish = uninitialized_copy(position, finish, new_finish);
                }
                    //#ifdef __STL_USE_EXCEPTIONS
                catch (...) {
                    //如果有异常发生，就销毁新分配的空间并报错
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;
                }
                //#endif
                //以下是清除并释放旧的vector
                destroy(start, finish);
                deallocate();

                start = new_start;
                finish = new_finish;
                end_of_storage = new_finish + len;

            }
        }
        return position;

    }


    template<class T, class Alloc>
    void vector<T, Alloc>::insert(iterator position, const_iterator first, const_iterator last) {
        if (first != last) {
            size_type n = 0;
            n = distance(first, last);               //在stl_iterator.h中
            if (size_type(end_of_storage - finish) >= n) {
                const size_type elems_after = finish - position;
                iterator old_finish;
                if (elems_after > n) {
                    uninitialized_copy(finish - n, finish, finish);
                    finish = finish + n;
                    copy_backward(position, old_finish - n, old_finish);
                    copy(first, last, position);
                } else {
                    uninitialized_copy(first + elems_after, last, finish);
                    finish = finish + n - elems_after;
                    uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    copy(first, first + n, position);
                }
            } else {
                const size_type old_size = size();
                const size_type len = old_size + max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;

                try {
                    new_finish = uninitialized_copy(start, position, new_start);
                    new_finish = uninitialized_copy(first, last, new_finish);
                    new_finish = uninitialized_copy(position, finish, new_finish);
                }
                catch (...) {
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;
                }
                destroy(start, finish);
                deallocate();

                start = new_start;
                finish = new_finish;
                end_of_storage = start + len;
            }

        }
    }

}


#endif //IMPLTMENT_STL_MY_VECTOR_H
