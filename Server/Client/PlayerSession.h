#include "ClientSession.h"
#include "UserProtocol.h"

class PlayerSession : public ClientSession
{
private:
	UserProtocol::Player _player;

public:
	virtual void onConnected();

	virtual void onAuthorized();

	void SetPlayer(UserProtocol::Player player) { _player = player; }
};
