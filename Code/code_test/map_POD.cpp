//
// Created by 梁磊磊 on 2023/6/10.
//


#include "my_map.h"
#include "iostream"
#include "string"

using std::string;
using std::cout;
using std::endl;
using mystl::map;
using mystl::pair;

int main() {
    map<string, int> simap;      //以string为键值，以int为实值
    simap[string("jjhou")] = 1;
    simap[string("jerry")] = 2;
    simap[string("jason")] = 3;
    simap[string("jimmy")] = 4;

    pair<const string, int> value(string("david"), 5);
    simap.insert(value);

    map<string, int>::iterator simap_iter = simap.begin();
    for (; simap_iter != simap.end(); ++simap_iter) {
        cout << simap_iter->first << ' '
             << simap_iter->second << endl;
    }

    int number = simap[string("jjhou")];
    cout << number << endl;

    map<string, int>::iterator ite1;
    ite1 = simap.find(string("mchen"));

    if (ite1 == simap.end()) {
        cout << "mchen not found" << endl;
    }

    ite1 = simap.find(string("jerry"));
    if (ite1 != simap.end()) {
        cout << "jerry found" << endl;
    }

    ite1->second = 9;
    int number2 = simap[string("jerry")];
    cout << number2 << endl;


}
