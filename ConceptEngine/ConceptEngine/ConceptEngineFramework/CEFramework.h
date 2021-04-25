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
		int Run(bool editorMode = false) const;
		HINSTANCE GetHInstance() const;

		//FOR EDITOR
		//TODO: For editor Create Custom Timer from QTimer class that imitate behavior of CETimer;
		CEFramework(HWND hWnd, Graphics::API graphicsApi, int width, int height, Graphics::CEPlayground* playground);
		void EditorRun(int width, int height) const;
		void EditorUpdateTimer() const;
		void EditorUpdate() const;
		void EditorRender() const;
		void EditorResize(int width, int height) const;
		void EditorKeyDown(WPARAM keyCode, std::string keyChar);
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
