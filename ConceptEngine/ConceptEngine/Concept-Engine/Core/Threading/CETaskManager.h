#pragma once
#include "CEConditionVariable.h"
#include "CEThread.h"
#include "../Common/CETypes.h"
#include "../Delegates/CEDelegate.h"
#include "../Containers/CEArray.h"
#include "../Common/CERef.h"
#include "CEMutex.h"
#include "CEThreadSafeInt.h"

namespace ConceptEngine::Core::Threading {

	typedef int64 TaskID;

	struct CETask {
		Delegates::CEDelegate<void()> Delegate;
	};

	class CETaskManager {
	public:
		bool Create();
		TaskID AddTask(const CETask NewTask);

		void WaitForTask(TaskID task);
		void WaitForAllTasks();

		void Release();

		static CETaskManager& Get();
	protected:
	private:
		CETaskManager();
		~CETaskManager();

		bool PopTask(CETask& Task);

		void KillWorkers();

		static void WorkThread();

	private:
		Containers::CEArray<Common::CERef<CEThread>> Threads;

		Containers::CEArray<CETask> Tasks;
		CEMutex TaskMutex;

		CEMutex WakeMutex;
		CEConditionVariable WakeCondition;

		CEThreadSafeInt32 TaskAdded;
		CEThreadSafeInt32 TaskCompleted;
		
		volatile bool IsRunning;

		static CETaskManager Instance;
	};
}
