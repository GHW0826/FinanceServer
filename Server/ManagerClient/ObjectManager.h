#pragma once
#include "Monster.h"
#include "OwnerPlayer.h"

class ObjectManager
{
public:
	static Protocol::GameObjectType GetObjectTypeById(int32 id)
	{
		int32 type = (id >> 24) & 0x7F;
		return static_cast<Protocol::GameObjectType>(type);
	}
public:
	void Update()
	{
		for (auto& player : _players) {
			player.second->Update();
		}
		for (auto& monster : _monsters) {
			monster.second->Update();
		}
	}

	void PreRender()
	{
		for (auto& player : _players) {
			player.second->PreRender();
		}
		for (auto& monster : _monsters) {
			monster.second->PreRender();
		}
	}

	void Render()
	{
		for (auto& player : _players) {
			player.second->Render();
		}
		for (auto& monster : _monsters) {
			monster.second->Render();
		}
	}
public:
	void Add(GameObjectRef object, bool ownerPlayer = false)
	{
		WRITE_LOCK;
		Protocol::GameObjectType objectType = ObjectManager::GetObjectTypeById(object->GetObjectId());
		if (objectType == Protocol::GameObjectType::PLAYER) {
			_players.insert({ object->GetObjectId(), static_pointer_cast<Player>(object) });

			if (ownerPlayer) {
				OwnerPlayerRef ownerPlayer = static_pointer_cast<OwnerPlayer>(object);
				//ownerPlayer->sess
			}
		}
		else if (objectType == Protocol::GameObjectType::MONSTER) {
			_monsters.insert({ object->GetObjectId(), static_pointer_cast<Monster>(object) });
		}
	}

	PlayerRef FindPlayer(int32 objectId) {
		READ_LOCK;
		return _players[objectId];
	}
	MonsterRef FindMonster(int32 objectId) {
		READ_LOCK;
		return _monsters[objectId];
	}
public:
	std::unordered_map<uint64, PlayerRef> _players;
	std::unordered_map<uint64, MonsterRef> _monsters;
	// { get; set; }
private:
	USE_LOCK;
};

