#pragma once
#include "BufferWriter.h"
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_ENTERGAME = 1000,
	PKT_S_ENTERGAME = 1001,
	PKT_S_LEAVEGAME = 1002,
	PKT_S_SPAWN = 1003,
	PKT_S_DESPAWN = 1004,
	PKT_C_MOVE = 1005,
	PKT_S_MOVE = 1006,
	PKT_C_CHAT = 1007,
	PKT_S_CHAT = 1008,
};


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_C_ENTERGAME(PacketSessionRef& session, Protocol::C_ENTERGAME& pkt);
bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt);
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt);

class ClientPacketHandler
{
public:

	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; ++i) {
			GPacketHandler[0] = Handle_INVALID;
		}
		GPacketHandler[PKT_C_ENTERGAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ENTERGAME>(Handle_C_ENTERGAME, session, buffer, len); };
		GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, session, buffer, len); };
		GPacketHandler[PKT_C_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_CHAT>(Handle_C_CHAT, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_ENTERGAME&pkt) { return _MakeSendBuffer(pkt, PKT_S_ENTERGAME); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LEAVEGAME&pkt) { return _MakeSendBuffer(pkt, PKT_S_LEAVEGAME); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SPAWN&pkt) { return _MakeSendBuffer(pkt, PKT_S_SPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_DESPAWN&pkt) { return _MakeSendBuffer(pkt, PKT_S_DESPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MOVE&pkt) { return _MakeSendBuffer(pkt, PKT_S_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CHAT&pkt) { return _MakeSendBuffer(pkt, PKT_S_CHAT); }

private:

	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef _MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;

		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));

		sendBuffer->Close(packetSize);
		return sendBuffer;
	}
};