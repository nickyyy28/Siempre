#include <siem/common/singleton.h>
#include <string>
#include <iostream>
#include <memory>

int main()
{
    std::string* ptr1 = siem::SingleTon<std::string>::getInstance();
    std::string* ptr2 = siem::SingleTon<std::string>::getInstance();

    if (ptr1 == ptr2) {
        std::cout << "普通单例模式成功" << std::endl;
    } else {
        std::cout << "普通单例模式失败" << std::endl;
    }

    std::shared_ptr<std::string> ptr3 = siem::SingleTonPtr<std::string>::getInstancePtr();

    std::cout << "ptr3 的引用计数:" << ptr3.use_count() << std::endl;

    std::shared_ptr<std::string> ptr4 = siem::SingleTonPtr<std::string>::getInstancePtr();

    std::cout << "ptr3 的引用计数:" << ptr3.use_count() << std::endl;

    return 0;
}