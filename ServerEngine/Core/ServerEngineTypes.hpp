#pragma once

namespace Engine
{
	using BYTE = unsigned char;
	using int8 = __int8;
	using int16 = __int16;
	using int32 = __int32;
	using int64 = __int64;
	using uint8 = unsigned __int8;
	using uint16 = unsigned __int16;
	using uint32 = unsigned __int32;
	using uint64 = unsigned __int64;

	template<typename T>
	using Atomic = std::atomic<T>;
	using Mutex = std::mutex;
	using CondVar = std::condition_variable;
	using UniqueLock = std::unique_lock<std::mutex>;
	using LockGuard = std::lock_guard<std::mutex>;

	#define USING_SHARED_PTR(name)	using name##Ref = std::shared_ptr<class name>;

	USING_SHARED_PTR(Session);
	USING_SHARED_PTR(SendBuffer);
	USING_SHARED_PTR(IOCPObject);
	USING_SHARED_PTR(IOCPManager);
	USING_SHARED_PTR(SendBufferChunk);
	USING_SHARED_PTR(PacketSession);
	USING_SHARED_PTR(Listener);
	USING_SHARED_PTR(ServerService);
	USING_SHARED_PTR(ClientService);
	USING_SHARED_PTR(Job);

	#define size16(value)	static_cast<int16>(sizeof(value))
	#define size32(value)	static_cast<int32>(sizeof(value))
	#define len16(arr) static_cast<int16>(sizeof(arr)/sizeof(arr[0]))
	#define len32(arr) static_cast<int32>(sizeof(arr)/sizeof(arr[0]))


	#define _STOMP
}