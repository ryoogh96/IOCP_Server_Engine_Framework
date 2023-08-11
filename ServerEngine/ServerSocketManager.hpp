#pragma once

#include "ServerEnginePCH.hpp"

namespace Engine
{
	class ServerSocketManager
	{
	public:
		ServerSocketManager();
		~ServerSocketManager();

		void Broadcast();

	private:
		Listener* m_Listener = nullptr;
		Connector* m_Connector = nullptr;
		IOCPManager* m_iocpManager = nullptr;

		void Initialize();
	};
}
