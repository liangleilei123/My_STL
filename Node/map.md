## 概述

map的特性是，所有元素都会根据元素的键值自动被排序。map 的所有元素都是pair，同时拥有实值（value）和键值 （key）。pair的第一元素被视为键值，第二元素被视为实值。map 不允许两个元素拥有相同的键值。

不能修改键值，这样会严重破坏map的组织。可以修改元素的实值。

map 拥有和list 相同的某些性质：当容户端对它进行元素新增操作(insert）或删除操作（erase）时，操作之前的所有迭代器，在操作完成之后都依然有效．当然，被删除的那个元素的迭代器必然是个例外。



源码：

```c++
template<class Key, class T, class Compare = less<Key>, class Alloc=alloc>
class map {
    typedef Key key_type;
    typedef T data_type;
    typedef T mapped_type;
    typedef pair<const Key, T> value_type;
    typedef Compare key_compare;


    ///嵌套类
    //嵌套类只能从封闭类中直接使用名称、类型名称，静态成员的名称和枚举数。 若要使用其他类成员的名称，您必须使用指针、引用或对象名。
    //将一个类嵌入另一个类中不会为嵌入类的成员函数提供特殊访问权限。 同样，封闭类的成员函数不具有对嵌套类的成员的特殊访问权限。
    class value_compare
            : public binary_function<value_type, value_type, bool> {
        friend class map<Key, T, Compare, Alloc>;       //value_compare可以访问map的私有成员

    protected:
        Compare comp;

        value_compare(Compare c) : comp(c) {}

    public:
        bool operator()(const value_type &x, const value_type &y) const {
            return comp(x.first, y.first);
        }
    };

private:
    typedef _rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc> _rep_type;
    _rep_type _m_t;             //底层是红黑树，私有成员

public:
    typedef typename _rep_type::pointer pointer;          //map不允许修改键值
    typedef typename _rep_type::const_pointer const_pointer;
    typedef typename _rep_type::reference reference;
    typedef typename _rep_type::const_reference const_reference;
    typedef typename _rep_type::iterator iterator;
    typedef typename _rep_type::const_iterator const_iterator;
    typedef typename _rep_type::size_type size_type;
    typedef typename _rep_type::difference_type difference_type;
    typedef typename _rep_type::allocator_type allocator_type;

    map() : _m_t(Compare(), allocator_type()) {}

    explicit map(const Compare &comp, const allocator_type &a = allocator_type())
            : _m_t(comp, a) {}

    map(const value_type *first, const value_type *last)
            : _m_t(Compare(), allocator_type()) {
        _m_t.insert_unique(first, last);
    }

    map(const value_type *first, const value_type *last,
        const Compare &comp,
        const allocator_type &a = allocator_type())
            : _m_t(comp, a) {
        _m_t.insert_unique(first, last);
    }

    map(const_iterator first, const_iterator last)
            : _m_t(Compare(), allocator_type()) {
        _m_t.insert_unique(first, last);
    }

    map(const_iterator first, const_iterator last,
        const Compare &comp,
        const allocator_type &a = allocator_type())
            : _m_t(comp, a) {
        _m_t.insert_unique(first, last);
    }

    map(const map<Key, Compare, Alloc> &x) : _m_t(x._m_t) {

    }

    map<Key, Compare, Alloc> &operator=(const map<Key, Compare, Alloc> &x) {
        _m_t = x._m_t;
        return *this;
    }

    key_compare key_comp() const { return _m_t.key_comp(); }

    value_compare value_comp() const { return value_compare(_m_t.key_comp()); }

    allocator_type get_allocator() const { return _m_t.get_allocator(); }

    iterator begin() { return _m_t.begin(); }

    iterator begin() const { return _m_t.begin(); }

    iterator end() { return _m_t.end(); }

    iterator end() const { return _m_t.end(); }

    bool empty() const { return _m_t.empty(); }

    size_type size() const { return _m_t.size(); }

    size_type max_size() const { return _m_t.max_size(); }

    //找不到就插入，找到就返回
    T &operator[](const key_type &k) {
        iterator i = lower_bound(k);

        if (i == end() || key_comp()(k, (*i).first)) {
            i = insert(i, value_type(k, T()));
        }
        return (*i).second;
    }

    void swap(map<Key, Compare, Alloc> &x) { _m_t.swap(x._m_t); }

    ///insert
    pair<iterator, bool> insert(const value_type &x) {
        return _m_t.insert_unique(x);
    }

    iterator insert(iterator position, const value_type &x) {
        return _m_t.insert_unique(position, x);
    }

    void insert(const_iterator first, const_iterator last) {
        _m_t.insert_unique(first, last);
    }

    void insert(const value_type *first, const value_type *last) {
        _m_t.insert_unique(first, last);
    }

    ///erase
    void erase(iterator position) {
        _m_t.erase(position);
    }

    size_type erase(const key_type &x) {
        return _m_t.erase(x);
    }

    void erase(iterator first, iterator last) {
        _m_t.erase(first, last);
    }

    void clear() { _m_t.clear(); }

    iterator find(const key_type &x) { return _m_t.find(x); }

    iterator find(const key_type &x) const { return _m_t.find(x); }

    size_type count(const key_type &x) const {
        return _m_t.find(x) == _m_t.end() ? 0 : 1;
    }

    iterator lower_bound(const key_type &x) {
        return _m_t.lower_bound(x);
    }

    iterator lower_bound(const key_type &x) const {
        return _m_t.lower_bound(x);
    }

    iterator upper_bound(const key_type &x) {
        return _m_t.upper_bound(x);
    }

    iterator upper_bound(const key_type &x) const {
        return _m_t.upper_bound(x);
    }

    pair<iterator, iterator> equal_range(const key_type &x) {
        return _m_t.equal_range(x);
    }

    pair<iterator, iterator> equal_range(const key_type &x) const {
        return _m_t.equal_range(x);
    }


};

template<class Key, class Compare, class Alloc>
inline bool operator==(const map<Key, Compare, Alloc> &x,
                       const map<Key, Compare, Alloc> &y) {
    return x._m_t == y._m_t;
}

template<class Key, class Compare, class Alloc>
inline bool operator<(const map<Key, Compare, Alloc> &x,
                      const map<Key, Compare, Alloc> &y) {
    return x._m_t < y._m_t;
}

template<class Key, class Compare, class Alloc>
inline bool operator!=(const map<Key, Compare, Alloc> &x,
                       const map<Key, Compare, Alloc> &y) {
    return !(x._m_t == y._m_t);
}

template<class Key, class Compare, class Alloc>
inline bool operator>(const map<Key, Compare, Alloc> &x,
                      const map<Key, Compare, Alloc> &y) {
    return y._m_t < x._m_t;
}

template<class Key, class Compare, class Alloc>
inline bool operator<=(const map<Key, Compare, Alloc> &x,
                       const map<Key, Compare, Alloc> &y) {
    return !(y._m_t < x._m_t);
}

template<class Key, class Compare, class Alloc>
inline bool operator>=(const map<Key, Compare, Alloc> &x,
                       const map<Key, Compare, Alloc> &y) {
    return !(x._m_t < y._m_t);
}

template<class Key, class Compare, class Alloc>
inline void swap(map<Key, Compare, Alloc> &x,
                 map<Key, Compare, Alloc> &y) {
    x.swap(y);
}

```

