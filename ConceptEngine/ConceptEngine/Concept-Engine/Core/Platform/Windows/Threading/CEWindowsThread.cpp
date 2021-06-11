#include "CEWindowsThread.h"

#include <condition_variable>

#include "../../../Common/CERef.h"

#include "../../../Log/CELog.h"

#include "../../../../Utilities/CEStringUtilities.h"

using namespace ConceptEngine::Core::Platform::Windows::Threading;

ConceptEngine::Core::Threading::CEThread* ConceptEngine::Core::Threading::CEThread::Create(ThreadFunction func) {
	std::condition_variable variable;

	CERef<CEWindowsThread> newThread = new CEWindowsThread();
	if (!newThread->Create(func)) {
		return nullptr;
	}

	return newThread.ReleaseOwnership();
}

CEWindowsThread::CEWindowsThread() : CEThread(), Thread(0), ThreadID(0) {
}

CEWindowsThread::~CEWindowsThread() {
	if (Thread) {
		CloseHandle(Thread);
	}
}

bool CEWindowsThread::Create(Core::Threading::ThreadFunction func) {
	Func = func;

	Thread = CreateThread(NULL, 0, CEWindowsThread::ThreadRoutine, (LPVOID)this, 0, &ThreadID);
	if (!Thread) {
		CE_LOG_ERROR("[CEWindowsThread]: Failed to create thread");
		return false;
	}

	return true;
}

void CEWindowsThread::Wait() {
	WaitForSingleObject(Thread, INFINITE);
}

void CEWindowsThread::SetName(const std::string& name) {
	std::wstring wName = ConvertToWString(name);
	SetThreadDescription(Thread, wName.c_str());
}

ConceptEngine::Core::Threading::ThreadID CEWindowsThread::GetID() {
	return ThreadID;
}

DWORD CEWindowsThread::ThreadRoutine(LPVOID threadParameter) {

	volatile CEWindowsThread* currentThread = (CEWindowsThread*)threadParameter;
	if (currentThread) {
		Assert(currentThread->Func != nullptr);

		currentThread->Func();

		return 0;
	}

	return -1;
}
