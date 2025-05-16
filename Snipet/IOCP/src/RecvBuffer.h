#pragma once

class RecvBuffer
{
	enum {
		BUFFER_COUNT = 10
	};
public:
	RecvBuffer(int32 bufferSize)
		: _capacity(bufferSize* BUFFER_COUNT), _bufferSize(bufferSize)
	{
		_buffer.resize(_capacity);
	}
	~RecvBuffer()
	{
	}

	void Clean()
	{
		int32 dataSize = DataSize();
		if (dataSize == 0) {
			// read, write 커서 동일 둘 다 리셋
			_readPos = 0;
			_writePos = 0;
		}
		else {
			// 여유 공간이 버퍼 1개 크기 미만이면, 데이터를 앞으로 땡김
			if (FreeSize() < _bufferSize) {
				::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
				_readPos = 0;
				_writePos = dataSize;
			}
		}
	}

	bool OnRead(int32 numOfBytes)
	{
		if (numOfBytes > DataSize())
			return false;

		_readPos += numOfBytes;
		return true;
	}
	bool OnWrite(int32 numOfBytes)
	{
		if (numOfBytes > FreeSize())
			return false;

		_writePos += numOfBytes;
		return true;
	}


	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	int32 DataSize() { return _writePos - _readPos; }
	int32 FreeSize() { return _capacity - _writePos; }
private:
	int32 _capacity = 0;
	int32 _bufferSize = 0;
	int32 _readPos = 0;
	int32 _writePos = 0;
	Vector<BYTE> _buffer;
};

