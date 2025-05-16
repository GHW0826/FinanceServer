
struct PacketHeader
{
	uint16 size;
	uint16 id;		
};

class PacketSession : public Session
{
public:
	PacketSession()
	{
	}

	virtual ~PacketSession()
	{
	}

	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }
protected:
	virtual int32 OnRecv(BYTE* buffer, int32 len) sealed
	{
		int32 processLen = 0;
		while (true) {
			int32 dataSize = len - processLen;
			// 최소한 헤더는 파싱할 수 있어야 한다.
			if (dataSize < sizeof(PacketHeader))
				break;

			PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
			// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 함.
			if (dataSize < header.size)
				break;

			// 패킷 조립 성공
			OnRecvPacket(&buffer[processLen], header.size);

			processLen += header.size;
		}

		return processLen;
	}
	virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract;
};
