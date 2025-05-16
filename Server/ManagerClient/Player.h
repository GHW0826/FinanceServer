#pragma once
#include "GameObject.h"

using PlayerRef = std::shared_ptr<class Player>;

class Player : public GameObject
{
public:
	Player() 
	{
		_symbol = PLAYER_SYMBOL;
	}
public:
	void Update() override
	{

	}
	void PreRender() override
	{
		// 이전 위치 지우기
		// GotoXY(_prevX, _prevY);
		// std::cout << ' ';
	}
	void Render() override
	{
		// GotoXY(_posInfo.posx(), _posInfo.posy());
		// SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		// SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		// 
		//COORD pos = { (SHORT)_posInfo.posx(), (SHORT)_posInfo.posy() };
		//SetConsoleCursorPosition(hConsole[active], pos);
		//SetConsoleTextAttribute(hConsole[active], FOREGROUND_RED | FOREGROUND_INTENSITY);

		//DWORD written;
		//WriteConsoleOutputCharacterA(hConsole[active], &_symbol, 1, pos, &written);
	}
};

