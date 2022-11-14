#include "pch.h"
#include "SessionManager.h"

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
}

void SessionManager::addSession(const shared_ptr<Session> sessionPtr)
{
	_sessionContainer.insert(sessionPtr);
}

void SessionManager::removeSession(const shared_ptr<Session> sessionPtr)
{
	_sessionContainer.erase(sessionPtr);
}

vector<shared_ptr<Session>> SessionManager::getSessions()
{
	vector<shared_ptr<Session>> sessions;

	{
		StdWriteLock lk(_sync);
		for (auto& session : _sessionContainer)
		{
			sessions.push_back(session);
		}
	}

	return sessions;
}
