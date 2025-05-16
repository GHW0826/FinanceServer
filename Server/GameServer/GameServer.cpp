#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"
#include "iniparser.h"
#include "StringHelper.h"
#include "TickRunner.h"

enum {
	WORKER_TICK = 64,
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true) {
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리 (Timer)
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐 로직
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	// Loading from file
	INI::File ft;
	std::ifstream file(L"./settings.ini", std::ios::in);
	if (ft.Load("./settings.ini") == false) {
		// Loading from stream
		cout << "Faile Load Config File" << endl;
		return false;
	}
	StdString loadCs = ft.GetSection("DB")->GetValue("ConnectionString").AsString().c_str();
	String cs = StringHelper::ToWString(loadCs);

	// ASSRT_CRASH(GDBConnectionPool->Connect(1, L"Server=127.0.0.1;Port=3306;Database=investar;Uid =root;Pwd=a4s5d6A4s5!;"));
	// ASSERT_CRASH(GDBConnectionPool->Connect(1, cs.c_str()));
	/*
	GRoom->DoTimer(1000, [] { cout << "Hello 1000" << endl; });
	GRoom->DoTimer(3000, [] { cout << "Hello 3000" << endl; });
	GRoom->DoTimer(2000, [] { cout << "Hello 2000" << endl; });
	*/

	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IOCPCore>(),
		MakeShared<GameSession>,
		100);

	ASSERT_CRASH(service->Start());

	GRoom->DoAsync(&GameRoom::GenMonster);

	// GGameMapManager->Add();

	for (int32 i = 0; i < 5; ++i) {
		GThreadManager->Launch([&service]() {
			DoWorkerJob(service);
		});
	}

	for (int32 i = 0; i < 1; ++i) {
		GThreadManager->Launch([]() {
			TickRunner<GameRoom> tickRunner(GRoom, 1000);
		});
	}

	DoWorkerJob(service);
	// Monster* monster = ObjectPool<Monster>().Pop();
	
	GThreadManager->Join();

	return 0;
}
