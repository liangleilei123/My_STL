## 概述

set 的特性是，**所有元素都会根据元素的键值自动被排序。**set 的元素不像map那样可以同时拥有实值（waluve）和键值（key），set 元素的键值就是实值，实值就是键值。set 不允许两个元素有相同的键值。

**不能通过迭代器修改set的元素值。**因为**set元素值就是键值**，关系到set元素的排列规则，如果人意改变set元素值，会严重破坏set组织。

set 拥有与 list 相同的某些性质：当客户端对它进行元素新增操作(insert）或删除操作（erase）时，**操作之前的所有迭代器，在操作完成之后都依然有效。**当然，被删除的那个元素的迭代器必然是个例外。



源码如下：

```c++
template<class Key, class Compare = less<Key>, class Alloc = alloc>
class set {
public:
    typedef Key key_type;
    typedef Key value_type;
    typedef Compare key_compare;
    typedef Compare value_compare;

private:
    typedef _rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> _rep_type;
    _rep_type _m_t;             //底层是红黑树，私有成员

public:
    typedef typename _rep_type::const_pointer pointer;          //set不允许修改键值
    typedef typename _rep_type::const_pointer const_pointer;
    typedef typename _rep_type::const_reference reference;
    typedef typename _rep_type::const_reference const_reference;
    typedef typename _rep_type::const_iterator iterator;
    typedef typename _rep_type::const_iterator const_iterator;
    typedef typename _rep_type::size_type size_type;
    typedef typename _rep_type::difference_type difference_type;
    typedef typename _rep_type::allocator_type allocator_type;

    set() : _m_t(Compare(), allocator_type()) {}

    explicit set(const Compare &comp, const allocator_type &a = allocator_type())
            : _m_t(comp, a) {}


    set(const value_type *first, const value_type *last)
            : _m_t(Compare(), allocator_type()) {
        _m_t.insert_unique(first, last);
    }

    set(const value_type *first, const value_type *last,
        const Compare &comp,
        const allocator_type &a = allocator_type())
            : _m_t(comp, a) {
        _m_t.insert_unique(first, last);
    }

    set(const_iterator first, const_iterator last)
            : _m_t(Compare(), allocator_type()) {
        _m_t.insert_unique(first, last);
    }

    set(const_iterator first, const_iterator last,
        const Compare &comp,
        const allocator_type &a = allocator_type())
            : _m_t(comp, a) {
        _m_t.insert_unique(first, last);
    }

    set(const set<Key, Compare, Alloc> &x) : _m_t(x._m_t) {

    }

    set<Key, Compare, Alloc> &operator=(const set<Key, Compare, Alloc> &x) {
        _m_t = x._m_t;
        return *this;
    }

    key_compare key_comp() const { return _m_t.key_comp(); }

    value_compare value_comp() const { return _m_t.key_comp(); }

    allocator_type get_allocator() const { return _m_t.get_allocator(); }

    iterator begin() const { return _m_t.begin(); }

    iterator end() const { return _m_t.end(); }

    bool empty() const { return _m_t.empty(); }

    size_type size() const { return _m_t.size(); }

    size_type max_size() const { return _m_t.max_size(); }

    void swap(set<Key, Compare, Alloc> &x) { _m_t.swap(x._m_t); }

    ///insert
    std::pair<iterator, bool> insert(const value_type &x) {
        std::pair<typename _rep_type::iterator, bool> p = _m_t.insert_unique(x);
        return std::pair<iterator, bool>(p.first, p.second);
    }

    iterator insert(iterator position, const value_type &x) {
        typedef typename _rep_type::iterator _rep_iterator;
        return _m_t.insert_unique((_rep_iterator &) position, x);
    }

    void insert(const_iterator first, const_iterator last) {
        _m_t.insert_unique(first, last);
    }

    void insert(const value_type *first, const value_type *last) {
        _m_t.insert_unique(first, last);
    }

    ///erase
    void erase(iterator position) {
        typedef typename _rep_type::iterator _rep_iterator;     //两个迭代器的底层实现是相同的，可以进行强制转换
        _m_t.erase((_rep_iterator &) position);
    }

    size_type erase(const key_type &x) {
        return _m_t.erase(x);
    }

    void erase(iterator first, iterator last) {
        typedef typename _rep_type::iterator _rep_iterator;
        _m_t.erase((_rep_iterator &) first, (_rep_iterator &) last);
    }

    void clear() { _m_t.clear(); }

    iterator find(const key_type &x) const { return _m_t.find(x); }

    size_type count(const key_type &x) const {
        return _m_t.find(x) == _m_t.end() ? 0 : 1;
    }

    iterator lower_bound(const key_type &x) const {
        return _m_t.lower_bound(x);
    }

    iterator upper_bound(const key_type &x) const {
        return _m_t.upper_bound(x);
    }

    std::pair<iterator, iterator> equal_range(const key_type &x) const {
        return _m_t.equal_range(x);
    }
};

template<class Key, class Compare, class Alloc>
inline bool operator==(const set<Key, Compare, Alloc> &x,
                       const set<Key, Compare, Alloc> &y) {
    return x._m_t == y._m_t;
}

template<class Key, class Compare, class Alloc>
inline bool operator<(const set<Key, Compare, Alloc> &x,
                      const set<Key, Compare, Alloc> &y) {
    return x._m_t < y._m_t;
}

template<class Key, class Compare, class Alloc>
inline bool operator!=(const set<Key, Compare, Alloc> &x,
                       const set<Key, Compare, Alloc> &y) {
    return !(x._m_t == y._m_t);
}

template<class Key, class Compare, class Alloc>
inline bool operator>(const set<Key, Compare, Alloc> &x,
                      const set<Key, Compare, Alloc> &y) {
    return y._m_t < x._m_t;
}

template<class Key, class Compare, class Alloc>
inline bool operator<=(const set<Key, Compare, Alloc> &x,
                       const set<Key, Compare, Alloc> &y) {
    return !(y._m_t < x._m_t);
}

template<class Key, class Compare, class Alloc>
inline bool operator>=(const set<Key, Compare, Alloc> &x,
                       const set<Key, Compare, Alloc> &y) {
    return !(x._m_t < y._m_t);
}

template<class Key, class Compare, class Alloc>
inline void swap(set<Key, Compare, Alloc> &x,
                 set<Key, Compare, Alloc> &y) {
    x.swap(y);
}

```



测试代码：

```c++
#include "my_set.h"
#include "iostream"


int main() {
    int i;
    int ia[5] = {0, 1, 2, 3, 4};
    mystl::set<int> iset(ia, ia + 5);

    std::cout << "size = " << iset.size() << std::endl;
    std::cout << "3 count = " << iset.count(3) << std::endl;
    iset.insert(3);
    std::cout << "size = " << iset.size() << std::endl;
    std::cout << "3 count = " << iset.count(3) << std::endl;

    iset.insert(5);
    std::cout << "size = " << iset.size() << std::endl;
    std::cout << "3 count = " << iset.count(3) << std::endl;

    iset.erase(1);
    std::cout << "size = " << iset.size() << std::endl;
    std::cout << "3 count = " << iset.count(3) << std::endl;
    std::cout << "1 count = " << iset.count(1) << std::endl;

    mystl::set<int>::iterator ite1 = iset.begin();
    mystl::set<int>::iterator ite2 = iset.end();

    for (; ite1 != ite2; ++ite1) {
        std::cout << *ite1 << ' ';
    }
    std::cout << std::endl;

    ite1 = iset.find(3);
    if (ite1 != iset.end()) {
        std::cout << "3 found" << std::endl;
    }


}
```

