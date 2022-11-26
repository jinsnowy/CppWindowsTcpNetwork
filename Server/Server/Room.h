#pragma once
#include "TaskSerializer.h"

class Room : public TaskSerializer
{
public:
	void Enter(PlayerPtr player);
	void Leave(PlayerPtr player);
	void Broadcast(BufferSegment buffer);

private:
	map<uint64, PlayerPtr> _players;
};
