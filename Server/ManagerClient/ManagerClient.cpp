#include "pch.h"
#include "Service.h"
#include "ThreadManager.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"
#include "MapRenderer.h"
#include "ObjectManager.h"


/*

void gotoxy(int x, int y) 
{
	COORD pos = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hConsole, pos);
}

void setColor(WORD attr) 
{
	SetConsoleTextAttribute(hConsole, attr);
}

void ClearScreen() 
{
	system("cls");
}

void drawTerrain(const Vec2& camOrigin) 
{
	for (int y = 0; y < VIEW_HEIGHT; ++y) {
		for (int x = 0; x < VIEW_WIDTH; ++x) {
			int wx = camOrigin.x + x;
			int wy = camOrigin.y + y;
			if (wx >= 0 && wx < WORLD_WIDTH && wy >= 0 && wy < WORLD_HEIGHT) {
				gotoxy(x, y);
				char tile = worldMap[wy][wx];
				if (tile == '#') 
					setColor(FOREGROUND_RED);
				else 
					setColor(FOREGROUND_INTENSITY);
				std::cout << tile;
			}
		}
	}
	setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}


HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// 지형 맵 ('.': 평지, '#': 벽 등)
char worldMap[WORLD_HEIGHT][WORLD_WIDTH];
constexpr int WORLD_WIDTH = 100;
constexpr int WORLD_HEIGHT = 100;

void generateMap() {
	for (int y = 0; y < WORLD_HEIGHT; ++y) {
		for (int x = 0; x < WORLD_WIDTH; ++x) {
			if (y == 0 || y == WORLD_HEIGHT - 1 || x == 0 || x == WORLD_WIDTH - 1 || rand() % 30 == 0)
				worldMap[y][x] = '#';
			else
				worldMap[y][x] = '.';
		}
	}
}
*/

int main()
{

	ServerPacketHandler::Init();

	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IOCPCore>(),
		MakeShared<ServerSession>,
		1);

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
		service->CreateSession();
	}

	GameMap map;
	map.LoadMap(1);
	Renderable::Init();

	while (true) {

		// ESC 누르면 종료
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			std::cout << "Exiting...\n";
			break;
		}

		// Renderable::ClearBuffer();
		map.Render();

		GObjectManager->Update();
		GObjectManager->PreRender();
		GObjectManager->Render();
		Renderable::flipBuffer();
		Sleep(100); // 너무 빠르지 않게 잠깐 쉼
	}
}