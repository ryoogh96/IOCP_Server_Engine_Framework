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

#include <iostream>

#include <windows.h>
#include <tchar.h>
#include <DbgHelp.h>

#define SERVER_IP          L"127.0.0.1"
#define DEFAULT_PORT       20000
#define MAX_BUF_SIZE       1024

#include "ServerEngineMacro.hpp"
#include "ServerEngineTypes.hpp"
#include "ServerEngineGlobal.hpp"

#include "Memory/RingBuffer.hpp"
#include "Memory/ReferenceCounter.hpp"
#include "Memory/Allocator.hpp"
#include "Memory/Container.hpp"
#include "Memory/MemoryPool.hpp"
#include "Memory/Memory.hpp"
#include "Memory/ObjectPool.hpp"
#include "Memory/TypeCast.hpp"


#include "Network/IOCPEvent.hpp"
#include "Network/Accepter.hpp"
#include "Network/Session.hpp"
#include "Network/Listener.hpp"
#include "Network/Connector.hpp"
#include "Network/IOCPManager.hpp"
#include "Network/ServerSocketManager.hpp"
#include "Network/ClientSocketManager.hpp"

#include "Utility/MiniDump.hpp"
#include "Utility/LockStack.hpp"
#include "Utility/LockQueue.hpp"
#include "Utility/LockFreeStack.hpp"
#include "Utility/LockFreeQueue.hpp"