#include <siem>

int main(int argc, char** argv)
{
    siem::Fiber::ptr cur = siem::Fiber::getThis();

    std::cout << cur->getCurFiberID();

    return 0;
}