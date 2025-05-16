#pragma once
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <Windows.h>

using namespace std;

enum {
	WVARCHAR_MAX = 4000,
	BINARY_MAX = 8000,
};

class DBConnection
{
public:
	bool Connect(SQLHDBC henv, const WCHAR* connectionString)
	{
		if (::SQLAllocHandle(SQL_HANDLE_DBC, henv, &_connection) != SQL_SUCCESS)
			return false;

		WCHAR stringBuffer[MAX_PATH] = { 0, };
		::wcscpy_s(stringBuffer, connectionString);

		WCHAR resultString[MAX_PATH] = { 0, };
		SQLSMALLINT resultStringLen = 0;

		SQLRETURN ret = ::SQLDriverConnectW(
			_connection,
			NULL,
			reinterpret_cast<SQLWCHAR*>(stringBuffer),
			_countof(stringBuffer),
			OUT reinterpret_cast<SQLWCHAR*>(resultString),
			_countof(resultString),
			OUT & resultStringLen,
			SQL_DRIVER_NOPROMPT
		);

		if (::SQLAllocHandle(SQL_HANDLE_STMT, _connection, &_statement) != SQL_SUCCESS)
			return false;

		return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
	}

	void Clear()
	{
		if (_connection != SQL_NULL_HANDLE) {
			::SQLFreeHandle(SQL_HANDLE_DBC, _connection);
			_connection = SQL_NULL_HANDLE;
		}
		if (_statement != SQL_NULL_HANDLE) {
			::SQLFreeHandle(SQL_HANDLE_STMT, _statement);
			_statement = SQL_NULL_HANDLE;
		}
	}

	bool Execute(const WCHAR* query)
	{
		SQLRETURN ret = ::SQLExecDirectW(_statement, (SQLWCHAR*)query, SQL_NTSL);
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			return true;

		HandleError(ret);

		return false;
	}

	bool Fetch()
	{
		SQLRETURN ret = ::SQLFetch(_statement);
		switch (ret) {
		case SQL_SUCCESS:
		case SQL_SUCCESS_WITH_INFO:
			return true;
		case SQL_NO_DATA:
			return false;
		case SQL_ERROR:
			HandleError(ret);
			break;
		default:
			return true;
		}

		return true;
	}

	int GetRowCount()
	{
		SQLLEN count = 0;
		SQLRETURN ret = ::SQLRowCount(_statement, &count);
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			return static_cast<int>(count);

		return -1;
	}


	void UnBind()
	{
		::SQLFreeStmt(_statement, SQL_UNBIND);
		::SQLFreeStmt(_statement, SQL_RESET_PARAMS);
		::SQLFreeStmt(_statement, SQL_CLOSE);
	}

	void HandleError(SQLRETURN ret)
	{
		if (ret == SQL_SUCCESS)
			return;

		SQLSMALLINT index = 1;
		SQLWCHAR sqlState[MAX_PATH] = { 0, };
		SQLINTEGER nativeErr = 0;
		SQLWCHAR errMsg[MAX_PATH] = { 0, };
		SQLSMALLINT msgLen = 0;
		SQLRETURN errRet = 0;

		while (true) {
			errRet = ::SQLGetDiagRecW(
				SQL_HANDLE_STMT,
				_statement,
				index,
				sqlState,
				OUT &nativeErr,
				errMsg,
				_countof(errMsg),
				OUT & msgLen);

			if (errRet == SQL_NO_DATA)
				break;

			if (errRet != SQL_SUCCESS && errRet != SQL_SUCCESS_WITH_INFO)
				break;

			// TODO Log
			wcout.imbue(locale("kor"));
			wcout << errMsg << endl;
			++index;
		}
	}
private:
	SQLHDBC _connection = SQL_NULL_HANDLE;
	SQLHSTMT _statement = SQL_NULL_HANDLE;
};
