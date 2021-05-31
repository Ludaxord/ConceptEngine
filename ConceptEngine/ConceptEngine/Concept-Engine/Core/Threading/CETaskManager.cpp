#include "CETaskManager.h"

#include "CEProcess.h"
#include "CEScopedLock.h"

#include "../Log/CELog.h"


using namespace ConceptEngine::Core::Threading;

CETaskManager CETaskManager::Instance;

bool CETaskManager::Create() {
	uint32 threadCount = Math::CEMath::Max<int32>(CEProcess::GetNumProcessors() - 1, 1);
	Threads.Resize(threadCount);

	CE_LOG_INFO("[CETaskManager]: Starting '" + std::to_string(threadCount) + "' Workers");

	IsRunning = true;

	for (uint32 i = 0; i < threadCount; i++) {
		if (Common::CERef<CEThread> newThread = CEThread::Create(CETaskManager::WorkThread); newThread) {
			Threads[i] = newThread;
			newThread->SetName("CEThread_" + std::to_string(i));
		}
		else {
			KillWorkers();
			return false;
		}
	}

	return true;
}

TaskID CETaskManager::AddTask(const CETask NewTask) {
	{
		CEScopedLock<CEMutex> lock(TaskMutex);
		Tasks.EmplaceBack(NewTask);
	}

	ThreadID newTaskID = TaskAdded.Increment();

	WakeCondition.NotifyOne();

	return newTaskID;
}

void CETaskManager::WaitForTask(TaskID task) {
	while (TaskCompleted.Load() < task) {
		CEProcess::Sleep(0);
	}
}

void CETaskManager::WaitForAllTasks() {
	while (TaskCompleted.Load() < TaskAdded.Load()) {
		CEProcess::Sleep(0);
	}
}

void CETaskManager::Release() {
	KillWorkers();

	for (Common::CERef<CEThread> thread : Threads) {
		thread->Wait();
	}

	Threads.Clear();
}

CETaskManager& CETaskManager::Get() {
	return Instance;
}

CETaskManager::CETaskManager(): TaskMutex(), IsRunning(false) {
}

CETaskManager::~CETaskManager() {
}

bool CETaskManager::PopTask(CETask& Task) {
	CEScopedLock<CEMutex> Lock(TaskMutex);

	if (!Tasks.IsEmpty()) {
		Task = Tasks.Front();
		Tasks.Erase(Tasks.Begin());

		return true;
	}
	return false;
}

void CETaskManager::KillWorkers() {
	IsRunning = false;
	WakeCondition.NotifyAll();
}

void CETaskManager::WorkThread() {
	CE_LOG_INFO("[CETaskManager]: Starting Work Thread: " + std::to_string(CEProcess::GetThreadID()));

	while (Instance.IsRunning) {
		CETask currentTask;

		if (!Instance.PopTask(currentTask)) {
			CEScopedLock<CEMutex> Lock(Instance.WakeMutex);
			Instance.WakeCondition.Wait(Lock);
		}
		else {
			currentTask.Delegate();
			Instance.TaskCompleted++;
		}
	}

	CE_LOG_INFO("[CETaskManager]: End Work Thread: " + std::to_string(CEProcess::GetThreadID()));
}
