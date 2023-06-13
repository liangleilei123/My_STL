//
// Created by 梁磊磊 on 2023/6/2.
//

#ifndef IMPLTMENT_STL_RB_TREE_H
#define IMPLTMENT_STL_RB_TREE_H

#include "utility"
#include "stl_iterator.h"
#include "stl_alloc.h"
#include "util.h"
#include "stl_algobase.h"
#include "stl_constructor.h"
#include "my_pair.h"

namespace mystl {
    typedef bool __rb_tree_color_type;
    const __rb_tree_color_type __rb_tree_red = false;
    const __rb_tree_color_type __rb_tree_black = true;

///红黑树的节点设计

    struct __rb_tree_node_base {
        typedef __rb_tree_color_type color_type;
        typedef __rb_tree_node_base *base_ptr;

        color_type color;
        base_ptr parent;            //红黑树的许多操作，必须知道父节点
        base_ptr left;
        base_ptr right;

        static base_ptr minimum(base_ptr x) {            //找到以当前节点为根节点的最小值
            while (x->left != 0) x = x->left;
            return x;
        }

        static base_ptr maximum(base_ptr x) {           //找到以当前节点为根节点的最大值
            while (x->right != 0) x = x->right;
            return x;
        }
    };

    template<class Value>
    struct __rb_tree_node : public __rb_tree_node_base {
        typedef __rb_tree_node<Value> *link_type;
        Value value_field;                  //节点值
    };


///红黑树的迭代器

//基层迭代器
    struct __rb_tree_base_iterator {
        typedef __rb_tree_node_base::base_ptr base_ptr;
        typedef bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;

        base_ptr node;      //它用来容器之间产生一个连结关系

        //指针前进,相当于中序遍历。红黑树维持的是一个有序序列，所以指针前进就是找到第一个比当前节点值大的节点
        void increment() {
            if (node->right != 0) {   //如果右子树不为空，就找右子树中最小的节点
                node = node->right;
                while (node->left != 0) {
                    node = node->left;
                }
            } else {                       //如果右子树为空，找第一个比当前节点值大的父节点
                base_ptr y = node->parent;      //找出父节点
                while (node == y->right) {        //如果父节点本身是个右子节点，父节点比node小，继续往上找
                    node = y;                   //就一直上溯，直到"不为右子节点"止
                    y = y->parent;
                }
                if (node->right != y) {           //如果此时的右子节点不等于此时的父节点，此时的父节点即为解答
                    node = y;                   //否则此时的node为解答。
                }
                //这个判断是为了应付一种特殊情况，我们欲寻找根节点的下一个节点，而恰巧根节点无右子节点
                //以上特殊做法必须配合RB-tree根节点与特殊header之间的特殊关系
            }
        }

        //指针后退，就是找到第一个比当前节点值小的节点
        void decrement() {
            if (node->color == __rb_tree_red && node->parent->parent == node) {
                node = node->right;
            }//这个只可能发生在node为header是，（即node 为end()时）。
                //注意，header之右子节点即mostright，指向整棵树的max节点
            else if (node->left != 0) {       //如果左子节点不为空，就找左子树中值最大的节点
                base_ptr y = node->left;
                while (y->right != 0) {
                    y = y->right;
                }
                node = y;
            } else {               //即非根节点，亦无左子节点,就找第一个比当前节点值小父节点
                base_ptr y = node->parent;
                while (node == y->left) {
                    node = y;
                    y = y->parent;
                }
                node = y;
            }

        }
    };

    //RB-tree的正规迭代器
    template<class Value, class Ref, class Ptr>
    struct __rb_tree_iterator : public __rb_tree_base_iterator {
        typedef Value value_type;
        typedef Ref reference;
        typedef Ptr pointer;
        typedef __rb_tree_iterator<Value, Value &, Value *> iterator;
        typedef __rb_tree_iterator<Value, const Value &, const Value *> const_iterator;
        typedef __rb_tree_iterator<Value, Ref, Ptr> self;
        typedef __rb_tree_node<Value> *link_type;

        __rb_tree_iterator() {}

        __rb_tree_iterator(link_type x) { node = x; }

        __rb_tree_iterator(const iterator &it) { node = it.node; }

        reference operator*() const { return link_type(node)->value_field; }

