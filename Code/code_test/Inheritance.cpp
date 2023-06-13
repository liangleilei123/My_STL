//
// Created by 梁磊磊 on 2023/5/25.
//

///测试c++的继承方式
#include "string"
#include "iostream"

using namespace std;


class Base {
public:
    string m_Base_pub = "m_Base_Pub";
protected:
    string m_Base_pro = "m_Base_pro";
private:
    string m_Base_pri = "m_Base_pri";
};

class Derivation_pub : public Base {
public:
    void show() {
        cout << this->m_Base_pub << endl;
        cout << this->m_Base_pro << endl;
        //cout<<this->m_Base_pri<<endl;             //子类中不能访问基类的私有成员
    }

};

class Derivation_pro : protected Base {
public:
    void show() {
        cout << this->m_Base_pub << endl;
        cout << this->m_Base_pro << endl;
        //cout<<this->m_Base_pri<<endl;             //子类中不能访问基类的私有成员
    }

};

class Derivation_pri : private Base {
public:
    void show() {
        cout << this->m_Base_pub << endl;
        cout << this->m_Base_pro << endl;
        //cout<<this->m_Base_pri<<endl;             //子类中不能访问基类的私有成员
    }

};

int main() {
    Base b;

    cout << "-------------基类测试--------------" << endl;
    cout << b.m_Base_pub << endl;       //类外可以访问类的公有数据成员
    //cout << b.m_Base_pro << endl;     //类外不能访问类的保护数据成员
    //cout<< b.m_Base_pri<<endl;             //类外不能访问类的私有数据成员

    //公有继承方式的测试
    cout << "--------------public继承------------" << endl;
    Derivation_pub Dpub;
    Dpub.show();            //类内访问基类的公有和保护数据成员测试，可以
    cout << Dpub.m_Base_pub << endl;            //类外只能访问基类的公有数据成员

    //保护继承方式的测试
    cout << "-------------protected继承-------------" << endl;
    Derivation_pro Dpro;
    Dpro.show();
    //cout<<Dpro.m_Base_pub<<endl;                  //出错，protected继承方式，基类的public和protected数据成员在子类中的权限都是protected,类外不能访问


    cout << "-------------private继承-------------" << endl;
    Derivation_pri Dpri;
    Dpri.show();
    //cout<<Dpro.m_Base_pub<<endl;                  //出错，private继承方式，基类的public和protected数据成员在子类中的权限都是private,类外不能访问

    return 0;
}