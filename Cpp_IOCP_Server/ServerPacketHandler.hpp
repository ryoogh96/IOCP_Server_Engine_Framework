#pragma once

using namespace std;
using namespace Engine;

enum
{
	S_TEST = 1
};

template<typename T, typename C>
class PacketIterator
{
public:
	PacketIterator(C& container, uint16 index) : m_Container(container), m_Index(index) { }

	bool				operator!=(const PacketIterator& other) const { return m_Index != other.m_Index; }
	const T& operator*() const { return m_Container[m_Index]; }
	T& operator*() { return m_Container[m_Index]; }
	T* operator->() { return &m_Container[m_Index]; }
	PacketIterator& operator++() { m_Index++; return *this; }
	PacketIterator		operator++(int32) { PacketIterator ret = *this; ++m_Index; return ret; }

private:
	C& m_Container;
	uint16			m_Index;
};

template<typename T>
class PacketList
{
public:
	PacketList() : m_Data(nullptr), m_Count(0) { }
	PacketList(T* data, uint16 count) : m_Data(data), m_Count(count) { }

	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < m_Count);
		return m_Data[index];
	}

	uint16 Count() { return m_Count; }

	// ranged-base for
	PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
	PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, m_Count); }

private:
	T* m_Data;
	uint16		m_Count;
};

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

};

#pragma pack(1)

// [ PKT_S_TEST ][BuffsListItem BuffsListItem BuffsListItem][victim victim][victim victim]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;

		// Victim List
		uint16 victimsOffset;
		uint16 victimsCount;
	};

	uint16 packetSize; // common header
	uint16 packetId; // common header
	uint64 id; // 8
	uint32 hp; // 4
	uint16 attack; // 2
	uint16 buffsOffset;
	uint16 buffsCount;
};

// [ PKT_S_TEST ][BuffsListItem BuffsListItem BuffsListItem][victim victim]
class PKT_S_TEST_WRITE
{
public:
	using BuffsListItem = PKT_S_TEST::BuffsListItem;
	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using BuffsVictimsList = PacketList<uint64>;

	PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 attack)
	{
		m_SendBuffer = GSendBufferManager->Open(4096);
		m_BufferWriter = BufferWriter(m_SendBuffer->Buffer(), m_SendBuffer->AllocSize());

		m_Pkt = m_BufferWriter.Reserve<PKT_S_TEST>();
		m_Pkt->packetSize = 0; // To Fill
		m_Pkt->packetId = S_TEST;
		m_Pkt->id = id;
		m_Pkt->hp = hp;
		m_Pkt->attack = attack;
		m_Pkt->buffsOffset = 0; // To Fill
		m_Pkt->buffsCount = 0; // To Fill
	}

	BuffsList ReserveBuffsList(uint16 buffCount)
	{
		BuffsListItem* firstBuffsListItem = m_BufferWriter.Reserve<BuffsListItem>(buffCount);
		m_Pkt->buffsOffset = (uint64)firstBuffsListItem - (uint64)m_Pkt;
		m_Pkt->buffsCount = buffCount;
		return BuffsList(firstBuffsListItem, buffCount);
	}

	BuffsVictimsList ReserveBuffsVictimsList(BuffsListItem* buffsItem, uint16 victimsCount)
	{
		uint64* firstVictimsListItem = m_BufferWriter.Reserve<uint64>(victimsCount);
		buffsItem->victimsOffset = (uint64)firstVictimsListItem - (uint64)m_Pkt;
		buffsItem->victimsCount = victimsCount;
		return BuffsVictimsList(firstVictimsListItem, victimsCount);
	}

	SendBufferRef CloseAndReturn()
	{
		m_Pkt->packetSize = m_BufferWriter.WriteSize();

		m_SendBuffer->Close(m_BufferWriter.WriteSize());
		return m_SendBuffer;
	}

private:
	PKT_S_TEST* m_Pkt = nullptr;
	SendBufferRef m_SendBuffer;
	BufferWriter m_BufferWriter;
};


#pragma pack()