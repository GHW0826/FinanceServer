#pragma once

class SendBufferChunk;

class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize)
		: _owner(owner), _buffer(buffer), _allocSize(allocSize)
	{
	}
	~SendBuffer()
	{
	}
public:
	BYTE* Buffer() { return _buffer; }
	uint32 AllocSize() { return _allocSize; }
	uint32 WriteSize() { return _writeSize; }
public:
	void Close(uint32 writeSize)
	{
		ASSERT_CRASH(_allocSize >= writeSize);
		_writeSize = writeSize;
		_owner->Close(writeSize);
	}
private:
	BYTE* _buffer;
	uint32 _allocSize = 0;
	uint32 _writeSize = 0;
	SendBufferChunkRef _owner;
};


class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum {
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk()
	{
	}
	~SendBufferChunk()
	{
	}
public:
	bool IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	uint32 FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }
public:
	void Reset()
	{
		_open = false;
		_usedSize = 0;
	}
	SendBufferRef Open(uint32 allocSize)
	{
		ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
		ASSERT_CRASH(_open == false);

		if (allocSize > FreeSize())
			return nullptr;

		_open = true;
		return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
	}
	void Close(uint32 writeSize)
	{
		ASSERT_CRASH(_open == true);
		_open = false;
		_usedSize += writeSize;
	}
private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer = {};
	bool _open = false;
	uint32 _usedSize = 0;
};

class SendBufferManager
{
public:
	SendBufferRef Open(uint32 size)
	{
		if (LSendBufferChunk == nullptr) {
			LSendBufferChunk = Pop(); // WRITE Lock
			LSendBufferChunk->Reset();
		}

		ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

		// 다 썻으면 버리고 새거로 교체
		if (LSendBufferChunk->FreeSize() < size) {
			LSendBufferChunk = Pop();
			LSendBufferChunk->Reset();
		}

		return LSendBufferChunk->Open(size);
	}
private:
	SendBufferChunkRef Pop()
	{
		{
			WRITE_LOCK;
			if (_sendBufferChunks.empty() == false) {
				SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
				_sendBufferChunks.pop_back();
				return sendBufferChunk;
			}
		}

		return SendBufferChunkRef(snew<SendBufferChunk>(), PushGlobal);
	}
	void Push(SendBufferChunkRef buffer)
	{
		WRITE_LOCK;
		_sendBufferChunks.push_back(buffer);
	}
	static void PushGlobal(SendBufferChunk* buffer)
	{
		GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
	}
private:
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks;
};

