#include <iostream>
#include <siem/siem>
#include <memory>
#include <siem/template/shared_ptr.hpp>


class Test{
public:
    typedef siem::shared_ptr<Test> ptr;

    Test(int val = 0){
        m_val = val;
    }
    ~Test(){}

    int getValue() const 
    {
        return m_val;
    }

    void setValue(int val) {
        m_val = val;
    }

    void test()
    {
        std::cout << "Test::test()" << std::endl;
    }

    explicit operator bool() const noexcept
    {
        if (m_val > 50) {
            return true;
        } else {
            return false;
        }
    }

    static Test::ptr getSmartPtr(int var)
    {
        return Test::ptr(new Test(var));
    }

private:
    int m_val;
};

int main()
{

#define LINE()   \
    std::cout << "---------------------------------------" << std::endl

#define XX(__index__)   \
    if (ptr##__index__) {   \
        std::cout << "Line:" << __LINE__ << " ptr"#__index__" use count: " << ptr##__index__.use_count() << " value: " << ptr##__index__->getValue() << std::endl;  \
    } else {    \
        std::cout << "Line:" << __LINE__ << " ptr"#__index__" is not a valid pointer but use count: " << ptr##__index__.use_count() << std::endl; \
    }

    siem::shared_ptr<Test> ptr1;
    siem::shared_ptr<Test> ptr2(new Test(20));
    Test::ptr ptr3(ptr1);
    Test::ptr ptr4(ptr2);
    Test::ptr ptr5 = ptr2;
    Test::ptr ptr6(nullptr);
    Test::ptr ptr7;
    ptr7 = ptr6;

    XX(1);
    XX(2);
    XX(3);
    XX(4);
    XX(5);
    XX(6);
    XX(7);

    ptr7 = ptr5;

    XX(7);

    ptr6.reset(new Test(30));
    XX(6);

    ptr6.reset(ptr2);
    XX(6);

    LINE();

    ptr6.reset();
    XX(6);
    XX(2);

    return 0;
}