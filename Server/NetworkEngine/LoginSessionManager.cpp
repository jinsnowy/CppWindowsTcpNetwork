#include "pch.h"
#include "LoginSessionManager.h"
#include "LoginSession.h"
#include "SessionManager.h"
#include "ServerSession.h"
#include "TcpNetwork.h"

LoginSessionManager::~LoginSessionManager()
{
    destroy();
}

shared_ptr<LoginSession> LoginSessionManager::NewLogin()
{
    auto login = make_shared<LoginSession>();

    {
        LockGuard lock(_mtx);
        _logins.emplace(login->GetSessionId(), login);
    }

    return login;
}

void LoginSessionManager::Start()
{
    _finished = false;
    _worker = std::thread([this]()
    {
        checkLogins();
    });
}

void LoginSessionManager::SetSessionFactory(ServerSessionFactory sessionFactory)
{
    _sessionFactory = sessionFactory;
}

void LoginSessionManager::Authorize(shared_ptr<LoginSession> loginSession)
{
    SessionID sessionId = loginSession->GetSessionId();
    removeSession(sessionId);

    auto network = loginSession->GetNetwork();
    auto serverSession = _sessionFactory();
    serverSession->SetSessionId(sessionId);

    network->AttachSession(serverSession);
    serverSession->onAuthorized();

    SessionManager::GetInstance()->addSession(serverSession);

    LOG_INFO("Authorize %llu", sessionId);
}

void LoginSessionManager::checkLogins()
{
    while (!_finished)
    {
        UniqueLock lk(_mtx);

        _cv.wait_for(lk, std::chrono::seconds(60), [this]() { return _finished; });

        if (_finished)
            return;

        std::vector<shared_ptr<LoginSession>> logins;
        {
            std::transform(_logins.begin(), _logins.end(), std::back_inserter(logins), [](const auto& pair)
            {
                return pair.second;
            });
        }

        lk.unlock();

        time_t now = time(NULL);

        std::vector<int64> removed;
        for (auto& session : logins)
        {
            if (session->IsValid(now, 60) == false)
            {
                session->DisconnectAsync();
                removed.push_back(session->GetSessionId());
            }
        }

        for (auto& removeId : removed)
        {
            removeSession(removeId);
        }
    }
}

void LoginSessionManager::destroy()
{
    {
        LockGuard lk(_mtx);
        _finished = true;
    }

    _cv.notify_one();

    if (_worker.joinable())
    {
        _worker.join();
    }
}

bool LoginSessionManager::existSession(SessionID sessionId)
{
    LockGuard lk(_mtx);
    return _logins.find(sessionId) != _logins.end();
}

void LoginSessionManager::removeSession(shared_ptr<LoginSession> session)
{
    LockGuard lk(_mtx);

    if (_logins.erase(session->GetSessionId()) > 0)
        return;

    auto iter = std::find_if(_logins.begin(), _logins.end(), [session](auto& pair)
    {
        return pair.second == session;
    });

    if (iter != _logins.end())
    {
        _logins.erase(iter);
    }
}

void LoginSessionManager::removeSession(SessionID loginId)
{
    LockGuard lk(_mtx);

    _logins.erase(loginId);
}
