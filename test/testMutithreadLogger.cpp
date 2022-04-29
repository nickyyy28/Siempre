#include <siem>

int main(void)
{
    std::vector<siem::Thread::ptr> ths;
    for (size_t i = 0 ; i < 10 ; ++i) {
        ths.push_back(siem::Thread::ptr(new siem::Thread([=](){
            int cnt = 500;
            while (cnt--)
            {
                LOG_INFO(GET_LOG_BY_NAME(system)) << "logger in thread " << std::to_string(i) << " cnt: " << cnt;
            }
        }, "thread_" + std::to_string(i))));
    }

    for (auto& th : ths) {
        th->join();
    }



    return 0;
}
