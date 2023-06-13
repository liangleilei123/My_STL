//
// Created by 梁磊磊 on 2023/5/15.
//

#ifndef IMPLTMENT_STL_MY_DEQUE_H
#define IMPLTMENT_STL_MY_DEQUE_H

#include "cstddef"
#include "stl_iterator.h"
#include "stl_alloc.h"
#include "stl_uninitialized.h"
#include "stl_algobase.h"


namespace mystl {
    //全局函数
    inline size_t __deque_buf_size(size_t n, size_t sz) {
        return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
    }


    template<class T, class Ref, class Ptr, size_t BufSiz>
    struct __deque_iterator {
        typedef __deque_iterator<T, T &, T *, BufSiz> iterator;
        typedef __deque_iterator<T, const T &, const T *, BufSiz> const_iterator;

        static size_t buffer_size() {
            return __deque_buf_size(BufSiz, sizeof(T));
        }     //一个缓冲区默认为512个字节，返回一个缓冲区能分配T类型数据的个数



        typedef random_access_iterator_tag iterator_category;               //deque的迭代器也是随机访问迭代器
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T **map_pointer;                    //中控器

        typedef __deque_iterator self;

        //一个迭代器所包含的指针成员，保持与容器的联结
        T *cur;                 //此迭代器指向缓冲区的现行元素
        T *first;               //此迭代器指向当前缓冲区的头
        T *last;                //此迭代器指向当前缓冲区的尾（含备用空间）
        map_pointer node;       //类型是T**,指向管控中心


        __deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

        __deque_iterator(const iterator &x) : cur(x.cur), first(x.first), last(x.last), node(x.node) {}

        reference operator*() const { return *cur; }

        pointer operator->() const { return &(operator*()); }

        difference_type operator-(const self &x) const {            //x是在this之后
            difference_type n = difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);

        }

