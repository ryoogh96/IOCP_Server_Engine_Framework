#pragma once

#include "IOCPManager.hpp"
#include "NetAddress.hpp"

class AcceptEvnet;
class ServerService;

namespace Engine
{
	class Listener : public IOCPObject
	{
	public:
		Listener() = default;
		~Listener();

	public:
		bool StartAccept(ServerServiceRef service);
		void CloseSocket();

	public:
		virtual HANDLE GetHandle() override;
		virtual void Dispatch(class IOCPEvent* iocpEvent, int32 numOfBytes = 0) override;

	private:
		void RegisterAccept(AcceptEvent* acceptEvent);
		void ProcessAccept(AcceptEvent* acceptEvent);

	protected:
		SOCKET m_Socket = INVALID_SOCKET;
		Vector<AcceptEvent*> m_AcceptEvents;
		ServerServiceRef m_Service;
	};
}