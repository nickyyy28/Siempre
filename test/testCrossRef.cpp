#include "siem/template/shared_ptr.hpp"
#include <iostream>
#include <memory>

/**
 * @brief 智能指针的交叉引用未解决
 * 
 */

class A;
class B;

using std::cout;
using std::endl;

class A{
public:
    A() { cout << "class A 构造" << endl; }
    ~A() { cout << "class A 析构" << endl; }

    siem::weak_ptr<B> ptr;
};

class B{
public:
    B() { cout << "class B 构造" << endl; }
    ~B() { cout << "class B 析构" << endl; }

    siem::weak_ptr<A> ptr;
};

int main(int argc, const char** argv)
{
    siem::shared_ptr<A> varA(new A());
    siem::shared_ptr<B> varB(new B());

    varA->ptr = varB;
    varB->ptr = varA;

    return 0;
}