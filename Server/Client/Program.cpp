#include "stdafx.h"
#include <crtdbg.h>
#include <TcpSocket.h>
#include <MemoryPool.h>

#include "ClientService.h"
#include "PlayerSession.h"
#include "UserProtocol.h"

using namespace std;

int main(int argc, char** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    NetUtils::initialize();
    Logger::initialize();
    MemoryPool::initialize();

    ClientSessionFactory sessionFactory = []() { return shared_ptr<ClientSession>(new PlayerSession()); };
    ClientServiceParam param(100, 1, "127.0.0.1", 12321, sessionFactory);
    ClientService service(param);
    service.Start();
    service.Run([&]() 
    {
        while (1)
        {
            UserProtocol::TEST test;
            test.set_text("hello world!");

            service.Broadcast(BufferSource::Sink(test));

            std::this_thread::sleep_for(200ms);
        }
    });

    return 0;
}