        pointer operator->() const { return &(operator*()); }

        self &operator++() {
            increment();
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            increment();
            return tmp;
        }

        self &operator--() {
            decrement();
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            decrement();
            return tmp;
        }

        bool get_color() const {
            return link_type(node)->color;
        }
    };

    inline bool operator==(const __rb_tree_base_iterator &x,
                           const __rb_tree_base_iterator &y) {
        return x.node == y.node;
    }

    inline bool operator!=(const __rb_tree_base_iterator &x,
                           const __rb_tree_base_iterator &y) {
        return x.node != y.node;
    }

    ///调整红黑树的操作
    //左旋操作
    inline void __rb_tree_rotate_left(__rb_tree_node_base *x, __rb_tree_node_base *&root) {
        __rb_tree_node_base *x_r = x->right;
        x->right = x_r->left;
        if (x_r->left != 0) {               //情况1
            x_r->left->parent = x;
        }
        x_r->parent = x->parent;

        if (x == root) {
            root = x_r;
        } else if (x == x->parent->left) {
            x->parent->left = x_r;
        } else {
            x->parent->right = x_r;
        }
        x_r->left = x;
        x->parent = x_r;
    }

    //右旋操作
    inline void __rb_tree_rotate_right(__rb_tree_node_base *x, __rb_tree_node_base *&root) {
        __rb_tree_node_base *x_l = x->left;
        x->left = x_l->right;
        if (x_l->right != 0) {
            x_l->right->parent = x;
        }
        x_l->parent = x->parent;

        if (x == root) {
            root = x_l;
        } else if (x == x->parent->right) {
            x->parent->right = x_l;
        } else {
            x->parent->left = x_l;
        }
        x_l->right = x;
        x->parent = x_l;

    }

    //参数为新增节点x,和红黑树的根节点
    inline void __rb_tree_rebalance(__rb_tree_node_base *x, __rb_tree_node_base *&root) {
        x->color = __rb_tree_red;       //新节点必为红，不能有两个连续的红节点
        while (x != root && x->parent->color == __rb_tree_red) {  //如果父节点为红
            if (x->parent == x->parent->parent->left) {   //如果父节点为祖父节点的左节点，
                __rb_tree_node_base *s = x->parent->parent->right;          //令s为伯父节点
                if (s && s->color == __rb_tree_red) {     //S为红
                    x->parent->color = __rb_tree_black; //把x父节点的颜色改为黑
                    s->color = __rb_tree_black;         //把伯父节点的颜色改为黑
                    x->parent->parent->color = __rb_tree_red;   //把祖父的节点颜色改为红
                    x = x->parent->parent;
                } else {   //无伯父节点或伯父节点为黑
                    if (x == x->parent->right) {      //S为黑且内侧插入，对应状况2;要旋转两次，先左旋再右旋
                        x = x->parent;
                        __rb_tree_rotate_left(x, root);      //第一参数为左旋点
                    }
                    //S为黑，且外侧插入新节点为父节点的左节点，对应状况1，只右旋
                    x->parent->color = __rb_tree_black;     //父节点改为黑
                    x->parent->parent->color = __rb_tree_red;       //祖父节点改为红
                    __rb_tree_rotate_right(x->parent->parent, root);     //第一参数为右旋点
                }
            } else {           //父节点为祖父节点的右节点
                __rb_tree_node_base *s = x->parent->parent->left;       //那伯父节点就是祖父节点的左节点
                if (s && s->color == __rb_tree_red) {
                    x->parent->color = __rb_tree_black;
                    s->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    x = x->parent->parent;          //把x置为祖父节点
                } else {
                    if (x == x->parent->left) {  //内侧插入
                        x = x->parent;
                        __rb_tree_rotate_right(x, root);
                    }
                    x->parent->color = __rb_tree_black;
                    x->parent->parent->color = __rb_tree_red;
                    __rb_tree_rotate_left(x->parent->parent, root);
                }
            }
        }
        root->color = __rb_tree_black;

    }

