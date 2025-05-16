#pragma once
#include "Renderable.h"
#include "Protocol.pb.h"
#include "Enum.pb.h"
#include "Struct.pb.h"

using GameObjectRef = std::shared_ptr<class GameObject>;

const char PLAYER_SYMBOL = '@';
const char MONSTER_SYMBOL = 'M';

class GameObject : public Renderable
{
public:
	Protocol::MoveDir GetMoveDir() { return _posInfo.movedir(); }
	Protocol::PosInfo GetPosInfo() { return _posInfo; }
	Protocol::GameObjectType GetGameObjectType() { return _objectType; }
	inline int32 GetObjectId() { return _objectId; }

	void SetPosInfo(Protocol::PosInfo posInfo)
	{
		if (IsSamePos(posInfo, _posInfo)) {
			cout << "same pos: " << "[" << _posInfo.posy() << "][" << _posInfo.posx() << "]" << endl;
			return;
		}

		_prevX = _posInfo.posx();
		_prevY = _posInfo.posy();
		_posInfo = posInfo;
		// update character TODO
	}
public:
	bool IsSamePos(const Protocol::PosInfo a, const Protocol::PosInfo b) {
		return a.posx() == b.posx() && a.posy() == b.posy() && a.state() == b.state() && a.movedir() == b.movedir();
	}
public:
	void SetObjectId(int32 id) { _objectId = id; }
public:
	virtual void Update() {}
	virtual void PreRender() {}
	virtual void Render() {}
protected:
	char _symbol;
	Protocol::PosInfo _posInfo;
	int32 _prevX;
	int32 _prevY;
private:
	int32 _objectId;
	Protocol::GameObjectType _objectType;
};
