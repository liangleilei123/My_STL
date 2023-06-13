//
// Created by 梁磊磊 on 2023/6/11.
//

#ifndef IMPLTMENT_STL_HASHTABLE_H
#define IMPLTMENT_STL_HASHTABLE_H


#include "stl_alloc.h"
#include "stl_iterator.h"
#include "stl_algobase.h"
#include "my_vector.h"
#include "util.h"
#include "my_pair.h"
#include "iostream"

namespace mystl {

    //以开链法完成的hash table
    template<class Value>
    struct _Hashtable_node {
        _Hashtable_node *_M_next;
        Value _M_val;
    };

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc = alloc>
    class hashtable;

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_iterator;

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_const_iterator;

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_iterator {
        typedef hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                _Hashtable;

        typedef _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                iterator;

        typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                const_iterator;

        typedef _Hashtable_node<_Val> _Node;

        typedef forward_iterator_tag iterator_category;
        typedef _Val value_type;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        typedef _Val &reference;
        typedef _Val *pointer;

        _Node *_M_cur;              //迭代器目前所指的节点
        _Hashtable *_M_ht;          //保持对容器的连结关系，可能需要从一个bucket跳到另一个bucket

        _Hashtable_iterator() {}

        _Hashtable_iterator(_Node *__n, _Hashtable *__tab)
                : _M_cur(__n), _M_ht(__tab) {}

        reference operator*() const { return _M_cur->_M_val; }

        pointer operator->() const { return &operator*(); }

        iterator &operator++();

        iterator operator++(int);

        bool operator==(const iterator &__it) const {
            return _M_cur == __it._M_cur;
        }

        bool operator!=(const iterator &__it) const {
            return _M_cur != __it._M_cur;
        }
    };


    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_const_iterator {
        typedef hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                _Hashtable;

        typedef _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                iterator;

        typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                const_iterator;

        typedef _Hashtable_node<_Val> _Node;

        typedef forward_iterator_tag iterator_category;
        typedef _Val value_type;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        typedef const _Val &reference;
        typedef const _Val *pointer;

        const _Node *_M_cur;
        const _Hashtable *_M_ht;          //指向哈希表

        _Hashtable_const_iterator() {}

        _Hashtable_const_iterator(const _Node *__n, const _Hashtable *__tab)
                : _M_cur(__n), _M_ht(__tab) {}

        _Hashtable_const_iterator(const iterator &it)
                : _M_cur(it._M_cur), _M_ht(it._M_ht) {}

        reference operator*() const { return _M_cur->_M_val; }

        pointer operator->() const { return &operator*(); }

        const_iterator &operator++();

        const_iterator operator++(int);

        bool operator==(const const_iterator &__it) const {
            return _M_cur == __it._M_cur;
        }

        bool operator!=(const const_iterator &__it) const {
            return _M_cur != __it._M_cur;
        }
    };

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc> &
    _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::operator++() {
        const _Node *_old = _M_cur;
        _M_cur = _M_cur->_M_next;          //如果存在，结果就是它
        if (!_M_cur) {
            //根据元素值，定位出下一个非空的bucket.其起头处就是结果
            size_type __bucket = _M_ht->_M_bkt_num(_old->_M_val);
            while (!_M_cur && ++__bucket < _M_ht->_M_buckets.size()) {
                _M_cur = _M_ht->_M_buckets[__bucket];
            }
        }
        return *this;
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::operator++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;

    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc> &
    _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::operator++() {
        const _Node *_old = _M_cur;
        _M_cur = _M_cur->_M_next;          //如果存在，结果就是它
        if (!_M_cur) {
            //根据元素值，定位出下一个非空的bucket.其起头处就是结果
            size_type __bucket = _M_ht->_M_bkt_num(_old->_M_val);
            while (!_M_cur && ++__bucket < _M_ht->_M_buckets.size()) {
                _M_cur = _M_ht->_M_buckets[__bucket];
            }
        }
        return *this;
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::operator++(int) {
        const_iterator tmp = *this;
        ++*this;
        return tmp;

    }


    //假设 long 至少是32个字节
    enum {
        __stl_num_primes = 28
    };      //定义一个枚举常量，用于指示__stl_primes_list数组的大小

    //虽然用的开链法，但还是用这28个质数来表示表格的大小。
    //这个数组中的每个元素都是一个质数，这些质数在STL的哈希表实现中被用作桶的数量。
    //这些质数是经过精心选择的（整体呈现大约两倍的关系），以便在不同的负载因子下提供较好的性能和空间效率。
    static const unsigned long __stl_prime_list[__stl_num_primes] = {
            53ul, 97ul, 193ul, 389ul, 769ul,
            1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
            49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
            1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
            50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
            1610612741ul, 3221225473ul, 4294967291ul
    };

    //找到上述28个质数中，第一个不小于n的数
    inline unsigned long __stl_next_prime(unsigned long n) {
        const unsigned long *__first = __stl_prime_list;
        const unsigned long *__last = __stl_prime_list + (int) __stl_num_primes;
        const unsigned long *pos = lower_bound(__first, __last, n);
        return pos == __last ? *(__last - 1) : *pos;
    }


    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    class hashtable {
        typedef _Key key_type;          //键值型别
        typedef _Val value_type;        //实值型别
        typedef _HashFcn hasher;        //构造散列的函数
        typedef _EqualKey key_equal;    //从节点中取出键值的方法

        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef value_type &reference;
        typedef const value_type &const_reference;

        hasher hash_funct() const { return _M_hash; }

        key_equal key_eq() const { return _M_equals; }

    private:
        typedef _Hashtable_node<_Val> _Node;
        typedef simple_alloc<_Node, _Alloc> _M_node_allocator_type;

        //创建节点和释放节点空间
        _Node *_M_get_node() { return _M_node_allocator_type::allocate(1); }

        void _M_put_node(_Node *p) { _M_node_allocator_type::deallocate(p, 1); }

    public:
        typedef _Alloc allocator_type;

        allocator_type get_allocator() const { return allocator_type(); }

        typedef _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                iterator;

        typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                const_iterator;

        //两个迭代器设为友元类，可以访问私有成员
        friend struct _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>;
        friend struct _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>;


    private:
        //哈希表的数据成员
        hasher _M_hash;
        key_equal _M_equals;
        _ExtractKey _M_get_key;
        vector<_Node *, _Alloc> _M_buckets;         //指针类型的vector
        size_type _M_num_elements;

    public:
        ///构造
        hashtable(size_type __n,
                  const _HashFcn &__hf,
                  const _EqualKey &__eql,
                  const _ExtractKey &__ext,
                  const allocator_type &__a = allocator_type())
                : _M_hash(__hf),
                  _M_equals(__eql),
                  _M_get_key(__ext),
                  _M_buckets(),
                  _M_num_elements(0) {
            _M_initialize_buckets(__n);
        }

        hashtable(size_type __n,
                  const _HashFcn &__hf,
                  const _EqualKey &__eql,
                  const allocator_type &__a = allocator_type())
                : _M_hash(__hf),
                  _M_equals(__eql),
                  _M_get_key(_ExtractKey()),
                  _M_buckets(),
                  _M_num_elements(0) {
            _M_initialize_buckets(__n);
        }

        hashtable(const hashtable &__ht)
                : _M_hash(__ht._M_hash),
                  _M_equals(__ht._M_equals),
                  _M_get_key(__ht._M_get_key),
                  _M_buckets(__ht._M_buckets),
                  _M_num_elements(0) {
            _M_copy_from(__ht);
        }

        hashtable &operator=(const hashtable &__ht) {
            if (&__ht != this) {
                clear();
                _M_hash = __ht._M_hash;
                _M_equals = __ht._M_equals;
                _M_get_key = __ht._M_get_key;
                _M_copy_from(__ht);
            }
            return *this;
        }

        ~hashtable() { clear(); }

        size_type size() const {
            return _M_num_elements;
        }

        size_type max_size() const {
            return size_type(-1);
        }

        bool empty() const {
            return _M_num_elements == 0;
        }

        void swap(hashtable &__ht) {
            mystl::swap(_M_hash, __ht._M_hash);
            mystl::swap(_M_equals, __ht._M_equals);
            mystl::swap(_M_get_key, __ht._M_get_key);
            _M_buckets.swap(__ht._M_buckets);
            mystl::swap(_M_num_elements, __ht._M_num_elements);
        }

        iterator begin() {
            for (size_type __n = 0; __n < _M_buckets.size(); ++__n)
                if (_M_buckets[__n])
                    return iterator(_M_buckets[__n], this);
            return end();
        }

        iterator end() { return iterator(0, this); }

        const_iterator begin() const {
            for (size_type __n = 0; __n < _M_buckets.size(); ++__n)
                if (_M_buckets[__n])
                    return const_iterator(_M_buckets[__n], this);
            return end();
        }

        const_iterator end() const { return const_iterator(0, this); }


        friend bool operator==(const hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc> &__ht1,
                               const hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc> &__ht2);

    public:
        //桶的数量
        size_type bucket_count() const { return _M_buckets.size(); }

        //桶的最大数量为质数数组的最大值
        size_type max_bucket_count() const { return __stl_prime_list[(int) __stl_num_primes - 1]; }

        //计算某个桶节点所链接链表的节点数
        size_type elems_in_bucket(size_type __bucket) const {
            size_type __result = 0;
            for (_Node *__cur = _M_buckets[__bucket]; __cur; __cur = __cur->_M_next)
                __result += 1;
            return __result;
        }


        //插入元素，不允许重复
        pair<iterator, bool> insert_unique(const value_type &__obj) {
            resize(_M_num_elements + 1);        //判断是否需要重建表格，如需要就扩充
            return insert_unique_noresize(__obj);
        }

        iterator insert_equal(const value_type &__obj) {
            resize(_M_num_elements + 1);
            return insert_equal_noresize(__obj);
        }

        pair<iterator, bool> insert_unique_noresize(const value_type &__obj);

        iterator insert_equal_noresize(const value_type &__obj);

        void insert_unique(const value_type *__f, const value_type *__l) {
            size_type __n = __l - __f;
            resize(_M_num_elements + __n);
            for (; __n > 0; --__n, ++__f) {
                insert_unique_noresize(*__f);
            }
        }

        void insert_equal(const value_type *__f, const value_type *__l) {
            size_type __n = __l - __f;
            resize(_M_num_elements + __n);
            for (; __n > 0; --__n, ++__f)
                insert_equal_noresize(*__f);
        }

        void insert_unique(const_iterator __f, const_iterator __l) {
            size_type __n = 0;
            distance(__f, __l, __n);
            resize(_M_num_elements + __n);
            for (; __n > 0; --__n, ++__f)
                insert_unique_noresize(*__f);
        }

        void insert_equal(const_iterator __f, const_iterator __l) {
            size_type __n = 0;
            distance(__f, __l, __n);
            resize(_M_num_elements + __n);
            for (; __n > 0; --__n, ++__f)
                insert_equal_noresize(*__f);
        }

        reference find_or_insert(const value_type &__obj);

        //查找键值，没找到返回end()
        iterator find(const key_type &__key) {
            size_type __n = _M_bkt_num_key(__key);
            _Node *__first = 0;
            for (__first = _M_buckets[__n];
                 __first && !_M_equals(_M_get_key(__first->_M_val), __key);
                 __first = __first->_M_next) {}
            return iterator(__first, this);
        }

        const_iterator find(const key_type &__key) const {
            size_type __n = _M_bkt_num_key(__key);
            const _Node *__first;
            for (__first = _M_buckets[__n];
                 __first && !_M_equals(_M_get_key(__first->_M_val), __key);
                 __first = __first->_M_next) {}
            return const_iterator(__first, this);
        }

        //这个键值在哈希表中的数量
        size_type count(const key_type &__key) const {
            const size_type __n = _M_bkt_num_key(__key);
            size_type __result = 0;

            for (const _Node *__cur = _M_buckets[__n]; __cur; __cur = __cur->_M_next)
                if (_M_equals(_M_get_key(__cur->_M_val), __key))
                    ++__result;
            return __result;
        }

        pair<iterator, iterator>
        equal_range(const key_type &key);       //与此键值相等的范围，左闭右开

        pair<const_iterator, const_iterator>
        equal_range(const key_type &key) const;       //与此键值相等的范围，左闭右开

        size_type erase(const key_type &__key);

        void erase(const iterator &__it);

        void erase(iterator __first, iterator __last);

        void erase(const const_iterator &__it);

        void erase(const_iterator __first, const_iterator __last);

        void resize(size_type __num_elements_hint);

        void clear();


    private:

        //从数组中得到第一个比__n大的质数，并分配相应的空间
        size_type _M_next_size(size_type __n) const { return __stl_next_prime(__n); }

        void _M_initialize_buckets(size_type __n) {
            const size_type __n_buckets = _M_next_size(__n);
            _M_buckets.reserve(__n_buckets);        //调整原来桶的大小，并通过插入完成对增加部分的初始化

            _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node *) 0);
            _M_num_elements = 0;
        }

        void _M_copy_from(const hashtable &__ht);       //拷贝构造用的函数
        ///判断元素的落脚处
        //接受键值和桶容器的大小
        size_type _M_bkt_num_key(const key_type &__key, size_t __n) const {
            return _M_hash(__key) % __n;
        }

        //接受实值和桶容器的大小
        size_type _M_bkt_num(const value_type &__obj, size_t __n) const {
            return _M_bkt_num_key(_M_get_key(__obj), __n);
        }


        //只接受键值
        size_type _M_bkt_num_key(const key_type &__key) const {
            return _M_bkt_num_key(__key, _M_buckets.size());
        }

        //只接受实值
        size_type _M_bkt_num(const value_type &obj) const {
            return _M_bkt_num_key(_M_get_key(obj));
        }


        //创建新节点
        _Node *_M_new_node(const value_type &__obj) {
            _Node *__n = _M_get_node();     //分配空间
            __n->_M_next = 0;
            try {
                construct(&__n->_M_val, __obj);     //构造
                return __n;
            }
            catch (...) {
                _M_put_node(__n);
            }
            throw;
        }

        //销毁新节点
        void _M_delete_node(_Node *__n) {
            destroy(&__n->_M_val);
            _M_put_node(__n);
        }

        void _M_erase_bucket(const size_type __n, _Node *__first, _Node *__last);

        void _M_erase_bucket(const size_type __n, _Node *__last);

    };

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    bool operator==(const hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc> &__ht1,
                    const hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc> &__ht2) {
        typedef typename hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::_Node _Node;
        if (__ht1._M_buckets.size() != __ht2._M_buckets.size()) {
            return false;
        }
        for (int __n = 0; __n < __ht1._M_buckets.size(); ++__n) {
            _Node *__cur1 = __ht1._M_buckets[__n];
            _Node *__cur2 = __ht2._M_buckets[__n];
            for (; __cur1 && __cur2 && __cur1->_M_val == __cur2->_M_val;
                   __cur1 = __cur1->_M_next, __cur2 = __cur2->_M_next) {}
            if (__cur1 || __cur2)
                return false;
        }
        return true;
    }