    inline __rb_tree_node_base *_rb_tree_rebalance_for_erase(__rb_tree_node_base *z,
                                                             __rb_tree_node_base *&root,
                                                             __rb_tree_node_base *&leftmost,
                                                             __rb_tree_node_base *&rightmost) {
        __rb_tree_node_base *y = z;             //要删除的节点
        __rb_tree_node_base *x = 0;             //要替换的节点
        __rb_tree_node_base *x_parent = 0;
        ///标准的二叉搜索树删除流程
        //要删除节点有一个子孩子的情况
        if (y->left == 0) {
            x = y->right;
        } else {
            if (y->right == 0) {
                x = y->left;
            } else {           //要删除节点的左右孩子均有的情况
                y = y->right;
                while (y->left != 0) {              //找到右子树值最小的节点
                    y = y->left;
                }       //循环结束时，y是右子树的最小节点，将用来替换z
                x = y->right;       //x可能是null
            }
        }

        //当y != z时，要删除的节点左右孩子均有
        if (y != z) {     //用y代替z，y是z的继任者，重新链接y。
            z->left->parent = y;        //因为上面替换的是右子树的最小值，有可能是z的右孩子,所以先把z的左子树连接到y
            y->left = z->left;
            if (y != z->right) {
                x_parent = y->parent;                   //后面移除y时用到
                if (x) {    //要把y的右子树连接到y的父节点
                    x->parent = y->parent;
                }
                y->parent->left = x;
                y->right = z->right;        //用y替换z
                z->right->parent = y;
            } else {       //y是z的右孩子
                x_parent = y;
            }
            if (root == z) {
                root = y;
            } else if (z->parent->left == z) {
                z->parent->left = y;
            } else {
                z->parent->right = y;
            }
            y->parent = z->parent;
            mystl::swap(y->color, z->color);
            y = z;          //y现在指向实际要删除的节点
        } else {       //y == z;   说明只有z结点只有一个子节点x,x要替换z,且有可能是最大值或最小值
            x_parent = y->parent;
            if (x) {
                x->parent = y->parent;
            }
            if (root == z) {
                root = x;
            } else {
                if (z->parent->left == z) {
                    z->parent->left = x;
                } else {
                    z->parent->right = x;
                }
            }
            //更新header的指针
            if (leftmost == z) {            //如果z是最小值
                if (z->right == 0) {
                    leftmost = z->parent;
                } else {
                    leftmost = __rb_tree_node_base::minimum(x);
                }
            }
            if (rightmost == z) {
                if (z->left == 0) {
                    rightmost = z->parent;
                } else {
                    rightmost = __rb_tree_node_base::maximum(x);
                }
            }

        }//经过标准的二叉树删除处理后，y指向要删除的节点，且有x_parent指向y节点的父节点

        if (y->color != __rb_tree_red) {      //y为黑，移除会破坏了红黑树的条件4
            while (x != root && (x == 0 || x->color == __rb_tree_black)) {          //x是要替换y的节点，满足该条件的x是双黑节点
                if (x == x_parent->left) {      //如果x是父节点的左孩子，
                    __rb_tree_node_base *w = x_parent->right;       //w记录要删除节点的兄弟节点
                    if (w->color == __rb_tree_red) {      //兄弟节点为父节点的右节点且为红，破坏了条件4，左旋
                        w->color = __rb_tree_black;
                        x_parent->color = __rb_tree_red;
                        __rb_tree_rotate_left(x_parent, root);
                        w = x_parent->right;
                    }
                    //兄弟节点为黑，且兄弟节点的左孩子和右孩子均为黑色
                    if ((w->left == 0 || w->left->color == __rb_tree_black)
                        && (w->right == 0 || w->right->color == __rb_tree_black)) {
                        w->color = __rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;        //之后递归的进行操作
                    } else {
                        //兄弟节点的子节点有一个是红色
                        if (w->right == 0 || w->right->color == __rb_tree_black) {      //如果左子节点是红，内侧，先右旋再左旋
                            if (w->left) w->left->color = __rb_tree_black;
                            w->color = __rb_tree_red;
                            __rb_tree_rotate_right(w, root);
                            w = x_parent->right;
                        }

                        // 右节点为红
                        w->color = x_parent->color;
                        x_parent->color = __rb_tree_black;
                        if (w->right) w->right->color = __rb_tree_black;
                        __rb_tree_rotate_left(x_parent, root);
                        break;
                    }
                    //对于右节点的操作是对称的
                } else {
                    __rb_tree_node_base *w = x_parent->left;       //w记录要删除节点的兄弟节点
                    if (w->color == __rb_tree_red) {      //兄弟节点为父节点的右节点且为红，破坏了条件4，左旋
                        w->color = __rb_tree_black;
                        x_parent->color = __rb_tree_red;
                        __rb_tree_rotate_right(x_parent, root);
                        w = x_parent->left;
                    }
                    //兄弟节点为黑，且兄弟节点的左孩子和右孩子均为黑色
                    if ((w->right == 0 || w->right->color == __rb_tree_black)
                        && (w->left == 0 || w->left->color == __rb_tree_black)) {
                        w->color = __rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;        //之后递归的进行操作
                    } else {
                        //兄弟节点的子节点有一个是红色
                        if (w->left == 0 || w->left->color == __rb_tree_black) {      //如果兄弟节点的右子节点是红，内侧，先左旋再右旋
                            if (w->right) w->right->color = __rb_tree_black;
                            w->color = __rb_tree_red;
                            __rb_tree_rotate_left(w, root);
                            w = x_parent->left;
                        }

                        // 右节点为红
                        w->color = x_parent->color;
                        x_parent->color = __rb_tree_black;
                        if (w->left) w->left->color = __rb_tree_black;
                        __rb_tree_rotate_right(x_parent, root);
                        break;
                    }
                }
            }
            if (x) x->color = __rb_tree_black;

        }
        return y;

    }


