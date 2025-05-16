#include "pch.h"
#include "ServerGlobal.h"
#include "ServerConfig.h"
#include "DBConfig.h"
#include "PlayerManager.h"
#include "GameMapManager.h"

PlayerManager* GPlayerManager = nullptr;
MonsterManager* GMonsterManager = nullptr;
GameMapManager* GGameMapManager = nullptr;

class ServerGlobal
{
public:
	ServerGlobal();
	~ServerGlobal();
} GServerGlobal;

ServerGlobal::ServerGlobal()
{
	RAW_POINTER_NEW(GPlayerManager, PlayerManager);
	RAW_POINTER_NEW(GMonsterManager, MonsterManager);
	RAW_POINTER_NEW(GGameMapManager, GameMapManager);
}


ServerGlobal::~ServerGlobal()
{
	RAW_POINTER_DELETE(GPlayerManager);
	RAW_POINTER_DELETE(GMonsterManager);
	RAW_POINTER_DELETE(GGameMapManager);
}
