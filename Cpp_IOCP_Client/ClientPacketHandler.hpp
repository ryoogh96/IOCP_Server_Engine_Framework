#pragma once

using namespace Engine;

enum
{
	S_TEST = 1
};

class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

	static void Handle_S_TEST(BYTE* buffer, int32 len);
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
