#include "pch.hpp"
#include "ServerPacketHandler.hpp"
#include "Utility/BufferReader.hpp"
#include "Utility/BufferWriter.hpp"

using namespace std;
using namespace Engine;

void ServerPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br.Peek(&header);

	switch (header.id)
	{
	default:
		break;
	}
}