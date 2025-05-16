#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"
#include "ServerSession.h"
#include "MapRenderer.h"
#include "ObjectManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return true;
}


// 내정보
bool Handle_S_ENTERGAME(PacketSessionRef& session, Protocol::S_ENTERGAME& pkt)
{
	if (session == nullptr)
		return false;
	ServerSessionRef serverSession = static_pointer_cast<ServerSession>(session);

	// cout << "[MangeClient]: S_ENTER [" << pkt.object().objectid() << "]" << "(" << pkt.object().posinfo().posx() << ", " << pkt.object().posinfo().posy() << ")" << endl;

	OwnerPlayerRef ownerPlayer = MakeShared<OwnerPlayer>();
	ownerPlayer->SetPosInfo(pkt.object().posinfo());
	ownerPlayer->SetObjectId(pkt.object().objectid());
	ownerPlayer->SetSession(static_pointer_cast<ServerSession>(session));
	GObjectManager->Add(static_pointer_cast<GameObject>(ownerPlayer), true);

	return true;
}

// 다른 오브젝트 정보
bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	for (auto& p : pkt.objects()) {

		Protocol::GameObjectType type = ObjectManager::GetObjectTypeById(p.objectid());
		if (type == Protocol::GameObjectType::PLAYER) {
			PlayerRef player = MakeShared<Player>();
			player->SetPosInfo(p.posinfo());
			player->SetObjectId(p.objectid());
			GObjectManager->Add(static_pointer_cast<GameObject>(player), true);
		}
		else if (type == Protocol::GameObjectType::MONSTER) {
			MonsterRef monster = MakeShared<Monster>();
			monster->SetPosInfo(p.posinfo());
			monster->SetObjectId(p.objectid());
			GObjectManager->Add(static_pointer_cast<GameObject>(monster), true);
		}

		int32 x = p.posinfo().posx();
		int32 y = p.posinfo().posy();
		// cout << "[ManagerClient]: [S_SPAWN] [" << p.objectid() << "]" << "(" << x << ", " << y << ")" << endl;
		// MapRenderer::DrawMonster(x, y);
	}
	return true;
}

bool Handle_S_LEAVEGAME(PacketSessionRef& session, Protocol::S_LEAVEGAME& pkt)
{
	return false;
}


bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	return false;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	// cout << "[Client]: S_MOVE [" << pkt.objectid() << "]" << "(" << pkt.posinfo().posy() << ", " << pkt.posinfo().posx() << ")" << endl;
	ServerSessionRef gameSession = static_pointer_cast<ServerSession>(session);
	Protocol::GameObjectType type = ObjectManager::GetObjectTypeById(pkt.objectid());
	if (type == Protocol::GameObjectType::PLAYER) {
		PlayerRef player = GObjectManager->FindPlayer(pkt.objectid());
		Protocol::PosInfo pos;
		pos.set_posx(pkt.posinfo().posx());
		pos.set_posy(pkt.posinfo().posy());
		player->SetPosInfo(pos);
	}
	else if (type == Protocol::GameObjectType::MONSTER) {
		MonsterRef monster = GObjectManager->FindMonster(pkt.objectid());
		Protocol::PosInfo pos;
		pos.set_posx(pkt.posinfo().posx());
		pos.set_posy(pkt.posinfo().posy());
		monster->SetPosInfo(pos);
	}


	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	return true;
}