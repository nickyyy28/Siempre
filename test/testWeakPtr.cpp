#include <iostream>
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

    Test::ptr ptr1(Test::getSmartPtr(30));
    Test::ptr ptr2 = ptr1;
    Test::ptr ptr3(nullptr);

    siem::weak_ptr<Test> wptr = ptr1;

    {
        Test::ptr ptr3(ptr1);
        XX(3);
    }

    XX(1);
    XX(2);

    std::cout << "weak use count: " << wptr.use_count() << std::endl;

    ptr1.reset();

    XX(1);
    XX(2);
    std::cout << "weak use count: " << wptr.use_count() << std::endl;

    ptr1 = ptr2;

    std::shared_ptr<Test> std_ptr(new Test(20));

    std::weak_ptr<Test> std_weak = std_ptr;

    LINE();

    XX(2);
    XX(3);

    ptr3.swap(ptr2);

    XX(2);
    XX(3);

    return 0;
}