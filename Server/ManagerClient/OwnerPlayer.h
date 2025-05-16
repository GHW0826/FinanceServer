#pragma once
#include "Player.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"

using OwnerPlayerRef = std::shared_ptr<class OwnerPlayer>;

class OwnerPlayer : public Player
{
public:
	OwnerPlayer()
	{
		_symbol = PLAYER_SYMBOL;
	}
public:

	void process_input() {

		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			//std::cout << "Arrow Up pressed" << std::endl;
			{
				Protocol::C_MOVE movePkt;
			//	cout << "process_input: " << "[" << _posInfo.posy() - 1 << "][" << _posInfo.posx() << "]" << endl;
				movePkt.mutable_posinfo()->set_posx(_posInfo.posx());
				movePkt.mutable_posinfo()->set_posy(_posInfo.posy() - 1);
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
				_session->Send(sendBuffer);
			}
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			//std::cout << "Arrow Down pressed" << std::endl;
			{
				Protocol::C_MOVE movePkt;
			//	cout << "process_input: " << "[" << _posInfo.posy() + 1 << "][" << _posInfo.posx() << "]" << endl;
				movePkt.mutable_posinfo()->set_posx(_posInfo.posx());
				movePkt.mutable_posinfo()->set_posy(_posInfo.posy() + 1);
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
				_session->Send(sendBuffer);
			}
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			//std::cout << "Arrow Left pressed" << std::endl;
			{
				Protocol::C_MOVE movePkt;
		//		cout << "process_input: " << "[" << _posInfo.posy() << "][" << _posInfo.posx() - 1 << "]" << endl;
				movePkt.mutable_posinfo()->set_posx(_posInfo.posx() - 1);
				movePkt.mutable_posinfo()->set_posy(_posInfo.posy());
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
				_session->Send(sendBuffer);
			}
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			//std::cout << "Arrow Right pressed" << std::endl;
			{
				Protocol::C_MOVE movePkt;
			//	cout << "process_input: " << "[" << _posInfo.posy() << "][" << _posInfo.posx() + 1 << "]" << endl;
				movePkt.mutable_posinfo()->set_posx(_posInfo.posx() + 1);
				movePkt.mutable_posinfo()->set_posy(_posInfo.posy());
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
				_session->Send(sendBuffer);
			}
		}
		/*
		if (GetAsyncKeyState('W') & 0x8000) {
			std::cout << "W key pressed" << std::endl;
		}
		if (GetAsyncKeyState('A') & 0x8000) {
			std::cout << "A key pressed" << std::endl;
		}
		if (GetAsyncKeyState('S') & 0x8000) {
			std::cout << "S key pressed" << std::endl;
		}
		if (GetAsyncKeyState('D') & 0x8000) {
			std::cout << "D key pressed" << std::endl;
		}
		*/
	}

	void Update() override
	{
		process_input();
	}

	void PreRender() override
	{
		// 이전 위치 지우기
		GotoXY(_prevX, _prevY);
		std::cout << ' ';
	}

	void Render() override
	{
		/*
		COORD pos = { (SHORT)_posInfo.posx(), (SHORT)_posInfo.posy() };
		SetConsoleCursorPosition(hConsole[active], pos);
		SetConsoleTextAttribute(hConsole[active], FOREGROUND_RED | FOREGROUND_INTENSITY);

		DWORD written;
		WriteConsoleOutputCharacterA(hConsole[active], &_symbol, 1, pos, &written);
		*/
		GotoXY(_posInfo.posx(), _posInfo.posy());
		SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << _symbol;
		SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
public:
	void SetSession(ServerSessionRef session) {
		_session = session;
	}
private:
	ServerSessionRef _session;
};

