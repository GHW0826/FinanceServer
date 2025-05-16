#pragma once


class RefCountable
{
public:
	RefCountable() : _refCount(1) {}
	virtual ~RefCountable() {}

	int32 GetRefCount() { return _refCount; }
	int32 AddRef() { return ++_refCount; }
	int32 Release() { 
		int32 refCount = --_refCount;
		if (refCount == 0)
			delete this;
		return _refCount; 
	}
protected:
	Atomic<int32> _refCount;
};

template <typename T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) { Set(ptr); }

	// Copy constructor
	TSharedPtr(const TSharedPtr& other) { Set(other._ptr); }
	// Move constructor
	TSharedPtr(TSharedPtr&& other) { _ptr = other._ptr; other._ptr = nullptr; }
	// »ó¼Ó
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	~TSharedPtr() { Release(); }

public:
	// Copy Operator	
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	// Move Operator
	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	bool operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool operator==(T* ptr) const { return _ptr == ptr; }
	bool operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool operator!=(T* ptr) const { return _ptr == ptr; }
	bool operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	T*			operator*()			{ return _ptr; }
	const T*	operator*() const	{ return _ptr; }
	operator T*			()			{ return _ptr; }
	T*			operator->()		{ return _ptr; }
	const T*	operator->() const	{ return _ptr; }

	bool IsNull() { return _ptr == nullptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (_ptr)
			_ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->Release();
			_ptr = nullptr;
		}
	}
private:
	T* _ptr = nullptr;
};