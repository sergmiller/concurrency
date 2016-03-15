#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
using namespace std;

struct A {
    int x;
    double y;
};

// Point f() {
//     return (3,3);
// }

int main() {
    vector <int> a = (1,2,3);
    tuple <int,string,double> x = (3, "aa", 4);
    int x;
    string y;
    double z;
    tie(x,y,z) = a;
    
}
