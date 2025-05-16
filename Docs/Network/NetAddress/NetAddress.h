#pragma once



class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr)
		: _sockAddr(sockAddr)
	{
	}
	NetAddress(wstring ip, uint16 port)
	{
		::memset(&_sockAddr, 0, sizeof(_sockAddr));
		_sockAddr.sin_family = AF_INET;
		_sockAddr.sin_addr = Ip2Address(ip.c_str());
		_sockAddr.sin_port = ::htons(port);
	}
public:
	SOCKADDR_IN&		GetSockAddr() { return _sockAddr; }
	inline uint16	GetPort() { return ::ntohd(_sockAddr.sin_port); }
	wstring			GetIpAddress()
	{
		WCHAR buffer[100];
		::InetNtopW(AF_INET, &_sockAddr.sin_addr, buffer, len32(buffer));
		return wstring(buffer);
	}
public:
	static IN_ADDR Ip2Address(const WCHAR* ip)
	{
		IN_ADDR address;
		::InetPtonW(AF_INET, ip, &address);
		return address;
	}
private:
	SOCKADDR_IN _sockAddr = {};
};

