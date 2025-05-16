#pragma once
#include <mutex>
#include <vector>
#include "DBConnection.h"

class DBConnectionPool
{
public:
	DBConnectionPool()
	{
	}

	~DBConnectionPool()
	{
		Clear();
	}

	bool Connect(int connectionCount, const WCHAR* connectionString)
	{
		std::lock_guard<std::mutex> lg(_m);

		if (::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_hEnv) != SQL_SUCCESS)
			return false;

		if (::SQLSetEnvAttr(_hEnv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0) != SQL_SUCCESS)
			return false;

		for (int i = 0; i < connectionCount; ++i) {
			DBConnection* connection = new DBConnection;
			if (connection->Connect(_hEnv, connectionString) == false)
				return false;
		}

		return true;
	}
	void Clear()
	{
		std::lock_guard<std::mutex> lg(_m);

		if (_hEnv != SQL_NULL_HANDLE) {
			::SQLFreeHandle(SQL_HANDLE_ENV, _hEnv);
			_hEnv = SQL_NULL_HANDLE;
		}

		for (DBConnection* connection : _connections) {
			if (connection) {
				delete connection;
				connection = nullptr;
			}
		}

		_connections.clear();
	}

	DBConnection* Pop()
	{
		std::lock_guard<std::mutex> lg(_m);

		if (_connections.empty())
			return nullptr;

		DBConnection* connection = _connections.back();
		_connections.pop_back();
		return connection;
	}

	void Push(DBConnection* connection)
	{
	}
private:
	std::mutex _m;
	SQLHENV _hEnv = SQL_NULL_HANDLE;
	std::vector<DBConnection*> _connections;
};
