#include "Singleton.h"
#include "Session.h"

class SessionManager : public ISingleton<SessionManager>
{
public:
	SessionManager();
	~SessionManager();

	void addSession(const shared_ptr<Session> sessionPtr);

	void removeSession(const shared_ptr<Session> sessionPtr);

	vector<shared_ptr<Session>> getSessions();

private:
	StdMutex _sync;
	std::map<SessionID, shared_ptr<Session>> _sessionContainer;
};
