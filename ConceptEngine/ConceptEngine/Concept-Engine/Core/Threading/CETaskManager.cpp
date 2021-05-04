#include "CETaskManager.h"

using namespace ConceptEngine::Core::Threading;

bool CETaskManager::Create() {
	return true;
}

TaskID CETaskManager::AddTask(const CETask NewTask) {
	return 0;
}

void CETaskManager::WaitForTask(TaskID) {
}

void CETaskManager::WaitForAllTasks() {
}

void CETaskManager::Release() {
}

CETaskManager& CETaskManager::Get() {
	return Instance;
}

CETaskManager::CETaskManager() {
}

CETaskManager::~CETaskManager() {
}

bool CETaskManager::PopTask(CETask& Task) {
	return true;
}

void CETaskManager::KillWorkers() {
}

void CETaskManager::WorkThread() {
}
