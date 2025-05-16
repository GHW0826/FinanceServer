#pragma once

extern HANDLE hConsole;
extern int32 active;

class Renderable
{
public:
	static void Init()
	{
		CONSOLE_CURSOR_INFO cursorInfo{ 1, FALSE };
		GetConsoleCursorInfo(hConsole, &cursorInfo);
		cursorInfo.bVisible = FALSE;
		SetConsoleCursorInfo(hConsole, &cursorInfo);

		////////////////////////////////

		SetConsoleSize(100, 100);
	}
	static void ClearBuffer() {
		system("cls");
	}

	static void flipBuffer() 
	{
	}


	static void Render()
	{
	}

	static void SetConsoleSize(int width, int height)
	{
		// 콘솔 버퍼 크기 설정
		COORD bufferSize = { (SHORT)width, (SHORT)height };
		SetConsoleScreenBufferSize(hConsole, bufferSize);

		// 콘솔 창 크기 설정
		SMALL_RECT windowSize = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
		SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
	}
protected:
	void GotoXY(int32 x, int32 y)
	{
		COORD pos = { (SHORT)x, (SHORT)y };
		SetConsoleCursorPosition(hConsole, pos);
	}

	void SetColor(WORD attr)
	{
		SetConsoleTextAttribute(hConsole, attr);
	}
};

