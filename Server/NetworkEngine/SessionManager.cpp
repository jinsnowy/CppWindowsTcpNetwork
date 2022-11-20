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
	_sessionContainer.emplace(sessionPtr->GetSessionId(),sessionPtr);
}

void SessionManager::removeSession(const shared_ptr<Session> sessionPtr)
{
	_sessionContainer.erase(sessionPtr->GetSessionId());
}

vector<shared_ptr<Session>> SessionManager::getSessions()
{
	vector<shared_ptr<Session>> sessions;

	{
		StdWriteLock lk(_sync);
		for (auto& pair : _sessionContainer)
		{
			sessions.push_back(pair.second);
		}
	}

	return sessions;
}
