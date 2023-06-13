//
// Created by 梁磊磊 on 2023/6/13.
//

#ifndef IMPLTMENT_STL_MY_HASH_MAP_H
#define IMPLTMENT_STL_MY_HASH_MAP_H


#include "stl_hash_fun.h"
#include "functional.h"
#include "stl_alloc.h"
#include "my_pair.h"
#include "hashtable.h"

namespace mystl {
    template<class _Key, class _Tp, class _HashFcn = hash<_Key>, class _EqualKey = equal_to<_Key>, class _Alloc = alloc>
    class hash_map {
    private:
        typedef hashtable<pair<const _Key, _Tp>, _Key, _HashFcn,
                _Select1st<pair<const _Key, _Tp> >, _EqualKey, _Alloc> _Ht;
        _Ht _M_ht;

    public:
        typedef typename _Ht::key_type key_type;
        typedef _Tp data_type;
        typedef _Tp mapped_type;
        typedef typename _Ht::value_type value_type;
        typedef typename _Ht::hasher hasher;
        typedef typename _Ht::key_equal key_equal;

        typedef typename _Ht::size_type size_type;
        typedef typename _Ht::difference_type difference_type;
        typedef typename _Ht::pointer pointer;
        typedef typename _Ht::const_pointer const_pointer;
        typedef typename _Ht::reference reference;
        typedef typename _Ht::const_reference const_reference;

        typedef typename _Ht::iterator iterator;
        typedef typename _Ht::const_iterator const_iterator;

        typedef typename _Ht::allocator_type allocator_type;

        hasher hash_funct() const { return _M_ht.hash_funct(); }

        key_equal key_eq() const { return _M_ht.key_eq(); }

        allocator_type get_allocator() const { return _M_ht.get_allocator(); }

    public:
        hash_map() : _M_ht(100, hasher(), key_equal(), allocator_type()) {}

        explicit hash_map(size_type __n)
                : _M_ht(__n, hasher(), key_equal(), allocator_type()) {}

        hash_map(size_type __n, const hasher &__hf)
                : _M_ht(__n, __hf, key_equal(), allocator_type()) {}

        hash_map(size_type __n, const hasher &__hf, const key_equal &__eql,
                 const allocator_type &__a = allocator_type())
                : _M_ht(__n, __hf, __eql, __a) {}

        hash_map(const value_type *__f, const value_type *__l)
                : _M_ht(100, hasher(), key_equal(), allocator_type()) { _M_ht.insert_unique(__f, __l); }

        hash_map(const value_type *__f, const value_type *__l, size_type __n)
                : _M_ht(__n, hasher(), key_equal(), allocator_type()) { _M_ht.insert_unique(__f, __l); }

        hash_map(const value_type *__f, const value_type *__l, size_type __n,
                 const hasher &__hf)
                : _M_ht(__n, __hf, key_equal(), allocator_type()) { _M_ht.insert_unique(__f, __l); }

        hash_map(const value_type *__f, const value_type *__l, size_type __n,
                 const hasher &__hf, const key_equal &__eql,
                 const allocator_type &__a = allocator_type())
                : _M_ht(__n, __hf, __eql, __a) { _M_ht.insert_unique(__f, __l); }

        hash_map(const_iterator __f, const_iterator __l)
                : _M_ht(100, hasher(), key_equal(), allocator_type()) { _M_ht.insert_unique(__f, __l); }

        hash_map(const_iterator __f, const_iterator __l, size_type __n)
                : _M_ht(__n, hasher(), key_equal(), allocator_type()) { _M_ht.insert_unique(__f, __l); }

        hash_map(const_iterator __f, const_iterator __l, size_type __n,
                 const hasher &__hf)
                : _M_ht(__n, __hf, key_equal(), allocator_type()) { _M_ht.insert_unique(__f, __l); }

        hash_map(const_iterator __f, const_iterator __l, size_type __n,
                 const hasher &__hf, const key_equal &__eql,
                 const allocator_type &__a = allocator_type())
                : _M_ht(__n, __hf, __eql, __a) { _M_ht.insert_unique(__f, __l); }

    public:
        size_type size() const { return _M_ht.size(); }

        size_type max_size() const { return _M_ht.max_size(); }

        bool empty() const { return _M_ht.empty(); }

        void swap(hash_map &__hs) { _M_ht.swap(__hs._M_ht); }

