#include "CETaskManager.h"

#include "Platform/PlatformProcess.h"

#include "ScopedLock.h"

#include <condition_variable>

CETaskManager CETaskManager::Instance;

CETaskManager::CETaskManager()
    : TaskMutex()
    , IsRunning(false)
{
}

CETaskManager::~CETaskManager()
{
    // Empty for now
}

bool CETaskManager::PopTask(Task& OutTask)
{
    TScopedLock<Mutex> Lock(TaskMutex);

    if (!Tasks.IsEmpty())
    {
        OutTask = Tasks.Front();
        Tasks.Erase(Tasks.Begin());

        return true;
    }
    else
    {
        return false;
    }
}

void CETaskManager::KillWorkers()
{
    IsRunning = false;

    WakeCondition.NotifyAll();
}

void CETaskManager::WorkThread()
{
    CE_LOG_INFO("Starting Workthread: " + std::to_string(PlatformProcess::GetThreadID()));

    while (Instance.IsRunning)
    {
        Task CurrentTask;

        if (!Instance.PopTask(CurrentTask))
        {
            TScopedLock<Mutex> Lock(Instance.WakeMutex);
            Instance.WakeCondition.Wait(Lock);
        }
        else
        {
            CurrentTask.Delegate();
            Instance.TaskCompleted++;
        }
    }

    CE_LOG_INFO("End Workthread: " + std::to_string(PlatformProcess::GetThreadID()));
}

bool CETaskManager::Create()
{
    // NOTE: Maybe change to NumProcessors - 1 -> Test performance
    uint32 ThreadCount = Math::Max<int32>(PlatformProcess::GetNumProcessors() - 1, 1);
    WorkThreads.Resize(ThreadCount);

    CE_LOG_INFO("[TaskManager]: Starting '" + std::to_string(ThreadCount) + "' Workers");

    // Start so that workers now that they should be running
    IsRunning = true;
    
    for (uint32 i = 0; i < ThreadCount; i++)
    {
        CERef<GenericThread> NewThread = GenericThread::Create(CETaskManager::WorkThread);
        if (NewThread)
        {
            WorkThreads[i] = NewThread;
            NewThread->SetName("WorkerThread " + std::to_string(i));
        }
        else
        {
            KillWorkers();
            return false;
        }
    }

    return true;
}

TaskID CETaskManager::AddTask(const Task& NewTask)
{
    {
        TScopedLock<Mutex> Lock(TaskMutex);
        Tasks.EmplaceBack(NewTask);
    }

    ThreadID NewTaskID = TaskAdded.Increment();

    WakeCondition.NotifyOne();

    return NewTaskID;
}

void CETaskManager::WaitForTask(TaskID Task)
{
    while (TaskCompleted.Load() < Task)
    {
        // Look into proper yeild
        PlatformProcess::Sleep(0);
    }
}

void CETaskManager::WaitForAllTasks()
{
    while (TaskCompleted.Load() < TaskAdded.Load())
    {
        // Look into proper yeild
        PlatformProcess::Sleep(0);
    }
}

void CETaskManager::Release()
{
    KillWorkers();

    for (CERef<GenericThread> Thread : WorkThreads)
    {
        Thread->Wait();
    }

    WorkThreads.Clear();
}

CETaskManager& CETaskManager::Get()
{
    return Instance;
}
