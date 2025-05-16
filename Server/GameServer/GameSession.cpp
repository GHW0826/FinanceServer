#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameRoom.h"
#include "ClientPacketHandler.h"

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	// Echo
	// cout << "OnRecv Len = " << len << endl;

	// ServerPacketHandler::HandlePacket(buffer, len);

	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO: packetID 대역 체크
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{
	// cout << "OnSend Len = " << len << endl;
}

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));

	/*
	_currentPlayer = GPlayerManager->Add();
	{
		_currentPlayer->_playerInfo.set_name("Player" + _currentPlayer->playerId);
		_currentPlayer->_playerInfo.set_posx(0);
		_currentPlayer->_playerInfo.set_posy(0);
		_currentPlayer->ownerSession = shared_from_this();
	}
	*/
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
	/*
	if (_currentPlayer) {
		if (auto room = _room.lock()) {
			room->DoAsync(&GameRoom::Leave, _currentPlayer);
		}
	}
	*/
	_currentPlayer = nullptr;
	_players.clear();
}
