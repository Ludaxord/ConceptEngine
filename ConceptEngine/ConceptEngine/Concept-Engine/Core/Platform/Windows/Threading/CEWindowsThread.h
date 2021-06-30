#pragma once

#include <Windows.h>

#include "../../../Threading/CEThread.h"

namespace ConceptEngine::Core::Platform::Windows::Threading {
	class CEWindowsThread : public Core::Threading::CEThread {
	public:
		CEWindowsThread();
		~CEWindowsThread();

		bool Init(Core::Threading::ThreadFunction func);

		virtual void Wait() override final;
		virtual void SetName(const std::string& name) override final;
		virtual Core::Threading::ThreadID GetID() override final;
	private:
		static DWORD WINAPI ThreadRoutine(LPVOID threadParameter);

		HANDLE Thread;
		DWORD ThreadID;

		//TODO: add boost::function variable;
		Core::Threading::ThreadFunction Func;
	};
}
