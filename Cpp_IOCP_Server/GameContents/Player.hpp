#pragma once

class Player
{
public:
	Engine::uint64			playerId = 0;
	std:: string			name;
	Protocol::PlayerType	type = Protocol::PLAYER_TYPE_NONE;
	GameSessionRef			ownerSession; // Cycle
}; 