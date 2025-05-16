#pragma once
#include "Renderable.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// 현재 플레이어 좌표

class GameMap : public Renderable
{
public:
	void LoadMap(int32 mapId, string pathPrefix = "../Common/MapData")
	{
		std::ostringstream oss;
		oss << "Map_" << std::setw(3) << std::setfill('0') << mapId;
		std::string mapName = oss.str();

		// Collision 관련 파일
		std::string filePath = pathPrefix + "/" + mapName + ".txt";
		std::ifstream file(filePath);
		if (!file.is_open()) {
			std::cerr << "파일 열기 실패: " << filePath << std::endl;
			return;
		}

		std::string line;
		// 첫 줄: _minx
		if (std::getline(file, line))
			_minx = std::stoi(line);
		// 둘째 줄: _maxx
		if (std::getline(file, line))
			_maxx = std::stoi(line);
		// 셋째 줄: _miny
		if (std::getline(file, line))
			_miny = std::stoi(line);
		// 넷째 줄: _maxy
		if (std::getline(file, line))
			_maxy = std::stoi(line);

		std::cout << "minx: " << _minx << ", maxx: " << _maxx << ", miny: " << _miny << ", maxy: " << _maxy << std::endl;

		int xCount = _maxx - _minx + 1;
		int yCount = _maxy - _miny + 1;

		_collision.resize(yCount, vector<bool>(xCount, false));
		for (int y = 0; y < yCount; ++y) {
			if (!std::getline(file, line)) {
				std::cerr << "줄을 읽을 수 없습니다 (y=" << y << ")\n";
				return;
			}

			for (int x = 0; x < xCount && x < line.size(); ++x) {
				_collision[y][x] = (line[x] == '1' ? true : false);
				if (line[x] == '1') {
					cout << "collision - [" << y << "][" << x << "]" << endl;
				}
			}
		}
	}

	void Render()
	{
		for (int x = 0; x < _collision[0].size(); ++x) {
			for (int y = 0; y < _collision.size(); ++y) {
				GotoXY(y, x);  // 커서 위치 지정

				if (_collision[y][x])
				{
					SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY); // 충돌: 빨강
					std::cout << '#';
				}
				else
				{
					SetColor(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); // 통과: 밝은 청록
					std::cout << '.';
				}
			}
		}
	}
private:
	vector<vector<bool>> _collision;

	int _minx;
	int _maxx;
	int _miny;
	int _maxy;
};

/*
class MapRenderer
{
public:
	static void gotoxy(int x, int y);
	static void Init();

	static void PreRender();
	static void Render();


	// 플레이어 출력
	static void DrawPlayer(int32 x, int32 y);

	static void DrawMonster(int32 x, int32 y);
};

*/