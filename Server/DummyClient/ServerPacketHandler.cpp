#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

/*
#pragma pack(1)
class PKT_S_TEST
{
public:
	class BuffsListItem
	{
	public:
		uint64 buffId;
		float remainTime;
	};


	uint16 packetSize;
	uint16 packetId;
	uint64 id;
	uint32 hp;
	uint16 attack;
	uint32 buffsOffset;
	uint32 buffsCount;
public:
	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		if (packetSize < size)
			return false;

		size += buffsCount * sizeof(BuffsListItem);
		if (size != packetSize)
			return false;

		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;
		return true;
	}

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	BuffsList GetBuffsList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
	}
};
#pragma pack()
*/

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return true;
}

bool Handle_S_TEST(BYTE* buffer, int32 len)
{
	return true;
}

bool Handle_S_ENTERGAME(PacketSessionRef& session, Protocol::S_ENTERGAME& pkt)
{
	return false;
}

bool Handle_S_LEAVEGAME(PacketSessionRef& session, Protocol::S_LEAVEGAME& pkt)
{
	return false;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	return false;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	return false;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	std::cout << "[Client]: " << pkt.posinfo().posx() << ", " << pkt.posinfo().posy() << endl;


	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTERGAME& pkt)
{
	// TODO
	

	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	std::cout << "[Client]: " << pkt.msg() << endl;

	return true;
}

void HandlePacket(BYTE* buffer, int32 len)
{

	/*
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	switch (header.id) {
	case PKT_S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}
	*/
}
