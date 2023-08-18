#pragma once

#include "Core/ServerEnginePCH.hpp"

namespace Engine
{
	class ServerSocketManager
	{
	public:
		ServerSocketManager();
		~ServerSocketManager();

		void Broadcast();

	private:
		void Initialize();

	private:
		Listener* m_Listener = nullptr;
		Connector* m_Connector = nullptr;
		IOCPManager* m_iocpManager = nullptr;

		
	};
}
