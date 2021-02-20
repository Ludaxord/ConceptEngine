#pragma once
#include "CEScreen.h"

namespace ConceptEngineFramework::Game {
	class CEGame;

	class CEWindow : public CEScreen {

	public:
		void Create() override;
		void Destroy() override;
		void Show() override;
		void Hide() override;
		
		std::wstring GetName() override;

		HWND GetWindowHandle() const;
		int GetWidth() const;
		int GetHeight() const;

		void SetResolution(int width, int height);
		void SetWidth(int width);
		void SetHeight(int height);
	
	protected:
		friend class CEGame;
		friend class std::default_delete<CEWindow>;

		explicit CEWindow(std::wstring windowName, HINSTANCE hInstance, int width, int height);
		~CEWindow() = default;

		bool RegisterWindow() const;
		bool InitWindow();
	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;

		std::wstring m_windowName;
		std::wstring m_windowClassName;
		int m_width;
		int m_height;
	};
}
