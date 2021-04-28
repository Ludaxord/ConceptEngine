#pragma once
#include <activation.h>


namespace ConceptEngine::Project {
	class CEProjectBinder {
	public:
		template <typename T, typename... TArgs>
		inline T Call(LPCSTR lpProcName, TArgs ... args) {
			typedef T (*vargs)(TArgs ...);
			return ((vargs)GetProc(lpProcName))(args...);
		}

		explicit CEProjectBinder(const TCHAR* ceModule);
		virtual ~CEProjectBinder();

	protected:
		FARPROC GetProc(LPCSTR lpProcName);
	private:
		HMODULE lnk;
	};
}
