#include "stdafx.h"
#include "Listener.h"
#include "ServerService.h"
#include "ServerSession.h"

using namespace std;

int main(int argc, char ** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    NetUtils::initialize();
    Logger::initialize();
    MemoryPool::initialize();

    ServerServiceParam param(12321, 10);
    ServerService service(param);
    service.Start();
    service.Run();

    return 0;
}