#pragma once
#include "../Common/CETypes.h"
#include "../Delegates/CEDelegate.h"

namespace ConceptEngine::Core::Threading {

	typedef int64 TaskID;

	struct CETask {
		Delegates::CEDelegate<void()> Delegate;
	};

	class CETaskManager {
	public:
		bool Create();
		TaskID AddTask(const CETask NewTask);

		void WaitForTask(TaskID);
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
		volatile bool IsRunning;

		static CETaskManager Instance;
	};
}