    ///红黑树的数据结构
    template<class T, class Alloc=alloc>
    struct _rb_tree_base {
        typedef Alloc allocator_type;

        allocator_type get_allocator() const { return allocator_type(); }

        _rb_tree_base(const allocator_type &) : header(0) {
            header = get_node();
        }

        ~_rb_tree_base() {
            put_node(header);
        }

    protected:
        __rb_tree_node<T> *header;
        typedef simple_alloc<__rb_tree_node<T>, Alloc> alloc_type;

        __rb_tree_node<T> *get_node() {
            return alloc_type::allocate(1);
        }

        void put_node(__rb_tree_node<T> *p) {
            alloc_type::deallocate(p, 1);
        }
    };

    //Key是键值，Value是数据
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
    class _rb_tree : protected _rb_tree_base<Value, Alloc> {
        typedef _rb_tree_base<Value, Alloc> Base;
    protected:
        typedef __rb_tree_node_base *base_ptr;
        typedef __rb_tree_node<Value> rb_tree_node;
        typedef __rb_tree_color_type color_type;
    public:
        typedef Key key_type;
        typedef Value value_type;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef rb_tree_node *link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef typename Base::allocator_type allocator_type;

        allocator_type get_allocator() const { return Base::get_allocator(); }

    protected:
        using Base::header;
        using Base::get_node;
        using Base::put_node;

    protected:
        //成员数据
        size_type node_count;           //追踪树的大小
        Compare key_compare;

        //以下三个函数用来方便取得header的成员
        link_type &root() const { return (link_type &) header->parent; }

        link_type &leftmost() const { return (link_type &) header->left; }

        link_type &rightmost() const { return (link_type &) header->right; }

        //以下六个函数用来方便取得节点x的成员
        static link_type &left(link_type x) { return (link_type &) (x->left); }

        static link_type &right(link_type x) { return (link_type &) (x->right); }

        static link_type &parent(link_type x) { return (link_type &) (x->parent); }

        static reference value(link_type x) { return x->value_field; }

        static const key_type &_s_key(link_type x) { return KeyOfValue()(value(x)); }

        //KeyOfValue()表示创建一个KeyOfValue实例，并调用其函数操作符（函数重载运算符operator()）来提取节点元素中的关键字
        static color_type &color(link_type x) { return (color_type &) (x->color); }

        //以下六个函数用来方便去的节点x的成员

        static link_type &left(base_ptr x) { return (link_type &) (x->left); }

        static link_type &right(base_ptr x) { return (link_type &) (x->right); }

        static link_type &parent(base_ptr x) { return (link_type &) (x->parent); }

        static reference value(base_ptr x) { return ((link_type) x)->value_field; }

        static const key_type &_s_key(base_ptr x) { return KeyOfValue()(value(x)); }

