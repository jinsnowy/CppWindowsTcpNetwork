#pragma once
#include "ServerSession.h"
#include "Player.h"

class UserSession : public ServerSession
{
public:
	UserSession();

	virtual void onConnected();

	virtual void onAuthorized();

	PlayerPtr GetPlayer() { return _player; }

	void SetPlayer(UserProtocol::Player player);
private:
	PlayerPtr _player;
};
