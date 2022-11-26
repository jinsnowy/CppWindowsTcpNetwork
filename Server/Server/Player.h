#pragma once

class Player
{
public:
	Player(Session& sessionIn);

	uint64					playerId = 0;
	string					name;
	Session&				ownerSession;
};
