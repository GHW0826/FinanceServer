#pragma once

template<typename T>
class LockQueue
{
public:
	void Push(T item)
	{
		USE_LOCK;
		_items.push(item);
	}

	T Pop()
	{
		WRITE_LOCK;
		if (_items.empty())
			return T();

		T item = _items.front();
		_items.pop();
		return item;
	}

	void Clear()
	{
		WRITE_LOCK;
		_items = Queue<T>();
	}

	void PopAll(OUT Vector<T>& items)
	{
		WRITE_LOCK;
		while (T item = Pop())
			items.push_back(item);
	}
private:
	USE_LOCK;
	Queue<T> _items;
};
