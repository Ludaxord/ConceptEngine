#include "DirectXResources.h"

bool DirectXResources::IsDirectXRayTracingSupported(wrl::ComPtr<IDXGIAdapter1> adapter) {
	return true;
}

DirectXResources::DirectXResources(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat, UINT backBufferCount,
                                   D3D_FEATURE_LEVEL minFeatureLevel, UINT flags, UINT adapterID) :
	m_backBufferIndex(0),
	m_fenceValues{},
	m_rtvDescriptorSize(0),
	m_screenViewPort{},
	m_scissorRect{},
	m_backBufferFormat(backBufferFormat),
	m_depthBufferFormat(depthBufferFormat),
	m_backBufferCount(backBufferCount),
	m_d3dMinFeatureLevel(minFeatureLevel),
	m_outputSize{0, 0, 1, 1},
	m_options(flags),
	m_isWindowVisible(true),
	m_adapterIDoverride(adapterID),
	m_adapterID(UINT_MAX) {
}

DirectXResources::~DirectXResources() {
}

void DirectXResources::InitializeDXGIAdapter() {
}

void DirectXResources::CreateDeviceResources() {
}

void DirectXResources::CreateWindowSizeDependentResources() {
}

void DirectXResources::SetWindow(HWND window, int width, int height) {
}

void DirectXResources::WindowSizeChanged(int width, int height, bool minimized) {
}

void DirectXResources::HandleDeviceLost() {
}

void DirectXResources::Prepare(D3D12_RESOURCE_STATES beforeState) {
}

void DirectXResources::Present(D3D12_RESOURCE_STATES beforeState) {
}

void DirectXResources::ExecuteCommandList() {
}

void DirectXResources::WaitForGPU() noexcept {
}
