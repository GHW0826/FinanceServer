#pragma once
#include "NetAddress.h"

template<typename T>
static inline bool SetSocketOpt(SOCKET socket, int32 level, int32 optName, T optValue)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optValue), sizeof(T));
}

class SocketUtils
{
public:
	static LPFN_CONNECTEX ConnectEx = nullptr;
	static LPFN_DISCONNECTEX DisconnectEx = nullptr;
	static LPFN_ACCEPTEX AcceptEx = nullptr;
public:
	static void Init()
	{
		WSADATA wsaData;
		ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);

		// 런타임에 주소 얻어오는 API
		SOCKET dummySocket = CreateSocket();
		ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
		ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
		ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
		Close(dummySocket);
	}

	static void Clear()
	{
		::WSACleanup();
	}

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
	{
		DWORD bytes = 0;
		return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
	}

	static SOCKET CreateSocket()
	{
		return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	}

	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
	{
		LINGER option;
		option.l_onoff = onoff;
		option.l_linger = linger;
		return SetSocketOpt(socket, SOL_SOCKET, SO_LINGER, option);
	}

	static bool SetReuseAddress(SOCKET socket, bool flag)
	{
		return SetSocketOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
	}

	static bool SetRecvBufferSize(SOCKET socket, int32 size)
	{
		return SetSocketOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
	}

	static bool SetSendBufferSize(SOCKET socket, int32 size)
	{
		return SetSocketOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
	}

	static bool SetNoDelay(SOCKET socket, bool flag)
	{
		return SetSocketOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
	}

	// acceptSocket의 특성을 ClientSocket에 그대로 적용
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET acceptSocket)
	{
		return SetSocketOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, acceptSocket);
	}

	static bool Bind(SOCKET socket, NetAddress netAddress)
	{
		return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddress.GetSockAddr()), sizeof(SOCKADDR_IN));
	}

	static bool BindAnyAddress(SOCKET socket, uint16 port)
	{
		SOCKADDR_IN address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = ::htonl(INADDR_ANY);
		address.sin_port = ::htons(port);

		return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&address), sizeof(address));
	}

	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN)
	{
		return SOCKET_ERROR != ::listen(socket, backlog);
	}

	static void Close(SOCKET& socket)
	{
		if (socket != INVALID_SOCKET)
			::closesocket(socket);
		socket = INVALID_SOCKET;
	}
};