        self &operator++() {
            ++cur;                                  //切换至下一个元素
            if (cur == last) {                        //如果已达所载缓冲区的尾端
                set_node(node + 1);        //就切换至缓冲区的下一个节点的第一个元素
                cur = first;
            }
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self &operator--() {
            if (cur == first) {
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }

        //以下是实现随机存取，迭代器可以直接跳跃n个距离。
        self &operator+=(difference_type n) {
            difference_type offset = n + (cur - first);               //直接计算+n是否大于一个缓冲区的大小
            if (offset >= 0 && offset < difference_type(buffer_size())) {
                //目标位置在同一缓冲区内
                cur += n;
            } else {
                //目标位置不在同一缓冲区内
                difference_type node_offset =
                        offset > 0 ? offset / difference_type(buffer_size()) :
                        -difference_type((-offset - 1) / buffer_size()) - 1;
                //切换至正确的节点（亦即缓冲区）
                set_node(node + node_offset);
                //切换至正确的元素
                cur = first + (offset - node_offset * difference_type(buffer_size()));
            }
            return *this;
        }

        self operator+(difference_type n) const {
            self tmp = *this;
            return tmp += n;
        }

        self &operator-=(difference_type n) {
            return *this += -n;
        }

        self operator-(difference_type n) const {
            self tmp = *this;
            return tmp -= n;
        }

        //实现随机存取，迭代器可以直接跳跃n个距离
        reference operator[](difference_type n) const {
            return *(*this + n);
        };

        bool operator==(const self &x) const { return cur == x.cur; }

        bool operator!=(const self &x) const { return !(*this == x); }

        bool operator<(const self &x) const {       //指针之间比较大小取决于他们所指的对象在内存中的大小
            return (node == x.node) ? (cur < x.cur) : (node < x.node);
        }

        bool operator>(const self &x) const {
            return x < *this;
        }

        bool operator<=(const self &x) const {
            return !(x < *this);
        }

        bool operator>=(const self &x) const {
            return !(*this < x);
        }

        //传入的中控器类型，这个函数通过中控器指针实现迭代器在不同节点之间跳转
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size());
        }
    };

    //deque的数据结构
    /**deque除了维护一个先前说过的指向map（中控器）的指针外，也维护start,finish两个迭代器，
     * 分别指向第一个缓冲区的第一个元素和最后缓冲区的最后一个元素（的下一个位置），c++中的范围是左闭右开的
     * 此外，必须记住目前map的大小。因为一旦map所提供的节点不足，就必须重新配置更大的一块map
     */

    template<class T, class Alloc=alloc, size_t BufSiz = 0>
    class deque {
    public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type &reference;
        typedef const value_type const_reference;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        typedef __deque_iterator<T, T &, T *, BufSiz> iterator;
        typedef __deque_iterator<T, const T &, const T *, BufSiz> const_iterator;


    protected:
        typedef pointer *map_pointer;       //T**类型的指针，指向中控器
        iterator start;                     //中控器的第一个节点
        iterator finish;                    //中控器的最后一个节点
        map_pointer map;                    //指向map(中控器)，map是块连续的空间，其每个元素都是一个指针，指向一个节点*（缓冲区）
        size_type map_size;                 //map中有多少个节点

    protected:
        typedef simple_alloc<value_type, Alloc> data_allocator;      //每次配置一个元素的大小
        typedef simple_alloc<pointer, Alloc> map_allocator;          //每次配置一个指针的大小


    protected:
        static size_t initial_map_size() { return 8; }    //map的管理的初始节点默认是8个
        //负责产生并安排好deque的结构，并将元素的初值设定妥当
        void create_map_and_nodes(size_type num_elements);

        void fill_initialize(size_type n, const value_type &value);

        size_type buffer_size() {
            return __deque_buf_size(BufSiz, sizeof(T));
        }

        //自己写的，分配一个缓冲区
        pointer allocate_node() {
            return data_allocator::allocate(buffer_size());
        }

        //自己写的
        //释放一个缓冲区,x指向缓冲区的头节点
        //先析构再销毁
        void deallocate_node(pointer x) {
            destroy(x, x + buffer_size());
            data_allocator::deallocate(x, buffer_size());
        }

        //释放中控器
        void destroy_map_and_nodes() {
            //map指向中控器的首地址
            map_allocator::deallocate(map, map_size);
        }

        void reallocate_map(size_type nodes_to_add, bool add_at_front);

        void reserve_map_at_back(size_type nodes_to_add = 1) {
            if (nodes_to_add + 1 > map_size - (finish.node - map)) {
                //如果map尾端的节点备用空间不足
                //符合以上条件则必须重焕一个map（配置更大的，拷贝原来的，释放原来的）
                reallocate_map(nodes_to_add, false);
            }
        }

        void reserve_map_at_front(size_type nodes_to_add = 1) {
            if (nodes_to_add > start.node - map) {
                reallocate_map(nodes_to_add, true);
            }
        }

        void push_back_aux(const value_type &t);

        void push_front_aux(const value_type &t);

        void pop_back_aux();

        void pop_front_aux();

        iterator insert_aux(iterator, const value_type &x);

    public:
        //迭代器操作
        iterator begin() { return start; }

        pointer first() { return start.first; }

        pointer last() { return start.last; }

        iterator end() { return finish; }

        reference operator[](size_type n) {
            return start[difference_type(n)];           //调用__deque_iterator<>::operator[]
        }

        reference front() { return *start; }

        reference back() {
            iterator tmp = finish;
            --tmp;
            return *tmp;
            //上面三行不能改为return *--finish;因为finish要始终表示缓冲区最后一个元素（的下一个位置），不能移动
        }

        size_type size() const { return finish - start; }

        size_type max_size() const { return size_type(-1); }

        bool empty() const { return finish == start; }

        //构造和内存管理
        //默认构造
        deque() : start(), finish(), map(0), map_size(0) {
            fill_initialize(0, value_type());
        }

        //有参构造
        deque(int n, const value_type &value)
                : start(), finish(), map(0), map_size(0) {
            fill_initialize(n, value);
        }

        //deque的元素管理
        void push_back(const value_type &t) {
            if (finish.cur != finish.last - 1) {
                //最后缓冲区有一个以上的元素备用空间
                construct(finish.cur, t);
                ++finish.cur;
            } else {
                //最后一个缓冲区只能一个元素的备用空间
                push_back_aux(t);
            }
        }

        void push_front(const value_type &t) {
            if (start.cur != start.first) {
                construct(start.cur - 1, t);
                --start.cur;
            } else {
                push_front_aux(t);
            }
        }

        //这里之所以要判断最后一个缓冲区至少有一个元素或第一个缓冲区至少有两个元素
        //finish可以放在这个缓冲区里表示队列最后，可以不用释放缓冲区
        //第一个缓冲区只有一个元素是就需要移动start,需要释放缓冲区
        void pop_back() {
            //最后一个缓冲区有一个或更多元素
            if (finish.cur != finish.first) {
                --finish.cur;
                destroy(finish.cur);
            } else {
                //最后缓冲区没有任何元素
                pop_back_aux();         //这里将进行缓冲区的释放工作
            }

        }

        void pop_front() {
            if (start.cur != (start.last - 1)) {
                //第一个缓冲区有两个或更多元素
                destroy(start.cur);
                ++start.cur;
            } else {
                //第一个缓冲区仅有一个元素
                pop_front_aux();    //进行缓冲区的释放工作
            }

        }

        void clear();

        iterator erase(iterator pos);

        iterator erase(iterator first, iterator last);

        iterator insert(iterator position, const value_type &x) {
            if (position.cur == start.cur) {
                push_front(x);
                return start;
            } else if (position.cur == finish.cur) {
                push_back(x);
                iterator tmp = finish;
                --tmp;
                return tmp;             //返回指向插入数据的迭代器
            } else {
                return insert_aux(position, x);
            }
        }

        bool operator==(const deque &x) {
            return size() == x.size() && equal(begin(), end(), x.begin());
        }

        bool operator<(const deque &x) {
            return lexicographical_compare(begin(), end(), x.begin(), x.end());
        }

    };

    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::create_map_and_nodes(size_type num_elements) {
        //需要的节点数=(元素的个数/每个缓冲区可容纳的元素)+1
        //如果刚好整除，会多配一个节点
        size_type num_nodes = num_elements / buffer_size() + 1;
        //一个map要管理几个节点。最少是8个，最多是"所需节点数加2"前后各预留一个，扩充时可用
        map_size = max(initial_map_size(), num_nodes + 2);
        map = map_allocator::allocate(map_size);        //map指向分配空间的头节点

        //以下另nstart和nfinish指向map所拥有之全部节点的最中央区段
        //保持在最中央，可使头尾两段的扩重能量一样大。每个节点可对应一个缓冲区
        //指向的是中控器
        map_pointer nstart = map + (map_size - num_nodes) / 2;        //指向num_nodes的开始
        map_pointer nfinish = nstart + num_nodes - 1;                //指向num_nodes的结束

        map_pointer cur;
        try {
            for (cur = nstart; cur <= nfinish; ++cur) {
                *cur = allocate_node();
            }
        }
        catch (...) {
            //commit or rollback
            //创建到哪（cur），销毁到哪
            for (map_pointer n = nstart; n < cur; ++n) {
                deallocate_node(*n);
            }
            map_allocator::deallocate(map, map_size);
            throw;
        }

        //为deque内的两个迭代器start和end设定正确的内容
        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.first;
        finish.cur = finish.first + num_elements % buffer_size();
    }


    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::fill_initialize(size_type n, const value_type &value) {
        create_map_and_nodes(n);    //把deque的结构都产生并安排好
        map_pointer cur;            //中控器
        try {
            for (cur = start.node; cur < finish.node; ++cur) {
                //为每个中控器节点指向的缓冲区设定初值
                uninitialized_fill(*cur, *cur + buffer_size(), value);
            }
            //最后一个节点的设定稍有不同（因为尾端可能有备用空间，不必设初值）
            uninitialized_fill(finish.first, finish.cur, value);
        }
        catch (...) {       //若非全部成功，否则一个不留
            for (map_pointer i = start.node; i < cur; ++i) {
                destroy(*i, *i + buffer_size());     //参数是两个迭代器
            }
            destroy_map_and_nodes();
            throw;
        }
    }

    //插入数据时，上一个缓冲区存满，会调用这个函数，或跳到下一个缓冲区。或重新换一个map,用以增加缓冲区
    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::push_back_aux(const value_type &t) {
        value_type t_copy = t;
        reserve_map_at_back();          //符合某种条件则必须重换一个map
        *(finish.node + 1) = allocate_node();       //配置一个map新节点
        try {
            construct(finish.cur, t_copy);
            finish.set_node(finish.node + 1);       //跳到下一个缓冲区
            finish.cur = finish.first;
        }
        catch (...) {
            deallocate_node(*(finish.node + 1));          //分配失败就销毁finish.node + 1所指向的缓冲区
            throw;
        }
    }


    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::push_front_aux(const value_type &t) {
        value_type t_copy = t;
        reserve_map_at_front();
        *(start.node - 1) = allocate_node();
        try {
            start.set_node(start.node - 1);         //start的last指针指向缓冲区的结尾，但此处并没有空节点
            start.cur = start.last - 1;
            construct(start.cur, t_copy);
        }
        catch (...) {
            //commit or rollback
            start.set_node(start.node + 1);
            start.cur = start.first;
            deallocate_node(*(start.node - 1));
            throw;
        }
    }

    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes = finish.node - start.node + 1;               //旧map的大小
        size_type new_num_nodes = old_num_nodes + nodes_to_add;             //新map的大小
        map_pointer new_nstart;
        if (map_size > 2 * new_num_nodes) {
            //把new_nstart移动到新位置，这个计算方法保证指向有数据的缓冲区的指针是在map的中间位置，方便deque向左右扩充
            new_nstart =
                    map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            if (new_nstart < start.node)
                copy(start.node, finish.node + 1, new_nstart);     //把start和finish范围内的指针复制到new_nstart开始的区间内
            else
                copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);       //为防止覆盖，从后往前复制
        } else {
            //map_size不足new_num_nodes的二呗，需要扩充map,保持有数据的值在map中间
            size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            //把原map的内容拷贝过来
            copy(start.node, finish.node + 1, new_nstart);
            //释放原map
            map_allocator::deallocate(map, map_size);
            //设定新map的起始地址和大小
            map = new_map;
            map_size = new_map_size;
        }
        //重新设定迭代器start和finish
        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);

    }

    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::pop_back_aux() {
        deallocate_node(finish.first);
        finish.set_node(finish.node - 1);
        finish.cur = finish.last - 1;
        destroy(finish.cur);
    }

    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::pop_front_aux() {
        destroy(start.cur);
        deallocate_node(start.first);
        start.set_node(start.node + 1);
        start.cur = start.first;
    }

    //deque的最初状态保有一个缓冲区。因此，在clear()完成后恢复最初状态也要保留一个缓冲区。
    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::clear() {
        for (map_pointer node = start.node + 1; node < finish.node; ++node) {       //释放头尾以外的每一个缓冲区（他们一定是饱满的）
            destroy(*node, buffer_size());           //析构元素
            data_allocator::deallocate(*node, buffer_size());   //释放内存
        }
        //处理头尾两个缓冲区
        if (start.node != finish.node) {
            destroy(start.cur, start.last);      //将头缓冲区的所有元素析构
            destroy(finish.first, finish.cur);   //将尾缓冲区的所有元素析构
            //释放尾缓冲区，头缓冲区保留
            data_allocator::deallocate(finish.first, buffer_size());
        } else {
            //只有一个缓冲区
            destroy(start.cur, finish.last);         //将此唯一缓冲区的所有元素析构
            //不释放缓冲区，而是将这一缓冲区保留
        }
        finish = start;         //调整指针状态

    }

    //清除元素跟vector一样，需要把清除点之前或之后的元素移动
    template<class T, class Alloc, size_t BufSiz>
    typename mystl::deque<T, Alloc, BufSiz>::iterator
    deque<T, Alloc, BufSiz>::erase(iterator pos) {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;        //清除点之前的元素个数
        if (size_type(index) < (this->size() >> 1)) {                //如果清除点之前的元素比较少
            copy_backward(start, pos, next);            //这个操作是左闭右开的
            pop_front();
        } else {
            copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }

    template<class T, class Alloc, size_t BufSiz>
    typename mystl::deque<T, Alloc, BufSiz>::iterator
    deque<T, Alloc, BufSiz>::erase(iterator first, iterator last) {
        if (first == start && last == finish) {           //如果清除区间是整个deque,直接调用clear()
            clear();
            return finish;
        } else {
            difference_type n = last - first;
            difference_type elems_before = first - start;
            if (elems_before < (size() - n) / 2) {
                copy_backward(start, first, last);
                iterator new_start = start + n;
                destroy(start, new_start);
                //将冗余的缓冲区释放
                for (map_pointer cur = start.node; cur < new_start.node; ++cur) {
                    data_allocator::deallocate(*cur, buffer_size());
                }
                start = new_start;
            } else {
                copy(last, finish, first);
                iterator new_finish = finish - n;
                destroy(new_finish, finish);
                for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) {
                    data_allocator::deallocate(*cur, buffer_size());
                }
                finish = new_finish;
            }
            return start + elems_before;
        }
    }

    template<class T, class Alloc, size_t BufSiz>
    typename mystl::deque<T, Alloc, BufSiz>::iterator
    deque<T, Alloc, BufSiz>::insert_aux(iterator pos, const value_type &x) {
        difference_type index = pos - start;        //插入点之前的元素个数
        value_type x_copy = x;
        if (index < size() / 2) {
            push_front(front());                //在最前端加入与第一元素同值的元素，每次增删改后，迭代器会失效
            iterator front1 = start;               //以下标示记号，然后进行元素移动
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = start + index;
            iterator pos1 = pos;
            ++pos1;
            copy(front2, pos1, front1);               //元素移动
        } else {
            push_back(back());
            iterator back1 = finish;
            --back1;
            iterator back2 = back1;
            --back2;
            pos = start + index;
            copy_backward(pos, back2, back1);
        }
        *pos = x_copy;
        return pos;

    }

}   //namespace mystl


#endif //IMPLTMENT_STL_MY_DEQUE_H