        template<class _K1, class _T1, class _HF, class _EqK, class _Al>
        friend bool operator==(const hash_map<_K1, _T1, _HF, _EqK, _Al> &,
                               const hash_map<_K1, _T1, _HF, _EqK, _Al> &);

        iterator begin() { return _M_ht.begin(); }

        iterator end() { return _M_ht.end(); }

        const_iterator begin() const { return _M_ht.begin(); }

        const_iterator end() const { return _M_ht.end(); }

    public:
        pair<iterator, bool> insert(const value_type &__obj) { return _M_ht.insert_unique(__obj); }

        void insert(const value_type *__f, const value_type *__l) {
            _M_ht.insert_unique(__f, __l);
        }

        void insert(const_iterator __f, const_iterator __l) { _M_ht.insert_unique(__f, __l); }

        pair<iterator, bool> insert_noresize(const value_type &__obj) { return _M_ht.insert_unique_noresize(__obj); }

        iterator find(const key_type &__key) { return _M_ht.find(__key); }

        const_iterator find(const key_type &__key) const { return _M_ht.find(__key); }

        _Tp &operator[](const key_type &__key) {
            return _M_ht.find_or_insert(value_type(__key, _Tp())).second;
        }

        size_type count(const key_type &__key) const { return _M_ht.count(__key); }

        pair<iterator, iterator> equal_range(const key_type &__key) { return _M_ht.equal_range(__key); }

        pair<const_iterator, const_iterator>
        equal_range(const key_type &__key) const { return _M_ht.equal_range(__key); }

        size_type erase(const key_type &__key) { return _M_ht.erase(__key); }

        void erase(iterator __it) { _M_ht.erase(__it); }

        void erase(iterator __f, iterator __l) { _M_ht.erase(__f, __l); }

        void clear() { _M_ht.clear(); }

        void resize(size_type __hint) { _M_ht.resize(__hint); }

        size_type bucket_count() const { return _M_ht.bucket_count(); }

        size_type max_bucket_count() const { return _M_ht.max_bucket_count(); }

        size_type elems_in_bucket(size_type __n) const { return _M_ht.elems_in_bucket(__n); }

    };

    template<class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
    inline bool
    operator==(const hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc> &__hm1,
               const hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc> &__hm2) {
        return __hm1._M_ht == __hm2._M_ht;
    }

    template<class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
    inline bool
    operator!=(const hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc> &__hm1,
               const hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc> &__hm2) {
        return !(__hm1 == __hm2);
    }

    template<class _Key, class _Tp, class _HashFcn = hash<_Key>, class _EqualKey = equal_to<_Key>, class _Alloc = alloc>
    class hash_multimap {
    private:
        typedef hashtable<pair<const _Key, _Tp>, _Key, _HashFcn,
                _Select1st<pair<const _Key, _Tp> >, _EqualKey, _Alloc> _Ht;
        _Ht _M_ht;

    public:
        typedef typename _Ht::key_type key_type;
        typedef _Tp data_type;
        typedef _Tp mapped_type;
        typedef typename _Ht::value_type value_type;
        typedef typename _Ht::hasher hasher;
        typedef typename _Ht::key_equal key_equal;

        typedef typename _Ht::size_type size_type;
        typedef typename _Ht::difference_type difference_type;
        typedef typename _Ht::pointer pointer;
        typedef typename _Ht::const_pointer const_pointer;
        typedef typename _Ht::reference reference;
        typedef typename _Ht::const_reference const_reference;

        typedef typename _Ht::iterator iterator;
        typedef typename _Ht::const_iterator const_iterator;

        typedef typename _Ht::allocator_type allocator_type;

        hasher hash_funct() const { return _M_ht.hash_funct(); }

        key_equal key_eq() const { return _M_ht.key_eq(); }

        allocator_type get_allocator() const { return _M_ht.get_allocator(); }

    public:
        hash_multimap() : _M_ht(100, hasher(), key_equal(), allocator_type()) {}

        explicit hash_multimap(size_type __n)
                : _M_ht(__n, hasher(), key_equal(), allocator_type()) {}

        hash_multimap(size_type __n, const hasher &__hf)
                : _M_ht(__n, __hf, key_equal(), allocator_type()) {}

        hash_multimap(size_type __n, const hasher &__hf, const key_equal &__eql,
                      const allocator_type &__a = allocator_type())
                : _M_ht(__n, __hf, __eql, __a) {}

        hash_multimap(const value_type *__f, const value_type *__l)
                : _M_ht(100, hasher(), key_equal(), allocator_type()) { _M_ht.insert_equal(__f, __l); }

