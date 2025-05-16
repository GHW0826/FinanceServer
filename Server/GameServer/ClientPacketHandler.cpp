#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Player.h"
#include "GameRoom.h"
#include "GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 구현
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_C_ENTERGAME(PacketSessionRef& session, Protocol::C_ENTERGAME& pkt)
{
	cout << "[Server]: " << "C_ENTERGAME" << endl;

	/*
	PlayerRef playerRef = MakeShared<Player>();
	playerRef->playerId = gameSession->_players.size();
	playerRef->name = "Player[" + to_string(playerRef->playerId) + "]";
	playerRef->ownerSession = gameSession;
	*/
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef playerRef = GPlayerManager->Add();
	playerRef->ownerSession = gameSession;
	gameSession->_currentPlayer = playerRef; // READ_ONLY?
	gameSession->_room = GRoom;
	GRoom->DoAsync(&GameRoom::Enter, static_pointer_cast<GameObject>(gameSession->_currentPlayer));

	/*
	// enter한 본인 한테 정보 전송
	{
		Protocol::S_ENTERGAME enterGamePkt;
		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo;
		playerInfo->set_name(playerRef->name);
		playerInfo->set_playerid(playerRef->GetObjectId());

		int32 posx = rand() % 10;
		int32 posy = rand() % 10;
		cout << "C_ENTERGAME : [" << playerRef->GetObjectId() << "]" << "(" << posx << ", " << posy << ")" << endl;
		Protocol::PosInfo posInfo;
		posInfo.set_posx(posx);
		posInfo.set_posy(posy);
		*playerInfo->mutable_posinfo() = posInfo;
		enterGamePkt.set_allocated_player(playerInfo);
		auto enterBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
		session->Send(enterBuffer);

		// 입장한 player 말고 이미 입장한 플레이어들.
		/*
		Protocol::S_SPAWN spawnPkt;
		for (auto& p : GRoom->_players) {

		}
		spawnPkt.add_players();
		auto spawnBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);
		session->Send(spawnBuffer);
	}
	*/

	/*
		{
		GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
		gameSession->_room = GRoom;

		Protocol::S_ENTERGAME enterGamePkt;
		// enterGamePkt.set_allocated_player();
		auto enterBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
		session->Send(enterBuffer);

		Protocol::S_SPAWN spawnPkt;
		for (auto& p : GRoom->_players) {

		}
		spawnPkt.add_players();
		auto spawnBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);
		session->Send(spawnBuffer);
	}

	// 타인에게 정보 전송
	{
		Protocol::S_SPAWN spawnPkt;
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);
		GRoom->DoAsync(&GameRoom::Broadcast, sendBuffer);
	}

	{
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_playerids(1);
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(despawnPkt);
		GRoom->DoAsync(&GameRoom::Broadcast, sendBuffer);
	}
	*/
	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	if (gameSession->_currentPlayer == nullptr)
		return false;

	// TOOD 검증


	cout << "C_MOVE : [" << gameSession->_currentPlayer->GetObjectId() <<"] (" << pkt.posinfo().posy() << ", " << pkt.posinfo().posx() << ")" << endl;

	GRoom->DoAsync(&GameRoom::HandleMove, static_pointer_cast<GameObject>(gameSession->_currentPlayer), pkt);


	/*
	Protocol::S_MOVE movePkt;
	movePkt.set_playerid();
	movePkt.set_posx();
	movePkt.set_posy();
	*/
	return false;
}


bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << "[Server]: " << pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);
	GRoom->DoAsync(&GameRoom::Broadcast, sendBuffer);

	// GRoom.PushJob(MakeShared<BroadcastJob>(GRoom, sendBuffer));
	// GRoom.Broadcast(sendBuffer);

	return true;
}

