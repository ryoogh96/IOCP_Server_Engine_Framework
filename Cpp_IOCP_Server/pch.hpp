#pragma once

#define WIN32_LEAN_AND_MEAN // except unusing things from windows header

#ifdef _DEBUG
#pragma comment(lib, "ServerEngine\\Debug\\ServerEngine.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerEngine\\Release\\ServerEngine.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "Core/ServerEnginePCH.hpp"
#include "Protocol/Enum.pb.h"

using GameSessionRef = std::shared_ptr<class GameSession>;
using PlayerRef = std::shared_ptr<class Player>;