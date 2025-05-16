#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"
#include "ServerPacketHandler.h"

char sendBuffer[] = "Hello World";

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		// cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		// cout << "[OnConnected] " << endl;
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{

		PacketSessionRef session = GetPacketSessionRef();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		ServerPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		// cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		// cout << "On Disconnected" << endl;
	}
};

int main()
{

	ServerPacketHandler::Init();

	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IOCPCore>(),
		MakeShared<ServerSession>,
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; ++i) {
		GThreadManager->Launch([=]() {
			while (true) {
				service->GetIocpCore()->Dispatch();
			}
		});
	}
	this_thread::sleep_for(1s);

	{
		Protocol::C_ENTERGAME enterGamePkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
		service->Broadcast(sendBuffer);
	}
	/*
	// 입장 ui 버튼 눌러서 게임 입장

	*/

	Protocol::C_CHAT chatPkt;
	chatPkt.set_msg("Hello World");
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	while (true) {
		service->Broadcast(sendBuffer);
		this_thread::sleep_for(1s);

		{
			Protocol::C_MOVE movePkt;
			movePkt.mutable_posinfo()->set_posx(1);
			movePkt.mutable_posinfo()->set_posy(2);
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
			service->Broadcast(sendBuffer);
		}
		this_thread::sleep_for(1s);
	}
}