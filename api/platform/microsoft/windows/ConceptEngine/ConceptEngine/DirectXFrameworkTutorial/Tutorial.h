#pragma once
#include <activation.h>
#include <dxgi.h>
#include <intsafe.h>
#include <memory>
#include <string>
#include <wrl.h>

#include "DirectXResources.h"
#include "KeyCode.h"

namespace wrl = Microsoft::WRL;

class Tutorial {
public:
	Tutorial(UINT width, UINT height, std::wstring name);
	virtual ~Tutorial() = default;

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy();
	virtual void OnSizeChanged(UINT width, UINT height, bool minimized) = 0;

	virtual void OnKeyDown(KeyCode key) = 0;
	virtual void OnKeyUp(KeyCode key) = 0;
	virtual void OnWindowMoved(int x, int y) = 0;
	virtual void OnMouseMove(UINT x, UINT y) = 0;
	virtual void OnMouseButtonDown(KeyCode key, UINT x, UINT y);
	virtual void OnMouseButtonUp(KeyCode key, UINT x, UINT y) = 0;
	virtual void OnDisplayChanged() = 0;

	virtual void OnDeviceLost();
	virtual void OnDeviceRestored();
	virtual void CreateDeviceDependentResources() = 0;
	virtual void CreateWindowSizeDependentResources() = 0;
	virtual void ReleaseDeviceDependentResources() = 0;
	virtual void ReleaseWindowSizeDependentResources() = 0;

	std::wstring GetTitle() const { return m_title; }
	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }
	RECT GetWindowsBounds() const { return m_windowBounds; }
	DirectXResources* GetDirectXResources() const { return m_deviceResources.get(); }

	void UpdateForSizeChange(UINT clientWidth, UINT clientHeight);
	void SetWindowBounds(int left, int top, int right, int bottom);
	std::wstring GetAssetFullPath(LPCWSTR asssetName);

	virtual wrl::ComPtr<IDXGISwapChain> GetSwapChain() {
		return nullptr;
	}

protected:
	void SetCustomWindowText(LPCWSTR text);

	//Viewport Dimensions
	UINT m_width;
	UINT m_height;
	float m_aspectRatio;

	//Window bounds
	RECT m_windowBounds;

	bool m_enableUI;

	//DirectX resources
	UINT m_adapterID;
	bool m_vSync;
	std::unique_ptr<DirectXResources> m_deviceResources;
private:
	//Root assets path
	std::wstring m_assetsPath;

	//Window Title
	std::wstring m_title;
};
