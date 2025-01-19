#define XIAOHTTP_TEST_MAIN

#include <xiaoHttp/xiaoHttp_test.h>

using namespace xiaoHttp;

int main(int argc, char **argv)
{
    xiaoLog::Logger::setLogLevel(xiaoLog::Logger::LogLevel::kTrace);
    int testStatus = test::run(argc, argv);
    return testStatus;
}