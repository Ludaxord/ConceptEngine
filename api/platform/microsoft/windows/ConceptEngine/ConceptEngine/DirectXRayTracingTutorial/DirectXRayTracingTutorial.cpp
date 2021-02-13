#include "DirectXRayTracingTutorial.h"

DirectXRayTracingTutorial::DirectXRayTracingTutorial(UINT width, UINT height) : Tutorial(
	width, height, L"Box Tutorial") {
}

void DirectXRayTracingTutorial::OnInit() {
}

void DirectXRayTracingTutorial::OnUpdate() {
}

void DirectXRayTracingTutorial::OnRender() {
}

void DirectXRayTracingTutorial::OnDestroy() {
}

void DirectXRayTracingTutorial::OnSizeChanged(UINT width, UINT height, bool minimized) {
}

void DirectXRayTracingTutorial::OnKeyDown(KeyCode key) {
}

void DirectXRayTracingTutorial::OnKeyUp(KeyCode key) {
}

void DirectXRayTracingTutorial::OnWindowMoved(int x, int y) {
}

void DirectXRayTracingTutorial::OnMouseMove(UINT x, UINT y) {
}

void DirectXRayTracingTutorial::OnMouseButtonDown(KeyCode key, UINT x, UINT y) {
}

void DirectXRayTracingTutorial::OnMouseButtonUp(KeyCode key, UINT x, UINT y) {
}

void DirectXRayTracingTutorial::OnDisplayChanged() {
}

wrl::ComPtr<IDXGISwapChain> DirectXRayTracingTutorial::GetSwapChain() {
	return m_deviceResources->GetSwapChain();
}
