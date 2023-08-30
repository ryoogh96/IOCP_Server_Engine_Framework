#pragma once
namespace Engine
{
	class SendBufferChunk;

	class SendBuffer
	{
	public:
		SendBuffer(SendBufferChunkRef m_Owner, BYTE* buffer, int32 allocSize);
		~SendBuffer();

		BYTE*	Buffer() { return m_Buffer; }
		int32	WriteSize() { return m_WriteSize; }
		void	Close(uint32 writeSize);

	private:
		BYTE*				m_Buffer;
		uint32				m_AllocSize = 0;
		uint32				m_WriteSize = 0;
		SendBufferChunkRef	m_Owner;
	};

	class SendBufferChunk : public std::enable_shared_from_this<SendBufferChunk>
	{
		enum
		{
			SEND_BUFFER_CHUNK_SIZE = 6000
		};

	public:
		SendBufferChunk();
		~SendBufferChunk();

		void			Reset();
		SendBufferRef	Open(uint32 allocSize);
		void			Close(uint32 wrtieSize);

		bool	IsOpen() { return m_Open; }
		BYTE*	Buffer() { return &m_Buffer[m_UsedSize]; }
		uint32	FreeSize() { return static_cast<uint32>(m_Buffer.size()) - m_UsedSize; }

	private:
		Array<BYTE, SEND_BUFFER_CHUNK_SIZE> m_Buffer = { };
		bool								m_Open = false;
		uint32								m_UsedSize = 0;
	};

	class SendBufferManager
	{
	public:
		SendBufferRef		Open(uint32 size);
		
	private:
		SendBufferChunkRef	Pop();
		void				Push(SendBufferChunkRef buffer);

		static void			PushGlobal(SendBufferChunk* buffer);

	private:
		USE_LOCK;
		Vector<SendBufferChunkRef> m_SendBufferChunks;
	};
}