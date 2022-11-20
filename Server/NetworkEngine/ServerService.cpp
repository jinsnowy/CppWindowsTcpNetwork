#include "pch.h"
#include "ServerService.h"
#include "Listener.h"
#include "Handshake.h"
#include "SessionManager.h"
#include "TcpNetwork.h"
#include "LoginSession.h"
#include "LoginSessionManager.h"
#include "protocol\UserProtocol.pb.h"

ServerService::ServerService(const ServerServiceParam& param)
	:
	IoService(std::thread::hardware_concurrency()),
	_port(param.port),
	_backLog(param.backLog)
{
}

void ServerService::Start()
{
    IoService::Start();

    auto loginManager = LoginSessionManager::getInstance();

    loginManager->Start();

    ListenerConfig config;
    config.bindPort = 12321;
    config.backLog = 10;
    config.acceptCount = 1;
    config.onAccept = [this, loginManager](const shared_ptr<TcpNetwork>& network)
    {
        auto loginSession = loginManager->NewLogin();
        network->AttachSession(loginSession);

        return true;
    };

    config.networkFactory = [](IoService& ioService)
    {
        return make_shared<TcpNetwork>(ioService);
    };

    auto listener = make_shared<Listener>(*this, config);
    if (!listener->start())
    {
        throw std::exception("listener start failed");
    }
}

void ServerService::Run()
{
    IoService::Run();

    UserProtocol::TEST test;
    test.set_text("Hello World\n");

    while (true)
    {

        auto sessions = SessionManager::getInstance()->getSessions();

        for (auto& session : sessions)
        {
            BufferSegment segment = BufferSource::Sink(test);

            session->SendAsync(segment);
        }

        std::this_thread::sleep_for(100ms);
    }
}
