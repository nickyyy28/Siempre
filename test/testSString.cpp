#include <iostream>
#include <ostream>
#include "base/SString.h"
#include "siem"

int main()
{
    /*std::string str = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCDDDDDDDDDDDDDDDDDDDDDDD449999999999999999999999999999994sddddddddddddddddgggggggggggggggssssssssssshhhhhhtdhdhfghdghdf48df4g5dh4df6th8f8h7f6hfthf896h+8d46gsd8g4d6fgdfh46fdt7h8d4f68hd6h8dg4h6dh4d4d86DD\0";

    siem::SString str2(str);
    siem::SString str3("hello");
    siem::SString str4(" world");
    siem::SString str5;

    str5 = str3 + str4;
    siem::SString str6(str3 + str4);
    siem::SString str7 = str5;
    str3 += str2;

    str7.resize(7);
    siem::SString str8(str5);
    str8.append("app1").append("app2").append(str2);
    siem::SString str9 = str2.substr(255, 130);

    std::cout << "std::string size: " << str.size() << " cap: " << str.capacity() << " value: " << str << std::endl;
    std::cout << "str2 SString size: " << str2.size() << " cap: " << str2.capacity() << " value: " << str2 << std::endl;
    std::cout << "str3 SString size: " << str3.size() << " cap: " << str3.capacity() << " value: " << str3 << std::endl;
    std::cout << "str4 SString size: " << str4.size() << " cap: " << str4.capacity() << " value: " << str4 << std::endl;
    std::cout << "str5 SString size: " << str5.size() << " cap: " << str5.capacity() << " value: " << str5 << std::endl;
    std::cout << "str6 SString size: " << str6.size() << " cap: " << str6.capacity() << " value: " << str6 << std::endl;
    std::cout << "char at pos 10 is " << str6[10] << std::endl;
    std::cout << "str7 SString size: " << str7.size() << " cap: " << str7.capacity() << " value: " << str7 << std::endl;
    std::cout << "str8 SString size: " << str8.size() << " cap: " << str8.capacity() << " value: " << str8 << std::endl;
    std::cout << "str9 SString size: " << str9.size() << " cap: " << str9.capacity() << " value: " << str9 << std::endl;
*/

    siem::SString str1("abcdefg");

    siem::SString str2 = str1.substr(1, 4);
    siem::SString str3;
    str3 = str1.substr(1, 4);

#define XX(__index__)   \
    std::cout << "str"#__index__" SString size: " << str##__index__.size() << " cap: " << str##__index__.capacity() << " value: " << str##__index__ << std::endl

    std::cout << "-------------------------" << std::endl;

    XX(1);
    XX(2);
    XX(3);

    std::cout << "-------------------------" << std::endl;

    return 0;
}

