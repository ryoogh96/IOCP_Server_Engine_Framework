#include "pch.hpp"

#include "IOCPManager.hpp"
#include "IOCPEvent.hpp"

namespace Engine
{
	IOCPManager::IOCPManager()
	{
		m_IOCPHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		ASSERT_CRASH(m_IOCPHandle != INVALID_HANDLE_VALUE);
	}

	IOCPManager::~IOCPManager()
	{
		::CloseHandle(m_IOCPHandle);
	}

	bool IOCPManager::Register(IOCPObjectRef iocpObject)
	{
		return ::CreateIoCompletionPort(iocpObject->GetHandle(), m_IOCPHandle, 0, 0);
	}

	bool IOCPManager::Dispatch(uint32 timeoutMs)
	{
		DWORD numOfBytes = 0;
		ULONG_PTR key = 0;
		IOCPEvent* iocpEvent = nullptr;

		if (::GetQueuedCompletionStatus(m_IOCPHandle, OUT & numOfBytes, OUT & key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
		{
			IOCPObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
		}
		else
		{
			int32 errCode = ::WSAGetLastError();
			switch (errCode)
			{
			case WAIT_TIMEOUT:
				return false;
			default:
				IOCPObjectRef iocpObject = iocpEvent->owner;
				iocpObject->Dispatch(iocpEvent, numOfBytes);
				break;
			}
		}
	}
}