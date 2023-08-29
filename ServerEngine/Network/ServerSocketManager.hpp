#pragma once

namespace Engine
{
	class ServerSocketManager
	{
	public:
		ServerSocketManager();
		~ServerSocketManager();

		void Broadcast(SendBufferRef sendBuffer);

	private:
		void Initialize();

	private:
		USE_LOCK;
		Listener* m_Listener = nullptr;
		Connector* m_Connector = nullptr;
		IOCPManager* m_iocpManager = nullptr;
	};
}
