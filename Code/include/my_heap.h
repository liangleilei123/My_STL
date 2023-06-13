//
// Created by 梁磊磊 on 2023/5/23.
//

#ifndef IMPLTMENT_STL_MY_HEAP_H
#define IMPLTMENT_STL_MY_HEAP_H


#include "stl_iterator.h"

namespace mystl {
    /*****************************************************************************************/
    // push_heap
    // 往大根堆插入元素，要满足两个条件：1.接受两个迭代器，用来表现一个heap底部容器的头尾
    //2.新元素已经插入到底部容器的最尾端
    //时间复杂度：最差的情况是将根节点交换到堆的最下层，为O(logn)
    /*****************************************************************************************/

    template<class RandomAccessIterator, class Distance, class T>
    void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
        Distance parent = (holeIndex - 1) / 2;            //找到父节点的位置
        while (holeIndex > topIndex && *(first + parent) < value) {           //未达到顶端且父节点小于新值（大根堆），把父节点交换到子节点
            *(first + holeIndex) = *(first + parent);       //交换父节点
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;                 //新的父节点位置
        }
        *(first + holeIndex) = value;                   //holeIndex一直表示的是空节点的位置,移动到最后的值也应插入这个位置
    }

    template<class RandomAccessIterator, class Distance, class T>
    inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance *, T *) {
        //这里指定容器的尾（新插入的元素）的位置，"根节点"，新插入的元素
        __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
    }

    template<class RandomAccessIterator>
    inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
        //此函数被调用时，新元素已置于底部容器的最尾端
        __push_heap_aux(first, last, distance_type(first),
                        value_type(first));              //distance_type(）返回指向first迭代器相应的differece_type类型的指针
    }


    /*****************************************************************************************/
    // pop_heap
    // 往大根堆移除元素，要完成两步：1.记录最后一层最右边节点值，把根节点的放在底层容器最尾端
    //2.为了满足完全二叉树的条件，要把最后一层最右边节点找一个合适的位置来填补前面的空缺
    //具体操作：删除根节点后，根节点相当于空节点。下溯过程：将空节点和其较大子节点对调，并持续下放，直至叶节点位置。这个操作结束时空节点到达叶子节点。
    //将最后一层最右边节点的元素值给上个操作的空节点，对这个节点进行上溯操作（即如果大于父节点就和父节点交换，直到不大于为止），完成程序
    //时间复杂度介于O(logn)和O(n)之间，因为下溯之后还要上溯
    /*****************************************************************************************/
    template<class RandomAccessIterator, class T, class Distance>
    void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
        Distance topIndex = holeIndex;
        Distance secondChild = 2 * holeIndex + 2;     //空节点的右子节点
        while (secondChild < len) {
            //比较空节点的左右孩子的值，然后以secondChild代表大子节点
            if (*(first + secondChild) < *(first + (secondChild - 1))) {
                secondChild--;
            }
            //对调两节点，再记录空节点的下标
            *(first + holeIndex) = *(first + secondChild);
            holeIndex = secondChild;
            //找到新空节点的右子节点
            secondChild = 2 * secondChild + 2;
            //进入下溯操作的循环
        }
        if (secondChild == len) {     //当secondChild == len时，没有右子节点，只有左子节点
            //对调空节点和其左子节点
            *(first + holeIndex) = *(first + (secondChild - 1));
            holeIndex = secondChild - 1;
        }
        //当secondChild > len时，空节点（holeIndex）即为叶子节点，直接执行上溯操作
        __push_heap(first, holeIndex, topIndex, value);

    }

    template<class RandomAccessIterator, class T, class Distance>
    inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value,
                           Distance *) {
        *result = *first;       //设定尾值为首值，于是尾值为欲求结果，可用底层容器的pop_back()移除尾值
        __adjust_heap(first, Distance(0), Distance(last - first), value);    //空节点为0，欲调整值为value

    }

    template<class RandomAccessIterator, class T>
    inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T *) {
        __pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
    }

    template<class RandomAccessIterator>
    inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
        __pop_heap_aux(first, last, value_type(first));
    }

    /*****************************************************************************************/
    // sort_heap
    // pop_heap是把键值中最大的元素放在最尾端，如果对整个heap做pop_heap操作，每次将操作范围从后往前所见一个元素，
    //当整个程序执行完毕时，就有个一个递增序列
    //注意：排序过后，原来的heap就不再是一个合法的heap了
    //时间复杂度：O(nlogn)
    /*****************************************************************************************/
    template<class RandomAccessIterator>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
        while (last - first > 1) {
            pop_heap(first, last--);
        }
    }

    /*****************************************************************************************/
    // make_heap
    // 将一段现有的数据转化为一个heap。
    // 针对每个非叶子节点调整其位置，先下溯再上溯调用pop_heap的内部函数
    // 时间负责度为：O(n)
    /*****************************************************************************************/
    template<class RandomAccessIterator, class T, class Distance>
    void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T *, Distance *) {
        if (last - first < 2) return;      //如果长度为0或为1，不必重新排列
        Distance len = last - first;
        //找到第一个需要重新排列的子树头部，进行下溯操作。所有叶子节点都不需要下采样，就找最后一个节点的父节点。相当于序列中最后一个头节点的位置
        //这里的计算不太清晰，更清晰的表达式应该是parent = ((len - 1)-1)/2。
        //len是一段数据的大小，len-1是最后一个元素的下标，对于元素下标i，求父节点的表达式是(i-1)/2。
        Distance parent = (len - 2) / 2;
        while (true) {
            //重排以parent为首的子树。len是为了让__adjust_heap()判断操作范围
            __adjust_heap(first, parent, len, T(*(first + parent)));
            if (parent == 0) return;        //走完根节点，就结束
            parent--;                           //头部向前一个节点，最后一个头节点往前都是头节点
        }
    }

    template<class RandomAccessIterator>
    inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
        __make_heap(first, last, value_type(first), distance_type(first));
    }


    ///以下实现可以指定比较方式的heap
    /*****************************************************************************************/
    // push_heap
    /*****************************************************************************************/

    template<class RandomAccessIterator, class Distance, class T, class Compare>
    void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, Compare comp) {
        Distance parent = (holeIndex - 1) / 2;            //找到父节点的位置
        while (holeIndex > topIndex && comp(*(first + parent), value)) {           //未达到顶端且父节点小于新值（大根堆），把父节点交换到子节点
            *(first + holeIndex) = *(first + parent);       //交换父节点
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;                 //新的父节点位置
        }
        *(first + holeIndex) = value;                   //holeIndex一直表示的是空节点的位置,移动到最后的值也应插入这个位置
    }

    template<class RandomAccessIterator, class Distance, class T, class Compare>
    inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Compare comp, Distance *, T *) {
        //这里指定容器的尾（新插入的元素）的位置，"根节点"，新插入的元素
        __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)), comp);
    }

    template<class RandomAccessIterator, class Compare>
    inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        //此函数被调用时，新元素已置于底部容器的最尾端
        __push_heap_aux(first, last, comp, distance_type(first),
                        value_type(first));              //distance_type(）返回指向first迭代器相应的differece_type类型的指针
    }

    /*****************************************************************************************/
    // pop_heap
    /*****************************************************************************************/
    template<class RandomAccessIterator, class T, class Distance, class Compare>
    void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, Compare comp) {
        Distance topIndex = holeIndex;
        Distance secondChild = 2 * holeIndex + 2;     //空节点的右子节点
        while (secondChild < len) {
            //比较空节点的左右孩子的值，然后以secondChild代表大子节点
            if (comp(*(first + secondChild), *(first + (secondChild - 1)))) {
                secondChild--;
            }
            //对调两节点，再记录空节点的下标
            *(first + holeIndex) = *(first + secondChild);
            holeIndex = secondChild;
            //找到新空节点的右子节点
            secondChild = 2 * secondChild + 2;
            //进入下溯操作的循环
        }
        if (secondChild == len) {     //当secondChild == len时，没有右子节点，只有左子节点
            //对调空节点和其左子节点
            *(first + holeIndex) = *(first + (secondChild - 1));
            holeIndex = secondChild - 1;
        }
        //当secondChild > len时，空节点（holeIndex）即为叶子节点，直接执行上溯操作
        __push_heap(first, holeIndex, topIndex, value, comp);

    }

    template<class RandomAccessIterator, class T, class Distance, class Compare>
    inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value,
                           Compare comp, Distance *) {
        *result = *first;       //设定尾值为首值，于是尾值为欲求结果，可用底层容器的pop_back()移除尾值
        __adjust_heap(first, Distance(0), Distance(last - first), value, comp);    //空节点为0，欲调整值为value

    }

    template<class RandomAccessIterator, class T, class Compare>
    inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T *, Compare comp) {
        __pop_heap(first, last - 1, last - 1, T(*(last - 1)), comp, distance_type(first));
    }

    template<class RandomAccessIterator, class Compare>
    inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        __pop_heap_aux(first, last, value_type(first), comp);
    }

    /*****************************************************************************************/
    // sort_heap
    /*****************************************************************************************/
    template<class RandomAccessIterator, class Compare>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        while (last - first > 1) {
            pop_heap(first, last--, comp);
        }
    }

    /*****************************************************************************************/
    // make_heap
    /*****************************************************************************************/
    template<class RandomAccessIterator, class T, class Distance, class Compare>
    void __make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp, T *, Distance *) {
        if (last - first < 2) return;      //如果长度为0或为1，不必重新排列
        Distance len = last - first;
        //找到第一个需要重新排列的子树头部，进行下溯操作。所有叶子节点都不需要下采样，就找最后一个节点的父节点。相当于序列中最后一个头节点的位置
        //这里的计算不太清晰，更清晰的表达式应该是parent = ((len - 1)-1)/2。
        //len是一段数据的大小，len-1是最后一个元素的下标，对于元素下标i，求父节点的表达式是(i-1)/2。
        Distance parent = (len - 2) / 2;
        while (true) {
            //重排以parent为首的子树。len是为了让__adjust_heap()判断操作范围
            __adjust_heap(first, parent, len, T(*(first + parent)), comp);
            if (parent == 0) return;        //走完根节点，就结束
            parent--;                           //头部向前一个节点，最后一个头节点往前都是头节点
        }
    }

    template<class RandomAccessIterator, class Compare>
    inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        __make_heap(first, last, comp, value_type(first), distance_type(first));
    }


}


#endif //IMPLTMENT_STL_MY_HEAP_H
