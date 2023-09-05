#pragma once
#include <vector>
#include "Core/ServerEngineTypes.hpp"


namespace Engine
{
	class FileUtils
	{
	public:
		static Vector<BYTE>		ReadFile(const WCHAR* path);
		static String			Convert(std::string str);
	};
}