        hash_multimap(const value_type *__f, const value_type *__l, size_type __n)
                : _M_ht(__n, hasher(), key_equal(), allocator_type()) { _M_ht.insert_equal(__f, __l); }

        hash_multimap(const value_type *__f, const value_type *__l, size_type __n,
                      const hasher &__hf)
                : _M_ht(__n, __hf, key_equal(), allocator_type()) { _M_ht.insert_equal(__f, __l); }

        hash_multimap(const value_type *__f, const value_type *__l, size_type __n,
                      const hasher &__hf, const key_equal &__eql,
                      const allocator_type &__a = allocator_type())
                : _M_ht(__n, __hf, __eql, __a) { _M_ht.insert_equal(__f, __l); }

        hash_multimap(const_iterator __f, const_iterator __l)
                : _M_ht(100, hasher(), key_equal(), allocator_type()) { _M_ht.insert_equal(__f, __l); }

        hash_multimap(const_iterator __f, const_iterator __l, size_type __n)
                : _M_ht(__n, hasher(), key_equal(), allocator_type()) { _M_ht.insert_equal(__f, __l); }

        hash_multimap(const_iterator __f, const_iterator __l, size_type __n,
                      const hasher &__hf)
                : _M_ht(__n, __hf, key_equal(), allocator_type()) { _M_ht.insert_equal(__f, __l); }

        hash_multimap(const_iterator __f, const_iterator __l, size_type __n,
                      const hasher &__hf, const key_equal &__eql,
                      const allocator_type &__a = allocator_type())
                : _M_ht(__n, __hf, __eql, __a) { _M_ht.insert_equal(__f, __l); }

    public:
        size_type size() const { return _M_ht.size(); }

        size_type max_size() const { return _M_ht.max_size(); }

        bool empty() const { return _M_ht.empty(); }

        void swap(hash_multimap &__hs) { _M_ht.swap(__hs._M_ht); }

        template<class _K1, class _T1, class _HF, class _EqK, class _Al>
        friend bool operator==(const hash_multimap<_K1, _T1, _HF, _EqK, _Al> &,
                               const hash_multimap<_K1, _T1, _HF, _EqK, _Al> &);

        iterator begin() { return _M_ht.begin(); }

        iterator end() { return _M_ht.end(); }

        const_iterator begin() const { return _M_ht.begin(); }

        const_iterator end() const { return _M_ht.end(); }

        iterator insert(const value_type &__obj) { return _M_ht.insert_equal(__obj); }

        void insert(const value_type *__f, const value_type *__l) {
            _M_ht.insert_equal(__f, __l);
        }

        void insert(const_iterator __f, const_iterator __l) { _M_ht.insert_equal(__f, __l); }

        iterator insert_noresize(const value_type &__obj) { return _M_ht.insert_equal_noresize(__obj); }

        iterator find(const key_type &__key) { return _M_ht.find(__key); }

        const_iterator find(const key_type &__key) const { return _M_ht.find(__key); }

        size_type count(const key_type &__key) const { return _M_ht.count(__key); }

        pair<iterator, iterator> equal_range(const key_type &__key) { return _M_ht.equal_range(__key); }

        pair<const_iterator, const_iterator>
        equal_range(const key_type &__key) const { return _M_ht.equal_range(__key); }

        size_type erase(const key_type &__key) { return _M_ht.erase(__key); }

        void erase(iterator __it) { _M_ht.erase(__it); }

        void erase(iterator __f, iterator __l) { _M_ht.erase(__f, __l); }

        void clear() { _M_ht.clear(); }

    public:
        void resize(size_type __hint) { _M_ht.resize(__hint); }

        size_type bucket_count() const { return _M_ht.bucket_count(); }

        size_type max_bucket_count() const { return _M_ht.max_bucket_count(); }

        size_type elems_in_bucket(size_type __n) const { return _M_ht.elems_in_bucket(__n); }
    };

    template<class _Key, class _Tp, class _HF, class _EqKey, class _Alloc>
    inline bool
    operator==(const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc> &__hm1,
               const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc> &__hm2) {
        return __hm1._M_ht == __hm2._M_ht;
    }

    template<class _Key, class _Tp, class _HF, class _EqKey, class _Alloc>
    inline bool
    operator!=(const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc> &__hm1,
               const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc> &__hm2) {
        return !(__hm1 == __hm2);
    }


}
#endif //IMPLTMENT_STL_MY_HASH_MAP_H
