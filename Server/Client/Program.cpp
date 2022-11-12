#include "stdafx.h"
#include <crtdbg.h>
#include <IoService.h>
#include <TcpSocket.h>
#include <MemoryPool.h>

using namespace std;

int main(int argc, char** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    NetUtils::initialize();
    Logger::initialize();
    MemoryPool::initialize();


    return 0;
}