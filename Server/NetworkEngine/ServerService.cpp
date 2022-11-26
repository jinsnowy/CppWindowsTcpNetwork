#include "pch.h"
#include "ServerService.h"
#include "Listener.h"
#include "Handshake.h"
#include "SessionManager.h"
#include "TcpNetwork.h"
#include "LoginSession.h"
#include "LoginSessionManager.h"

ServerService::ServerService(const ServerServiceParam& param)
	:
	ServiceBase(std::thread::hardware_concurrency()),
	_port(param.port),
	_backLog(param.backLog),
    _sessionFactory(param.sessionFactory)
{
}

void ServerService::Start()
{
    ServiceBase::Start();

    auto loginManager = LoginSessionManager::GetInstance();
    loginManager->SetSessionFactory(_sessionFactory);
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

    config.networkFactory = [](ServiceBase& ServiceBase)
    {
        return make_shared<TcpNetwork>(ServiceBase);
    };

    auto listener = make_shared<Listener>(*this, config);
    if (!listener->start())
    {
        throw std::exception("listener start failed");
    }
}
