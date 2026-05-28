#include<iostream>
using namespace std;
class complex {
int a,b;
public:
//complex();
void printnumber(){
cout<<a<<"+"<<b<<"i"<<endl;
}
};

complex::complex(){
  //  a=10;
    //b=0;
}
int main() {
    complex c;
    c.printnumber();
     return 0;
}