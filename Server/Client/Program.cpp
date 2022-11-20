#include "stdafx.h"
#include <crtdbg.h>
#include <IoService.h>
#include <TcpSocket.h>
#include <MemoryPool.h>

#include "ClientService.h"
#include "ClientSession.h"

using namespace std;

int main(int argc, char** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    NetUtils::initialize();
    Logger::initialize();
    MemoryPool::initialize();

    ClientServiceParam param(1, 1, "127.0.0.1", 12321);
    ClientService service(param);
    service.Start();
    service.Run();

    return 0;
}