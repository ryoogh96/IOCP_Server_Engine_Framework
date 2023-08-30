#pragma once
#include "Network/Session.hpp"

class GameSession : public Engine::PacketSession
{
public:
	~GameSession()
	{
		std::cout << "~GameSession" << std::endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual Engine::int32 OnRecvPacket(BYTE* buffer, Engine::int32 len) override;
	virtual void OnSend(Engine::int32 len) override;
};