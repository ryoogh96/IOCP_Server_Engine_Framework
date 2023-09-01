#pragma once

#define  _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include <Winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <Mswsock.h>

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <algorithm>

#include <atomic>
#include <thread>
#include <mutex>

#include <memory>

#include <functional>
#include <utility>

#include <windows.h>
#include <iostream>

#include <windows.h>
#include <tchar.h>
#include <DbgHelp.h>

#include "ServerEngineMacro.hpp"
#include "ServerEngineTypes.hpp"
#include "ServerEngineGlobal.hpp"
#include "ServerEngineTLS.hpp"

#include "Thread/ThreadManager.hpp"
#include "Thread/ReadWriteSpinLock.hpp"
#include "Thread/DeadLockProfiler.hpp"

#include "Memory/RingBuffer.hpp"
#include "Memory/ReferenceCounter.hpp"
#include "Memory/Allocator.hpp"
#include "Memory/Container.hpp"
#include "Memory/MemoryPool.hpp"
#include "Memory/Memory.hpp"
#include "Memory/ObjectPool.hpp"
#include "Memory/TypeCast.hpp"

#include "Network/NetAddress.hpp"
#include "Network/RecvBuffer.hpp"
#include "Network/SendBuffer.hpp"
#include "Network/IOCPEvent.hpp"
#include "Network/SocketManager.hpp"
#include "Network/Session.hpp"
#include "Network/Listener.hpp"
#include "Network/IOCPManager.hpp"
#include "Network/Service.hpp"

#include "Utility/MiniDump.hpp"
#include "Utility/LockStack.hpp"
#include "Utility/LockQueue.hpp"
#include "Utility/LockFreeStack.hpp"
#include "Utility/LockFreeQueue.hpp"
#include "Utility/BufferReader.hpp"
#include "Utility/BufferWriter.hpp"

#include "Job/Job.hpp"
#include "Job/JobQueue.hpp"
#include "Job/JobSerializer.hpp"