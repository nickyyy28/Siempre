#include <siem/siem>

int main()
{
    siem::SString str("ABCDEFGHIJK");
    siem::SString str2("12345");

    DEBUG() << "cap: " << str.capacity() << " size: " << str.size() << ", content: " << str;

    str.replace(0, str2);

    DEBUG() << "cap: " << str.capacity() << " size: " << str.size() << ", content: " << str;

    str.insert(5, "insert", 6);

    DEBUG() << "cap: " << str.capacity() << " size: " << str.size() << ", content: " << str;

    siem::SString str3("end");
    str.insert(str.size(), str3);

    DEBUG() << "cap: " << str.capacity() << " size: " << str.size() << ", content: " << str;

    siem::SString str4("front");
    str.insert(0, str4);

    DEBUG() << "cap: " << str.capacity() << " size: " << str.size() << ", content: " << str;

    siem::SString str5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDDDDDDDDD");
    str.insert(str.size(), str5);

    DEBUG() << "cap: " << str.capacity() << " size: " << str.size() << ", content: " << str;

    return 0;
}