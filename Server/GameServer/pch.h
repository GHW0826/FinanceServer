#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\Core.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "Release\\Core.lib")
#endif

#include "CorePch.h"
#include "GameServerTypes.h"

#include "Enum.pb.h"

#include "ServerMacro.h"
#include "ServerGlobal.h"
#include "GameSessionManager.h"
#include "PlayerManager.h"
#include "GameMapManager.h"

#include "Random.h"
