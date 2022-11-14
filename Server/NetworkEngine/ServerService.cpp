#include "pch.h"
#include "ServerService.h"
#include "Listener.h"
#include "SessionManager.h"

ServerService::ServerService(const ServerServiceParam& param)
	:
	IoService(std::thread::hardware_concurrency()),
	_sessionFactory(param.sessionFactory),
    _onAccept(param.onAccept),
	_port(param.port),
	_backLog(param.backLog)
{
}

void ServerService::start()
{
    IoService::start();

    ListenerConfig config;
    config.bindPort = 12321;
    config.backLog = 10;
    config.acceptCount = 1;
    config.onAccept = _onAccept;
    config.sessionFactory = _sessionFactory;

    auto listener = make_shared<Listener>(*this, config);
    if (!listener->start())
    {
        throw std::exception("listener start failed");
    }
}

void ServerService::run()
{
    char buf[] = "Hello World\n";

    while (true)
    {
        auto sessions = SessionManager::getInstance()->getSessions();

        for (auto& session : sessions)
        {
            BufferSegment segment = BufferSource::Sink(buf, strlen(buf));

            session->sendAsync(segment);
        }

        std::this_thread::sleep_for(100ms);
    }
}
