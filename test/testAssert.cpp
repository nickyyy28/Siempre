#include <siem/siem>
#include <siem/common/macro.h>

int main(void)
{
    SIEM_ASSERT_STR(0, "hello world");

    while (true)
    {
        std::cout << "... ";
        std::cout.flush();
        sleep(1);
    }
    

    return 0;
}