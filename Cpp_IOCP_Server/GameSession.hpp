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
	virtual void OnRecvPacket(BYTE* buffer, Engine::int32 len) override;
	virtual void OnSend(Engine::int32 len) override;

public:
	Engine:: Vector<PlayerRef> m_Players;
};