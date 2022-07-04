#include <iostream>
#include <base/SString.h>
#include <ostream>

class Test{
public:
    Test(int val = 0){
        i = val;
        str = siem::SString("Test Construct");
    }

    int getVal() {return i;}
    siem::SString getStr() {return str;}

private:
    int i;
    siem::SString str;
};

std::ostream& operator<<(std::ostream& os, Test t)
{
    os << t.getVal();
    return os;
}

siem::SString fun()
{
    return siem::SString("hello fun");
}

siem::SStringList getList()
{
    siem::SStringList list;
    list.addString("hello");
    list.addString("world");
    return list;
}

int main()
{
    Test t;

    // std::coroutine cp;

#define XX(__index__)   \
    std::cout << "str"#__index__" SString size: " << str##__index__.size() << " cap: " << str##__index__.capacity() << " value: " << str##__index__ << std::endl

    siem::SStringList list = getList();

    for (auto& i: list) {
        std::cout << i << std::endl;
    }

    siem::SString str1 = list[0];
    siem::SString str2 = list[0];

    siem::SString str3 = t.getStr();
    siem::SString str4 = t.getStr();

    XX(1);
    XX(2);
    XX(3);
    XX(4);

    siem::SString str5("20");

    int value = siem::SString::toVariable<int>(str5);

    std::cout << "value " << value << std::endl;

    return 0;
}

