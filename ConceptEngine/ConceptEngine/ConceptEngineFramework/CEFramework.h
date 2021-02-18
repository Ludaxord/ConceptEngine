#pragma once
#include <memory>
#include <spdlog/logger.h>

namespace ConceptEngineFramework {
	class CEFramework {
	public:

		CEFramework(std::wstring name, HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow);
		int Run();

		HINSTANCE GetHInstance();
	protected:
	private:
		std::wstring m_name;
		HINSTANCE m_hInstance;
		LPSTR m_lpCmdLine;
		int m_nCmdShow;
	};
}