    //在不需要重建表格的情况下插入新节点，键值不允许重复
    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    pair<typename hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::iterator, bool>
    hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::insert_unique_noresize(const value_type &obj) {
        const size_type n = _M_bkt_num(obj);       //决定obj应位于n bucket
        _Node *__first = _M_buckets[n];
        for (_Node *__cur = __first; __cur; __cur = __cur->_M_next) {
            if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(obj))) {
                return pair<iterator, bool>(iterator(__cur, this), false);
            }
        }

        _Node *__tmp = _M_new_node(obj);
        __tmp->_M_next = __first;
        _M_buckets[n] = __tmp;      //头插法，让新节点成为链表第一个节点
        ++_M_num_elements;
        return pair<iterator, bool>(iterator(__tmp, this), true);

    }

    //不需要调整哈希表大小的情况下插入
    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    typename hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::iterator
    hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::insert_equal_noresize(const value_type &obj) {
        const size_type n = _M_bkt_num(obj);       //决定obj应位于n bucket
        _Node *__first = _M_buckets[n];
        for (_Node *__cur = __first; __cur; __cur = __cur->_M_next) {
            if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(obj))) {
                _Node *__tmp = _M_new_node(obj);
                __tmp->_M_next = __cur->_M_next;
                __cur->_M_next = __tmp;
                ++_M_num_elements;
                return iterator(__tmp, this);
            }
        }

        _Node *__tmp = _M_new_node(obj);
        __tmp->_M_next = __first;
        _M_buckets[n] = __tmp;      //头插法，让新节点成为链表第一个节点
        ++_M_num_elements;
        return iterator(__tmp, this);

    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    typename hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::size_type
    hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::erase(const key_type &__key) {
        const size_type __n = _M_bkt_num_key(__key);
        _Node *__first = _M_buckets[__n];
        size_type __erased = 0;         //找到落脚点

        if (__first) {
            _Node *__cur = __first;
            _Node *__next = __cur->_M_next;
            while (__next) {            //遍历整个bucket,
                if (_M_equals(_M_get_key(__next->_M_val), __key)) {
                    __cur->_M_next = __next->_M_next;
                    _M_delete_node(__next);
                    __next = __cur->_M_next;
                    ++__erased;
                    --_M_num_elements;
                } else {
                    __cur = __next;
                    __next = __cur->_M_next;
                }
            }
            //最后判断首节点是否和要删除的键值相等
            if (_M_equals(_M_get_key(__first->_M_val), __key)) {
                _M_buckets[__n] = __first->_M_next;
                _M_delete_node(__first);
                ++__erased;
                --_M_num_elements;
            }
        }
        return __erased;        //返回删除的节点个数
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    void hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::erase(const iterator &__it) {
        _Node *__p = __it._M_cur;
        if (__p) {
            const size_type __n = _M_bkt_num(__p->_M_val);
            _Node *__cur = _M_buckets[__n];     //找到落脚桶的头节点

            //删除首节点
            if (__cur == __p) {
                _M_buckets[__n] = __cur->_M_next;
                _M_delete_node(__cur);
                --_M_num_elements;
            } else {
                _Node *__next = __cur->_M_next;
                while (__next) {
                    if (__next == __p) {        //找到对应节点，删除
                        __cur->_M_next = __next->_M_next;
                        _M_delete_node(__next);
                        --_M_num_elements;
                        break;
                    } else {        //没找到继续往后找
                        __cur = __next;
                        __next = __cur->_M_next;
                    }
                }
            }

        }

    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    void hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::erase(iterator __first, iterator __last) {
        size_type __f_bucket = __first._M_cur ?
                               _M_bkt_num(__first._M_cur->_M_val) : _M_buckets.size();
        size_type __l_bucket = __last._M_cur ?
                               _M_bkt_num(__last._M_cur->_M_val) : _M_buckets.size();

        if (__first._M_cur == __last._M_cur)
            return;
        else if (__f_bucket == __l_bucket)
            _M_erase_bucket(__f_bucket, __first._M_cur, __last._M_cur);
        else {
            _M_erase_bucket(__f_bucket, __first._M_cur, 0);
            for (size_type __n = __f_bucket + 1; __n < __l_bucket; ++__n)
                _M_erase_bucket(__n, 0);
            if (__l_bucket != _M_buckets.size())
                _M_erase_bucket(__l_bucket, __last._M_cur);
        }
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    void hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::erase(const_iterator __first, const_iterator __last) {
        //const_cast 用于去除指针或引用类型的 const 限定符
        erase(iterator(const_cast<_Node *>(__first._M_cur), const_cast<hashtable *>(__first._M_.ht)),
              iterator(const_cast<_Node *>(__last._M_cur), const_cast<hashtable *>(__last._M_.ht)));
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    void hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::erase(const const_iterator &__it) {
        erase(iterator(const_cast<_Node *>(__it._M_cur),
                       const_cast<hashtable *>(__it._M_ht)));
    }


    //重建哈希表，以下函数判断是否需要重建表格，
    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    void hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::resize(size_type __num_elements_hint) {
        //哈希表中节点的总个数大于buckets的长度，就重建和希表
        const size_type __old_n = _M_buckets.size();
        if (__num_elements_hint > __old_n) {
            const size_type __n = _M_next_size(__num_elements_hint);
            if (__n > __old_n) {
                //设立新的buckets
                vector<_Node *, _Alloc> __tmp(__n, (_Node *) (0));
                try {

                    for (size_type __bucket = 0; __bucket < __old_n; ++__bucket) {
                        _Node *__first = _M_buckets[__bucket];
                        while (__first) {
                            size_type __new_bucket = _M_bkt_num(__first->_M_val, __n);
                            _M_buckets[__bucket] = __first->_M_next;       //令旧 bucket 指向其所对应之串行的下一个节点（以便迭代处理）
                            __first->_M_next = __tmp[__new_bucket];     //将当前节点插人到新 bucket 内，成为其对应串行的第一个节点
                            __tmp[__new_bucket] = __first;          //头插法
                            __first = _M_buckets[__bucket];         //回到旧 bucket所指的待处理串行，谁备处理下一个节点
                        }
                    }
                    _M_buckets.swap(__tmp);
                }
                catch (...) {
                    for (size_type __bucket = 0; __bucket < __tmp.size(); ++__bucket) {
                        while (__tmp[__bucket]) {
                            _Node *__next = __tmp[__bucket]->_M_next;
                            _M_delete_node(__tmp[__bucket]);
                            __tmp[__bucket] = __next;
                        }
                    }
                    throw;
                }
            }
        }
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    void hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::clear() {
        //把链表的每个节点都销毁
        for (size_type __i = 0; __i < _M_buckets.size(); ++__i) {
            _Node *__cur = _M_buckets[__i];
            while (__cur != 0) {
                _Node *__next = __cur->_M_next;
                _M_delete_node(__cur);
                __cur = __next;
            }
            _M_buckets[__i] = 0;
        }
        _M_num_elements = 0;
        //在此处，buckets vector并未释放掉空间，仍保持原来的大小
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    void hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::_M_copy_from(const hashtable &ht) {

        //先把当前的桶清空，
        _M_buckets.clear();     //清空后start和finish指针都指向开头
        _M_buckets.reserve(ht._M_buckets.size());
        _M_buckets.insert(_M_buckets.end(), ht._M_buckets.size(), (_Node *) 0);
        try {
            //遍历桶
            for (size_type __i = 0; __i < ht._M_buckets.size(); ++__i) {
                const _Node *__cur = ht._M_buckets[__i];        //vector中的每一个元素都是指针
                if (__cur) {
                    _Node *__copy = _M_new_node(__cur->_M_val);
                    _M_buckets[__i] = __copy;
                    //遍历桶节点对应的链表：把每个桶节点对应的链表都复制
                    for (_Node *__next = __cur->_M_next; __next; __cur = __next, __next = __cur->_M_next) {
                        __copy->_M_next = _M_new_node(__next->_M_val);
                        __copy = __copy->_M_next;
                    }
                }
            }
            _M_num_elements = ht._M_num_elements;
        }
        catch (...) {
            clear();
        }

    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    typename hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::reference
    hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::
    find_or_insert(const value_type &__obj) {
        resize(_M_num_elements + 1);

        size_type __n = _M_bkt_num(__obj);      //找到落脚点
        _Node *__first = _M_buckets[__n];
        for (_Node *__cur = __first; __cur; __cur = __cur->_M_next) {
            if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj))) {
                return __cur->_M_val;           //找到相等的，就返回
            }
        }
        //没找到，就创建并插入
        _Node *__tmp = _M_new_node(__obj);
        __tmp->_M_next = __first;
        _M_buckets[__n] = __tmp;
        ++_M_num_elements;
        return __tmp->_M_val;           //返回新插入的实值
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    pair<typename hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::iterator,
            typename hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::iterator>
    hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::equal_range(const key_type &__key) {
        typedef pair<iterator, iterator> _Pii;
        const size_type __n = _M_bkt_num_key(__key);    //找到落脚点

        for (_Node *__first = _M_buckets[__n]; __first; __first = __first->_M_next)
            if (_M_equals(_M_get_key(__first->_M_val), __key)) {        //找到相等的一直往后遍历查找结尾
                for (_Node *__cur = __first->_M_next; __cur; __cur = __cur->_M_next)
                    if (!_M_equals(_M_get_key(__cur->_M_val), __key))
                        return _Pii(iterator(__first, this), iterator(__cur, this));
                //如果查找的结尾到达bucket的结尾，返回下一个有值bucket的头，维持返回区间是左闭右开的
                for (size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
                    if (_M_buckets[__m])
                        return _Pii(iterator(__first, this),
                                    iterator(_M_buckets[__m], this));
                return _Pii(iterator(__first, this), end());
            }
        return _Pii(end(), end());
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    pair<typename hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::const_iterator,
            typename hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>::const_iterator>
    hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::equal_range(const key_type &__key) const {
        typedef pair<const_iterator, const_iterator> _Pii;
        const size_type __n = _M_bkt_num_key(__key);    //找到落脚点

        for (_Node *__first = _M_buckets[__n]; __first; __first = __first->_M_next)
            if (_M_equals(_M_get_key(__first->_M_val), __key)) {        //找到相等的一直往后遍历查找结尾
                for (_Node *__cur = __first->_M_next; __cur; __cur = __cur->_M_next)
                    if (!_M_equals(_M_get_key(__cur->_M_val), __key))
                        return _Pii(const_iterator(__first, this), const_iterator(__cur, this));
                //如果查找的结尾到达bucket的结尾，返回下一个有值bucket的头，维持返回区间是左闭右开的
                for (size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
                    if (_M_buckets[__m])
                        return _Pii(const_iterator(__first, this),
                                    const_iterator(_M_buckets[__m], this));
                return _Pii(const_iterator(__first, this), end());
            }
        return _Pii(end(), end());
    }


    //删除__n号桶的first到last区间的节点
    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    void hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::_M_erase_bucket(const size_type __n, _Node *__first, _Node *__last) {
        _Node *__cur = _M_buckets[__n];
        if (__cur == __first)
            _M_erase_bucket(__n, __last);
        else {
            _Node *__next;
            //找到first节点
            for (__next = __cur->_M_next; __next != __first;
                 __cur = __next, __next = __cur->_M_next);
            while (__next != __last) {
                __cur->_M_next = __next->_M_next;
                _M_delete_node(__next);
                __next = __cur->_M_next;
                --_M_num_elements;
            }

        }
    }

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    void hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
    ::_M_erase_bucket(const size_type __n, _Node *__last) {
        _Node *__cur = _M_buckets[__n];
        while (__cur != __last) {
            _Node *__next = __cur->_M_next;
            _M_delete_node(__cur);
            __cur = __next;
            _M_buckets[__n] = __cur;
            --_M_num_elements;
        }
    }

}


#endif //IMPLTMENT_STL_HASHTABLE_H
