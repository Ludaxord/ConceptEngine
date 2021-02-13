#pragma once
#include <intsafe.h>
#include <string>

class Tutorial {
public:
	Tutorial(UINT width, UINT height, std::wstring name);
	virtual ~Tutorial() = default;

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;
	virtual void OnSizeChanged(UINT width, UINT height, bool minimized) = 0;

	std::wstring GetTitle() { return m_title; }
	UINT GetWidth() { return m_width; }
	UINT GetHeight() { return m_height; }
	RECT GetWindowsBounds() { return m_windowBounds; }
	

	virtual wrl::ComPtr<IDXGISwapChain> GetSwapChain() {
		return nullptr;
	}
protected:
private:
};
