#include "stdafx.h"
#include "Listener.h"
#include "ServerService.h"
#include "ServerSession.h"
#include "protocol\UserProtocol.pb.h"
#include "SessionManager.h"
#include "UserSession.h"
#include "Room.h"
#include "TaskScheduler.h"

using namespace std;

static void Process();

int main(int argc, char ** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    NetUtils::initialize();
    Logger::initialize();
    MemoryPool::initialize();

    ServerSessionFactory sessionFactory = []() { return std::shared_ptr<ServerSession>(new UserSession()); };
    ServerServiceParam param(12321, 10, sessionFactory);
    ServerService service(param);
    service.Start();
    service.Run(Process);

    return 0;
}

void Process()
{
    while (true)
    {
        std::this_thread::sleep_for(5s);
    }
}