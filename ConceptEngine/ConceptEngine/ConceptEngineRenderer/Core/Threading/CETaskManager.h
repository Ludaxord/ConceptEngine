#pragma once
#include "ThreadSafeInt.h"

#include "Platform/Mutex.h"
#include "Platform/ConditionVariable.h"

#include "Generic/GenericThread.h"

#include "../../Core/Delegates/Delegate.h"

typedef int64 TaskID;

struct Task
{
    TDelegate<void()> Delegate;
};

class CETaskManager
{
public:
    bool Create();

    TaskID AddTask(const Task& NewTask);

    void WaitForTask(TaskID Task);
    void WaitForAllTasks();

    void Release();

    static CETaskManager& Get();

private:
    CETaskManager();
    ~CETaskManager();

    bool PopTask(Task& OutTask);

    void KillWorkers();

    static void WorkThread();

private:
    TArray<TRef<GenericThread>> WorkThreads;

    TArray<Task> Tasks;
    Mutex TaskMutex;

    Mutex WakeMutex;
    ConditionVariable WakeCondition;

    ThreadSafeInt32 TaskAdded;
    ThreadSafeInt32 TaskCompleted;

    volatile bool IsRunning;

    static CETaskManager Instance;
};