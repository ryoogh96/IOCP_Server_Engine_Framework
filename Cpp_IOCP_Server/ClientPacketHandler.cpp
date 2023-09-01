#include "pch.hpp"
#include "ClientPacketHandler.hpp"
#include "GameContents/Player.hpp"
#include "GameContents/Room.hpp"
#include "GameSession.hpp"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	static Atomic<uint64> idGenerator = 1;

	{
		auto player = loginPkt.add_players();
		player->set_name(u8"Jone Doe");
		player->set_playertype(Protocol::PLAYER_TYPE_KNIGHT);

		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->m_Players.push_back(playerRef);
	}

	{
		auto player = loginPkt.add_players();
		player->set_name(u8"Á¸ µµ");
		player->set_playertype(Protocol::PLAYER_TYPE_MAGE);

		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->m_Players.push_back(playerRef);
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	uint64 index = pkt.playerindex();

	PlayerRef player = gameSession->m_Players[index];

	//GRoom.PushJob(MakeShared<EnterJob>(GRoom, player));
	GRoom->PushJob(&Room::Enter, player);

	Protocol::S_ENTER_GAME enterGamePkt;
	enterGamePkt.set_success(true);
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
	player->ownerSession->Send(sendBuffer);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	//GRoom.PushJob(MakeShared<BroadcastJob>(GRoom, sendBuffer));
	GRoom->PushJob(&Room::Broadcast, sendBuffer);

	return true;
}
