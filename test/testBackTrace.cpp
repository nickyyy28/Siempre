#include <siem/siem>

void func1();
void func2();
void func3();

std::vector<std::string> vec;

int main(int argc, char** argv)
{
    func1();
    for(auto& i : vec) {
        std::cout << i;
    }

    return 0;
}

void func1()
{
    func2();
}
void func2()
{
    func3();
}

void func3()
{
    std::string str = siem::BackTraceToString(10, 0);
    std::cout << "trace:\n" << str;
}