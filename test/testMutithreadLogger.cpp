#include <siem/siem>

int main(void)
{
    std::vector<siem::Thread::ptr> ths;
    // for (size_t i = 0 ; i < 10 ; ++i) {
        
    // }

    int cnt = 500;
    siem::RWMutex mutex;

    ths.push_back(siem::Thread::ptr(new siem::Thread([&](){
        while (true)
        {
            siem::RWMutex::WriteLock lock(mutex);
            if (cnt <= 0) break;
            LOG_INFO(GET_LOG_BY_NAME(system)) << "logger in thread info, cnt: " << cnt;
            cnt --;
        }
    }, "thread_info")));

    ths.push_back(siem::Thread::ptr(new siem::Thread([&](){
        while (true)
        {
            siem::RWMutex::WriteLock lock(mutex);
            if (cnt <= 0) break;
            LOG_DEBUG(GET_LOG_BY_NAME(system)) << "logger in thread debug, cnt: " << cnt;
            cnt --;
        }
    }, "thread_debug")));

    ths.push_back(siem::Thread::ptr(new siem::Thread([&](){
        while (true)
        {
            siem::RWMutex::WriteLock lock(mutex);
            if (cnt <= 0) break;
            LOG_WARN(GET_LOG_BY_NAME(system)) << "logger in thread warning, cnt: " << cnt;
            cnt --;
        }
    }, "thread_warning")));

    ths.push_back(siem::Thread::ptr(new siem::Thread([&](){
        while (true)
        {
            siem::RWMutex::WriteLock lock(mutex);
            if (cnt <= 0) break;
            LOG_ERROR(GET_LOG_BY_NAME(system)) << "logger in thread error, cnt: " << cnt;
            cnt --;
        }
    }, "thread_error")));

    ths.push_back(siem::Thread::ptr(new siem::Thread([&](){
        while (true)
        {
            if (cnt <= 0) break;
            siem::RWMutex::WriteLock lock(mutex);
            LOG_FATAL(GET_LOG_BY_NAME(system)) << "logger in thread fatal, cnt: " << cnt;
            cnt --;
        }
    }, "thread_fatal")));

    for (auto& th : ths) {
        th->join();
    }



    return 0;
}
