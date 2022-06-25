#include <iostream>
#include <functional>
#include <memory>
#include <siem>
#include <string>

#include "base/SString.h"
#include "template/functional.hpp"

void hello(void)
{
    std::cout << "hello world" << std::endl;
}

int add(int a, int b)
{
    return a + b;
}


class Test{
public:
    Test(const siem::SString& str)
    {
        m_str = str;
    }

    Test(const std::string& str)
    {
        m_str = siem::SString(str);
    }

    void print()
    {
        std::cout << m_str << std::endl;
    }

private:
    siem::SString m_str;

};

int main(int argc, const char** argv)
{
    siem::function<void(void)> func1(hello);

    siem::function<int(int, int)> func2 = add;

    func1();
    std::reinterpret_pointer_cast()

    std::cout << func2(10, 20) << std::endl;

    Test test("hello Test function");

    // siem::function<void(Test*)> func4 = &Test::print;

    // std::function<void(Test*)> func3 = &Test::print;

    // func3(&test);

    return 0;
}