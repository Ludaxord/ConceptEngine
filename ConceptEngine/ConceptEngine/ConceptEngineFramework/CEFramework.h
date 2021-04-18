#pragma once
#include <memory>
#include <spdlog/logger.h>

#include "Graphics/CEGraphicsManager.h"

namespace ConceptEngineFramework {
	class CEFramework {
	public:
		CEFramework(std::wstring name,
		            HINSTANCE hInstance,
		            LPSTR lpCmdLine,
		            int nCmdShow,
		            int width,
		            int height,
		            Graphics::API graphicsAPI,
		            Graphics::CEPlayground* playground);

		CEFramework(HWND hWnd, Graphics::API graphicsApi, Graphics::CEPlayground* playground);
		int Run(bool editorMode = false) const;

		HINSTANCE GetHInstance() const;
	protected:
	private:
		//Game variables
		std::wstring m_name;
		HINSTANCE m_hInstance;
		LPSTR m_lpCmdLine;
		int m_nCmdShow;

		//Editor variables
		HWND m_hWnd;
	};
}
