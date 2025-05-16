#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"

ServerSession::~ServerSession()
{
	// cout << "~ServerSession" << endl;
}

void ServerSession::OnConnected()
{
	// cout << "[OnConnected] " << endl;
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void ServerSession::OnSend(int32 len)
{
}
void ServerSession::OnDisconnected()
{
}
