#include "stdafx.h"
#include "Listener.h"
#include "ServerService.h"
#include "ServerSession.h"

using namespace std;

StdLock mtx;
vector<SessionPtr> sessions;

void addSession(SessionPtr session)
{
    StdWriteLock lk(mtx);
    sessions.push_back(std::move(session));
}

int main(int argc, char ** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    NetUtils::initialize();
    Logger::initialize();
    MemoryPool::initialize();

    SessionFactory sessionFactory = [](IoService& ioService) { return make_shared<ServerSession>(ioService); };
    OnAcceptFunc onAccept = [](SessionPtr) { return true; };
    ServerServiceParam param(sessionFactory, onAccept, 12321, 10);
    ServerService service(param);
    service.start();
    service.run();

    return 0;
}