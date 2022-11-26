#include "stdafx.h"
#include "Room.h"
#include "Player.h"
#include "Session.h"

void Room::Enter(PlayerPtr player)
{
	_players[player->playerId] = player;

	static atomic<uint32> p(1);

	uint32 pnow = p.fetch_add(1) * 500;

	auto timer = Timer::startNew();

	RegisterAlarm(player->name, pnow, MakeTask([pnow, timer]() {
		
		auto tick = (uint32)timer.count<std::chrono::milliseconds>();

		LOG_INFO("[Alarm Test] expected : %u ms result : %u ms ", pnow, tick);

	}), false);
}

void Room::Leave(PlayerPtr player)
{
	_players.erase(player->playerId);
}

void Room::Broadcast(BufferSegment buffer)
{
	for (auto& pair : _players)
	{
		pair.second->ownerSession.SendAsync(buffer);
	}
}