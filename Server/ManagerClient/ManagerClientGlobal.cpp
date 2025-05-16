#include "pch.h"
#include "ManagerClientGlobal.h"
#include "ObjectManager.h"

class ObjectManager* GObjectManager = nullptr;

class MangerClientGlobal
{
public:
	MangerClientGlobal();
	~MangerClientGlobal();
} GMangerClientGloball;

MangerClientGlobal::MangerClientGlobal()
{
	RAW_POINTER_NEW(GObjectManager, ObjectManager);
}

MangerClientGlobal::~MangerClientGlobal()
{
	RAW_POINTER_DELETE(GObjectManager);
}
