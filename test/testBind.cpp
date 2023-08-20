#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include <siem/template/bind.hpp>

template<class T>
void showContainer(T& con)
{
    for (auto i = con.begin() ; i != con.end() ; ++i) {
        std::cout << *i << " ";
    }

    std::cout << std::endl;
}

int main(int argc, const char** argv)
{
    std::vector<int> data {6, 80, 4, 0, 99, 6544, 42, 67,155, 64,79210, 54,654, 485, 74, 25};

    showContainer(data);

    std::sort(data.begin(), data.end());

    showContainer(data);
    int value;

    std::cin >> value;

    auto it = siem::find_if(data.begin(), data.end(), siem::bind1st(std::less<int>(), value));

    data.insert(it, value);

    showContainer(data);

    if (it != data.end()) {
        std::cout << "value " << *it << std::endl;
    }

    return 0;
}