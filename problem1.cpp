#include <iostream>
using namespace std;
class ObjectCounter {
private:
static int counter; // Static member to hold the count of objects
public:
ObjectCounter() {
counter++; // Increment count in constructor
}
// Static function to return the count of objects
static int getObjectCount() {
return counter;
}
};
// Initialize static member
int ObjectCounter::counter = 0;
int main() {
ObjectCounter obj1;
ObjectCounter obj2;
ObjectCounter obj3;
cout << "Total number of objects created:" << ObjectCounter::getObjectCount() <<endl;
return 0;
}
