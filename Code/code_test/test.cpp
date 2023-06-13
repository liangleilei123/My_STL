//
// Created by 梁磊磊 on 2023/4/2.
//

#include "iostream"

using namespace std;

int main() {
    int *p = new int[0];
    p[0] = 1;
    cout << p[0] << endl;
    cout << p << endl;

    return 0;
}
