//
// Created by 梁磊磊 on 2023/4/20.
//

#include "iostream"


//list使用iterator的例子

//先定义一个list模版类

//list节点
template<typename T>
class ListItem {
public:
    ListItem() : _value(0), _next(nullptr) {

    }

    ListItem(T value) : _value(value) {

    }

    T _value;
    ListItem *_next;        //单向列表
};

//单链表
template<typename T>
class list {
public:
    list() : _size(0) {

    }

    void insert_front(T value) {

    }

    void insert_end(T value) {
//        ListItem<T> temp = ListItem<T>(value);
//        if (_front == _end) {
//            _front = new ListItem<T>();
//            _front->_value = temp._value;
//            _front->_next = temp._next;
//        } else {
//            _end->_next = temp._next;
//        }
//        ++_end;
//        ++_size;
    }

    void display(std::ostream &os = std::cout) const {
//        for (ListItem<T> *i = _front; i != _end; ++i) {
//            os << i->_value << std::endl;
//        }
    }

private:
    ListItem<T> *_end;
    ListItem<T> *_front;
    long _size;

};



//定义一个list_iter模版类
/*内部声明一个Item类型的指针，然后用构造函数接收，类内部对 * -> ++() ()++ == != 运算符进行重载 */
template<typename Item>
struct ListIter {
    Item *ptr;

    ListIter(Item *p = 0) : ptr(p) {}

    ListIter &operator*() const {
        return *ptr;
    }

    ListIter *operator->() const {
        return ptr;
    }

    //重载前置++运算符返回引用类型，因为他会修改被操作对象并返回对象本身的引用。
    ListIter &operator++() {
        ptr = ptr->next;
        return *this;
    }

    //后置++运算符通常返回值类型，参数中的int用于区分前置++和后置++
    ListIter operator++(int) {
        ListIter tmp = *this;
        *this++;
        return tmp;
    }

    bool operator==(const ListIter &tmp) {
        return ptr == tmp.ptr;
    }

    bool operator!=(const ListIter &tmp) {
        return ptr != tmp.ptr;
    }

};


//测试iter
int main() {

    list<int> mylist;

    for (int i = 0; i < 5; ++i) {
        mylist.insert_front(i);
        mylist.insert_end(i + 2);
    }
    mylist.display();

//    ListIter<ListItem<int> > begin(mylist.front());


    return 0;
}