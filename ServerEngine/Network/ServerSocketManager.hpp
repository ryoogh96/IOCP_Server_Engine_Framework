#pragma once

namespace Engine
{
	class ServerSocketManager
	{
	public:
		ServerSocketManager();
		~ServerSocketManager();

		void Broadcast(BYTE* buffer, int32 len);

	private:
		void Initialize();

	private:
		USE_LOCK;
		Listener* m_Listener = nullptr;
		Connector* m_Connector = nullptr;
		IOCPManager* m_iocpManager = nullptr;
	};
}