        //KeyOfValue()表示创建一个KeyOfValue实例，并调用其函数操作符（函数重载运算符operator()）来提取节点元素中的关键字
        static color_type &color(base_ptr x) { return (color_type &) ((link_type) (x)->color); }

        //求极大值和极小值
        static link_type minimun(link_type x) { return (link_type) __rb_tree_node_base::minimum(x); }

        static link_type maximun(link_type x) { return (link_type) __rb_tree_node_base::maximum(x); }

    public:
        typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
        typedef __rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

    private:
        iterator __insert(base_ptr x, base_ptr y, const value_type &v);

        link_type copy(link_type x, link_type p);

        void _m_erase(link_type x);


    private:
        void _empty_initialize() {
            header = get_node();
            color(header) = __rb_tree_red;      //另header为红色，用来区分header和root

            root() = 0;
            leftmost() = header;            //另header的左子节点为自己
            rightmost() = header;           //另header的右子节点为自己
        }

        link_type create_node(const value_type &x) {
            link_type tmp = get_node();
            try {
                construct(&tmp->value_field, x);     //在节点内数据域位置构造数据
            }
            catch (...) {
                put_node(tmp);
            }
            return tmp;
        }

        link_type clone_node(link_type x) {
            link_type tmp = create_node(x->value_field);
            tmp->color = x->color;
            tmp->left = 0;
            tmp->right = 0;
            return tmp;
        }

        void destroy_node(link_type p) {
            destroy(&p->value_field);
            put_node(p);
        }

    public:
        _rb_tree() : Base(allocator_type()), node_count(0), key_compare() {
            _empty_initialize();
        }

        _rb_tree(const Compare &comp)
                : Base(allocator_type()), node_count(0), key_compare(comp) {
            _empty_initialize();
        }


        _rb_tree(const Compare &comp, const allocator_type &a)
                : Base(a), node_count(0), key_compare(comp) {
            _empty_initialize();
        }


        _rb_tree(const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x)
                : Base(x.get_allocator()), node_count(x.node_count), key_compare(x.key_compare) {
            if (x.root() == 0)
                _empty_initialize();
            else {
                color(header) = __rb_tree_red;
                root() = copy(x.root(), header);
                leftmost() = minimun(root());
                rightmost() = maximun(root());
            }
        }

        ~_rb_tree() { clear(); }

        _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &
        operator=(const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x);

    public:
        Compare key_comp() const { return key_compare; }

        //iterator rt() const { return root(); }

        iterator begin() { return leftmost(); }

        const_iterator begin() const { return leftmost(); }

        iterator end() { return header; }

        const_iterator end() const { return header; }           //左闭右开区间



        bool empty() const { return node_count == 0; }

        size_type size() const { return node_count; }


        size_type max_size() const { return size_type(-1); }

        void swap(_rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x) {
            mystl::swap(header, x.header);
            mystl::swap(node_count, x.node_count);
            mystl::swap(key_compare, x.key_compare);
        }

    public:
        ///insert,erase
        pair<iterator, bool> insert_unique(const value_type &x);

        iterator insert_equal(const value_type &x);

        iterator insert_unique(iterator position, const value_type &x);

        iterator insert_equal(iterator position, const value_type &x);

        void insert_unique(const_iterator first, const_iterator last);

        void insert_unique(const value_type *first, const value_type *last);

        void insert_equal(const_iterator first, const_iterator last);

        void insert_equal(const value_type *first, const value_type *last);


        void erase(iterator position);

        size_type erase(const key_type &x);

        void erase(iterator first, iterator last);

        void erase(const key_type *first, const key_type *last);

        void clear() {
            if (node_count != 0) {
                _m_erase(root());
                leftmost() = header;
                root() = 0;
                rightmost() = header;
                node_count = 0;
            }
        }

    public:
        ///set operations
        iterator find(const key_type &x);

        const_iterator find(const key_type &x) const;

        size_type count(const key_type &x) const;

        iterator lower_bound(const key_type &x);

        const_iterator lower_bound(const key_type &x) const;

        iterator upper_bound(const key_type &x);

        const_iterator upper_bound(const key_type &x) const;

        pair<iterator, iterator> equal_range(const key_type &x);

        pair<const_iterator, const_iterator> equal_range(const key_type &x) const;

