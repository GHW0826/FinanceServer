#pragma once

class BufferWriter
{
public:
	BufferWriter()
	{
	}
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0)
		: _buffer(buffer), _size(size), _pos(pos)
	{
	}
	~BufferWriter()
	{
	}
public:
	BYTE* Buffer() { return _buffer; }
	uint32 Size() { return _size; }
	uint32 WriteSize() { return _pos; }
	uint32 FreeSize() { return _size - _pos; }
public:
	template<typename T>
	bool Write(T* src) { return Write(src, sizeof(T)); };
	bool Write(void* src, uint32 len)
	{
		if (FreeSize() < len)
			return false;

		::memcpy(&_buffer[_pos], src, len);
		_pos += len;
		return true;
	}

	template<typename T>
	T* Reserve(uint16 count = 1)
	{
		if (FreeSize() < count * sizeof(T))
			return nullptr;

		T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
		_pos += (count * sizeof(T));
		return ret;
	}

	template<typename T>
	BufferWriter& operator<<(T&& src)
	{
		using DataType = std::remove_reference_t<T>;
		*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(src);
		_pos += sizeof(T);
		return *this;
	}
private:
	BYTE* _buffer = nullptr;
	uint32 _size = 0;
	uint32 _pos = 0;
};
