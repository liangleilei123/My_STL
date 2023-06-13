//
// Created by 梁磊磊 on 2023/6/10.
//

#ifndef IMPLTMENT_STL_MY_PAIR_H
#define IMPLTMENT_STL_MY_PAIR_H

namespace mystl {
    template<class T1, class T2>
    struct pair {
        typedef T1 first_type;
        typedef T2 second_type;

        T1 first;
        T2 second;

        pair() : first(T1()), second(T2()) {}

        pair(const T1 &a, const T2 &b) : first(a), second(b) {}

    };

    template<class T1, class T2>
    inline bool operator==(const pair<T1, T2> &x, const pair<T1, T2> &y) {
        return x.first == y.first && x.second == y.second;
    }

    template<class T1, class T2>
    inline bool operator<(const pair<T1, T2> &x, const pair<T1, T2> &y) {
        return x.first < y.first && x.second < y.second;
    }

    template<class T1, class T2>
    inline bool operator!=(const pair<T1, T2> &x, const pair<T1, T2> &y) {
        return !(x == y);
    }

    template<class T1, class T2>
    inline bool operator>(const pair<T1, T2> &x, const pair<T1, T2> &y) {
        return y.first < x.first && y.second == x.second;
    }

    template<class T1, class T2>
    inline bool operator<=(const pair<T1, T2> &x, const pair<T1, T2> &y) {
        return !(y < x);
    }

    template<class T1, class T2>
    inline bool operator>=(const pair<T1, T2> &x, const pair<T1, T2> &y) {
        return !(x < y);
    }

    template<class T1, class T2>
    inline pair<T1, T2> make_pair(const T1 &x, const T2 &y) {
        return pair<T1, T2>(x, y);
    }


}

#endif //IMPLTMENT_STL_MY_PAIR_H
