#pragma once
#include "Session.h"

class LoginSession : public Session
{
public:
	LoginSession();

	bool IsValid(time_t now, int elapsed) { return now - _createTime < elapsed; }

protected:
	void makeSessionId();

	virtual void onConnected();
	virtual void onDisconnected();

private:
	time_t _createTime;
};