    public:
        ///debugging
        bool __rb_verify() const;

    };

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline bool operator==(const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x,
                           const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &y) {
        return x.size == y.size() && equal(x.begin(), x.end(), y.begin());
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline bool operator<(const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x,
                          const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &y) {
        return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline bool operator!=(const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x,
                           const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &y) {
        return !(x == y);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline bool operator>(const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x,
                          const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &y) {
        return y < x;
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline bool operator<=(const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x,
                           const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &y) {
        return !(y < x);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline bool operator>=(const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x,
                           const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &y) {
        return !(x < y);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline void swap(const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x,
                     const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &y) {
        x.swap(y);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::operator=(
            const _rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x) {
        if (this != x) {
            clear();
            node_count = 0;
            key_compare = x.key_compare;
            if (x.node_count == 0) {
                root() = 0;
                leftmost() = header;
                rightmost() = header;
            } else {
                root() = copy(x.root(), header);
                leftmost() = minimun(root());
                rightmost() = maximun(root());
                node_count = x.node_count;
            }
        }
        return *this;
    }

    ///深拷贝
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::link_type
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::copy(link_type x, link_type p) {      //x是指向原始树根节点的指针，p是指向复制树中根节点的父节点的指针
        link_type top = clone_node(x);
        top->parent = p;

        try {
            if (x->right) {
                top->right = copy(right(x), top);
            }
            p = top;
            x = left(x);

            while (x != 0) {
                link_type y = clone_node(x);
                p->left = y;
                y->parent = p;
                if (x->right) {
                    y->right = copy(right(x), y);
                }
                p = y;
                x = left(x);
            }
        }
        catch (...) {
            _m_erase(top);
        }
        return top;         //返回复制后树的根节点
    }

    ///insert
    //真正的插入程序，创建新节点并更新相关指针
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::__insert(base_ptr _x, base_ptr _y, const value_type &_v) {       //三个参数：x为新插入点，y为插入点的父节点，v为新值
        link_type x = (link_type) _x;
        link_type y = (link_type) _y;
        link_type z;

        //新插入节点的父节点等于头节点 || 要插入的值在红黑树中已存在 || 新插入点的键值小于父节点y的键值
        //为什么x != 0时，left(y) = z，传到这里的x参数不可能 != 0
        if (_y == header || x != 0 || key_compare(KeyOfValue()(_v), _s_key(y))) {
            z = create_node(_v);
            left(y) = z;            //插入到红黑树的左子树，

            //并更新相关指针
            if (y == header) {
                root() = z;
                rightmost() = z;
            } else if (y == leftmost()) {
                leftmost() = z;
            }
        } else {        //父节点不为头节点，并且新节点不存在于红黑树中
            z = create_node(_v);
            right(y) = z;
            if (y == rightmost()) {
                rightmost() = z;
            }
        }
        parent(z) = y;          //设定新节点的父节点
        left(z) = 0;
        right(z) = 0;
        __rb_tree_rebalance(z, header->parent);
        ++node_count;
        return iterator(z);

    }

    //insert_equal，插入新值，节点键值允许重复。往下找到一个空的叶子节点，插入
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::insert_equal(const Value &v) {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            y = x;
            //取两个节点的键值进行比较:v的键值小于x的键值，往左，v的键值大于x的键值，往右
            x = key_compare(KeyOfValue()(v), _s_key(x)) ? left(x) : right(x);
        }
        //三个新参数为，x为新值的插入点，参数y为插入点的父节点，参数v为新值
        return __insert(x, y, v);
    }

    //insert_unique,插入新值，节点键值允许重复，若重复则插入无效
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    pair<typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::insert_unique(const Value &v) {
        link_type y = header;
        link_type x = root();
        bool comp = true;
        while (x != 0) {
            y = x;
            comp = key_compare(KeyOfValue()(v), _s_key(x));
            x = comp ? left(x) : right(x);
        }//离开while循环后，y所指即插入点的父节点（此时，y节点必为叶子节点）
        iterator j = iterator(y);       //令迭代器j指向插入点的父节点y，其值是查找过程中最后一个比插入元素小的节点的迭代器
        if (comp) {                     //如果离开while循环时，comp为true，说明插入元素应该放在红黑树的最左端
            if (j == begin()) {         //说明红黑树为空，或者插入元素比红黑树中所有元素都要小
                return pair<iterator, bool>(__insert(x, y, v), true);      //插入到最左端
            } else {
                --j;        //将迭代器减一，使其指向最后一个比插入元素小的节点
            }
        }
        //最后一个比插入元素小的节点不等于 begin 迭代器，并且该节点的元素值不等于插入元素的值，
        // 则说明插入元素在红黑树中不存在，此时调用 __insert 函数插入元素
        if (key_compare(_s_key(j.node), KeyOfValue()(v))) {
            return pair<iterator, bool>(__insert(x, y, v), true);
        }
        //如果 _M_key_compare(_S_key(__j._M_node), _KeyOfValue()(__v)) 的值为 false，
        // 说明最后一个比插入元素小的节点的元素值等于插入元素的值，此时不能将插入元素插入红黑树中。
        //返回最后一个比插入元素小的节点的迭代器和 false。
        return pair<iterator, bool>(j, false);

    }

    //说是指定位置插入，但还得符合迭代器的性质
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::insert_unique(iterator position, const Value &v) {
        if (position.node == header->left) {        //position = begin()
            if (size() > 0 && key_compare(KeyOfValue()(v), _s_key(position.node))) {
                return __insert(position.node, position.node, v);           //表示把v插入到position的前面
            } else {
                return insert_unique(v).first;
            }
        } else if (position.node == header) {           //position = end()
            if (key_compare(_s_key(position.node), KeyOfValue()(v))) {
                return __insert(0, rightmost(), v);
            } else {
                return insert_unique(v).first;
            }
        } else {
            iterator before = position;
            --before;
            if (key_compare(_s_key(before.node), KeyOfValue()(v))             //v的值大于position的父节点，小于position
                && key_compare(KeyOfValue()(v), _s_key(position.node))) {
                if (right(before.node) == 0) {
                    return __insert(0, before.node, v);
                } else {
                    return __insert(position.node, position.node, v);
                }
            } else {
                return insert_unique(v).first;
            }
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::insert_equal(iterator position, const Value &v) {
        if (position.node == header->left) {        //position = begin()
            if (size() > 0 && key_compare(KeyOfValue()(v), _s_key(position.node))) {
                return __insert(position.node, position.node, v);           //表示把v插入到position的前面
            } else {
                return insert_equal(v);
            }
        } else if (position.node == header) {           //position = end()
            if (key_compare(_s_key(position.node), KeyOfValue()(v))) {
                return __insert(0, rightmost(), v);
            } else {
                return insert_equal(v);
            }
        } else {
            iterator before = position;
            --before;
            if (key_compare(_s_key(before.node), KeyOfValue()(v))             //v的值大于position的父节点，小于position
                && key_compare(KeyOfValue()(v), _s_key(position.node))) {
                if (right(before.node) == 0) {
                    return __insert(0, before.node, v);
                } else {
                    return __insert(position.node, position.node, v);
                }
            } else {
                return insert_equal(v);
            }
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::insert_equal(const Value *first, const Value *last) {
        for (; first != last; ++first) {
            insert_equal(*first);
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::insert_equal(const_iterator first, const_iterator last) {
        for (; first != last; ++first) {
            insert_equal(*first);
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::insert_unique(const Value *first, const Value *last) {
        for (; first != last; ++first) {
            insert_unique(*first);
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::insert_unique(const_iterator first, const_iterator last) {
        for (; first != last; ++first) {
            insert_unique(*first);
        }
    }


    ///erase
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline void _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::erase(iterator position) {
        link_type y = (link_type) _rb_tree_rebalance_for_erase(position.node,
                                                               header->parent,
                                                               header->left,
                                                               header->right);
        destroy_node(y);        //y为能安全删除的节点
        --node_count;
    }


    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::erase(const Key &x) {
        pair<iterator, iterator> p = equal_range(x);
        size_type n = 0;
        n = distance(p.first, p.second);
        erase(p.first, p.second);
        return n;
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::_m_erase(link_type x) {       //删除这个节点及其子树
        while (x != 0) {
            _m_erase(right(x));
            link_type y = left(x);
            destroy_node(x);
            x = y;
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::erase(iterator first, iterator last) {
        if (first == begin() && last == end()) {
            clear();
        } else {
            while (first != last) erase(first++);
        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline void _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::erase(const Key *first, const Key *last) {
        while (first != last) erase(*first++);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    pair<typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,
            typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator>
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::equal_range(const key_type &x) {
        return pair<iterator, iterator>(lower_bound(x), upper_bound(x));
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    pair<typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator,
            typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator>
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::equal_range(const key_type &x) const {
        return pair<const_iterator, const_iterator>(lower_bound(x), upper_bound(x));
    }

    //该函数的作用是在红黑树中查找第一个不小于给定键值的元素，并返回其迭代器。
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::lower_bound(const key_type &k) {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (!key_compare(_s_key(x), k)) {
                y = x;
                x = left(x);
            } else {
                x = right(x);
            }
        }
        return iterator(y);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::lower_bound(const key_type &k) const {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (!key_compare(_s_key(x), k)) {
                y = x;
                x = left(x);
            } else {
                x = right(x);
            }
        }
        return const_iterator(y);
    }

    //该函数的作用是在红黑树中查找第一个大于给定键值的元素，并返回其迭代器。
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::upper_bound(const key_type &k) {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (key_compare(k, _s_key(x))) {
                y = x;
                x = left(x);
            } else {
                x = right(x);
            }
        }
        return iterator(y);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::upper_bound(const key_type &k) const {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (key_compare(k, _s_key(x))) {
                y = x;
                x = left(x);
            } else {
                x = right(x);
            }
        }
        return const_iterator(y);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::count(const Key &k) const {
        pair<const_iterator, const_iterator> p = equal_range(k);
        size_type n = 0;
        n = distance(p.first, p.second);
        return n;
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::find(const Key &k) {
        link_type y = header;
        link_type x = root();

        while (x != 0) {
            if (!key_compare(_s_key(x), k)) {
                y = x;
                x = left(x);
            } else {
                x = right(x);
            }
        }
        iterator j = iterator(y);
        return (j == end() || key_compare(k, _s_key(j.node))) ? end() : j;

    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
    _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::find(const Key &k) const {
        link_type y = header;
        link_type x = root();

        while (x != 0) {
            if (!key_compare(_s_key(x), k)) {
                y = x;
                x = left(x);
            } else {
                x = right(x);
            }
        }
        const_iterator j = const_iterator(y);
        return (j == end() || key_compare(k, _s_key(j.node))) ? end() : j;

    }

    ///计算从给定节点到红黑树根节点路径上的黑色节点数量。
    inline int __black_count(__rb_tree_node_base *node, __rb_tree_node_base *root) {
        if (node == 0)
            return 0;
        else {
            int bc = node->color = __rb_tree_black ? 1 : 0;
            if (node == root) {
                return bc;
            } else {
                return bc + __black_count(node->parent, root);
            }

        }
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    bool _rb_tree<Key, Value, KeyOfValue, Compare, Alloc>
    ::__rb_verify() const {
        if (node_count == 0 || begin() == end()) {
            return node_count == 0 && begin() == end() &&
                   header->left == header && header->right == header;
        }       //空树是红黑树

        int len = __black_count(leftmost(), root());
        for (const_iterator it = begin(); it != end(); ++it) {
            link_type x = (link_type) it.node;
            link_type l = left(x);
            link_type r = right(x);

            if (x->color == __rb_tree_red) {        //条件3，如果节点为红，其子节点必须是黑色
                if ((l && l->color == __rb_tree_red) || (r && r->lolor == __rb_tree_red))
                    return false;
            }

            //不符合二叉搜索树
            if (l && key_compare(_s_key(x), _s_key(l)))
                return false;
            if (r && key_compare(_s_key(r), _s_key(x)))
                return false;
            //条件4 ：任一节点至 NULL（树尾端）的任何路径，所含之黑节点数必须相同。
            if (!l && !r && __black_count((x, root()) != len))
                return false;
        }

        if (leftmost() != __rb_tree_node_base::minimum(root()))
            return false;
        if (rightmost() != __rb_tree_node_base::maximum(root()))
            return false;

        return true;

    }

}


#endif //IMPLTMENT_STL_RB_TREE_H
