#include "Tutorial.h"

#include "ConceptEngineRunner.h"
#include "DirectXHelper.h"

Tutorial::Tutorial(UINT width, UINT height, std::wstring name) : m_width(width),
                                                                 m_height(height),
                                                                 m_windowBounds{0, 0, 0, 0},
                                                                 m_title(name),
                                                                 m_aspectRatio(0.0f),
                                                                 m_enableUI(true),
                                                                 m_adapterID(0),
                                                                 m_vSync(false) {
	WCHAR assetsPath[512];
	GetAssetsPath(assetsPath, _countof(assetsPath));
	m_assetsPath = assetsPath;

	UpdateForSizeChange(width, height);
}

void Tutorial::OnDestroy() {
	// m_deviceResources->WaitForGPU();
	OnDeviceLost();
}

void Tutorial::OnMouseButtonDown(KeyCode key, UINT x, UINT y) {
}

void Tutorial::OnDeviceLost() {
	ReleaseWindowSizeDependentResources();
	ReleaseDeviceDependentResources();
}

void Tutorial::OnDeviceRestored() {
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void Tutorial::UpdateForSizeChange(UINT clientWidth, UINT clientHeight) {
	m_width = clientWidth;
	m_height = clientHeight;
	m_aspectRatio = static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
}

void Tutorial::SetWindowBounds(int left, int top, int right, int bottom) {
	m_windowBounds = {
		static_cast<LONG>(left),
		static_cast<LONG>(top),
		static_cast<LONG>(right),
		static_cast<LONG>(bottom)
	};
}

std::wstring Tutorial::GetAssetFullPath(LPCWSTR assetName) const {
	return m_assetsPath + assetName;
}

void Tutorial::SetCustomWindowText(LPCWSTR text) const {
	std::wstring windowText = m_title + L": " + text;
	SetWindowText(ConceptEngineRunner::GetHWnd(), windowText.c_str());
}
