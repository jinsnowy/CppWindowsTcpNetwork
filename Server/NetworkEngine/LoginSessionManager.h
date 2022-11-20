#pragma once
#include "Singleton.h"

class LoginSession;
class LoginSessionManager : public ISingleton<LoginSessionManager>
{
	friend class LoginSession;
public:
	~LoginSessionManager();

	shared_ptr<LoginSession> NewLogin();

	void Start();

	void Authorize(shared_ptr<LoginSession> session);
private:
	bool _finished;
	Mutex _mtx;
	CondVar _cv;
	Thread _worker;

	void checkLogins();
	void destroy();
	
	bool existSession(SessionID sessionId);
	void removeSession(shared_ptr<LoginSession> session);
	void removeSession(SessionID loginId);
private:
	StdLock _sync;
	std::unordered_map<SessionID, std::shared_ptr<LoginSession>> _logins;
};