#pragma once

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
#endif

#include <Windows.h>
#include <windowsx.h>

#include "../../../Threading/CEThread.h"

#include "../../../Log/CELog.h"

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
