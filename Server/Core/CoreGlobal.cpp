#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
#include "GlobalQueue.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "JobTimer.h"
#include "DBConnectionPool.h"
#include "ConsoleLog.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
GlobalQueue* GGlobalQueue = nullptr;
JobTimer* GJobTimer = nullptr;
Memory* GMemory = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;
ConsoleLog* GConsoleLogger = nullptr;

class CoreGlobal
{
public:
	CoreGlobal();
	~CoreGlobal();
} GCoreGlobal;

CoreGlobal::CoreGlobal()
{
	RAW_POINTER_NEW(GMemory, Memory);
	RAW_POINTER_NEW(GThreadManager, ThreadManager);
	RAW_POINTER_NEW(GDeadLockProfiler, DeadLockProfiler);
	RAW_POINTER_NEW(GSendBufferManager, SendBufferManager);
	RAW_POINTER_NEW(GGlobalQueue, GlobalQueue);
	RAW_POINTER_NEW(GJobTimer, JobTimer);
	RAW_POINTER_NEW(GDBConnectionPool, DBConnectionPool);
	RAW_POINTER_NEW(GConsoleLogger, ConsoleLog);

	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	RAW_POINTER_DELETE(GThreadManager);
	RAW_POINTER_DELETE(GDeadLockProfiler);
	RAW_POINTER_DELETE(GSendBufferManager);
	RAW_POINTER_DELETE(GGlobalQueue);
	RAW_POINTER_DELETE(GJobTimer);
	RAW_POINTER_DELETE(GMemory);
	RAW_POINTER_DELETE(GDBConnectionPool);
	RAW_POINTER_DELETE(GConsoleLogger);

	SocketUtils::Clear();
}
