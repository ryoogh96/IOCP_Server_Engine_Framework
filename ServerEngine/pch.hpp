#pragma once

#define WIN32_LEAN_AND_MEAN // except unusing things from windows header

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerEngine.lib")
#else
#pragma comment(lib, "Release\\ServerEngine.lib")
#endif

#include "ServerEnginePCH.hpp"