#include "DirectXResources.h"

bool DirectXResources::IsDirectXRayTracingSupported(wrl::ComPtr<IDXGIAdapter1> adapter) {
	return false;
}

DirectXResources::DirectXResources(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat, UINT backBufferCount,
	D3D_FEATURE_LEVEL minFeatureLevel, UINT flags, UINT adapterID) {
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
