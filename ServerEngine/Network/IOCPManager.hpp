#pragma once

namespace Engine
{
	class IOCPObject : public std::enable_shared_from_this<IOCPObject>
	{
	public:
		virtual HANDLE GetHandle() abstract;
		virtual void Dispatch(class IOCPEvent* iocpEvent, int32 numOfBytes = 0) abstract;
	};

	class IOCPManager
	{
	public:
		IOCPManager();
		~IOCPManager();

		HANDLE GetHandle() { return m_IOCPHandle; }

		bool Register(IOCPObjectRef iocpObject);
		bool Dispatch(uint32 timeoutMs = INFINITE);
	private:
		HANDLE	m_IOCPHandle;
	};
}