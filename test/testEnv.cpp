
#include <siem/siem>

using namespace siem;

int main(int argc, char** argv)
{
    GET_LOG_BY_NAME(root);
    siem::LoggerMgr::getInstance()->setRootFormat("[%p%T%d] <thread: %t>\nFile: %f, Line: %l\nMessage: %m");
    siem::Thread::setThisName("main");

    SingleTonPtr<Env>::getInstancePtr()->addHelp("h", "help");
    SingleTonPtr<Env>::getInstancePtr()->addHelp("path", "the config yaml file path");

    SingleTonPtr<Env>::getInstancePtr()->init(argc, argv);

    // std::cout << "thread name" << Thread::getThisName() << std::endl;

    //siem::Thread::ptr p(new siem::Thread())

    if (GET_ENV()->has("p")) {
        std::cout << "p: " << GET_ENV()->get("p") << std::endl;
    }

    // GET_ENV()->printArgs();

    return 0;
}
