//
// Created by 梁磊磊 on 2023/6/10.
//

#ifndef IMPLTMENT_STL_MY_MULTISET_H
#define IMPLTMENT_STL_MY_MULTISET_H

#include "stl_alloc.h"
#include "functional.h"
#include "RB-tree.h"
#include "util.h"

namespace mystl {
    //默认情况下采用递增排序
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
            _m_t.insert_equal(first, last);
        }

        set(const value_type *first, const value_type *last,
            const Compare &comp,
            const allocator_type &a = allocator_type())
                : _m_t(comp, a) {
            _m_t.insert_equal(first, last);
        }

        set(const_iterator first, const_iterator last)
                : _m_t(Compare(), allocator_type()) {
            _m_t.insert_equal(first, last);
        }

        set(const_iterator first, const_iterator last,
            const Compare &comp,
            const allocator_type &a = allocator_type())
                : _m_t(comp, a) {
            _m_t.insert_equal(first, last);
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
        pair<iterator, bool> insert(const value_type &x) {
            pair<typename _rep_type::iterator, bool> p = _m_t.insert_equal(x);
            return pair<iterator, bool>(p.first, p.second);
        }

        iterator insert(iterator position, const value_type &x) {
            typedef typename _rep_type::iterator _rep_iterator;
            return _m_t.insert_equal((_rep_iterator &) position, x);
        }

        void insert(const_iterator first, const_iterator last) {
            _m_t.insert_equal(first, last);
        }

        void insert(const value_type *first, const value_type *last) {
            _m_t.insert_equal(first, last);
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

        pair<iterator, iterator> equal_range(const key_type &x) const {
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

}

#endif //IMPLTMENT_STL_MY_MULTISET_H
