#include "CERTXPlayground.h"

using namespace Concept::GameEngine::Playground;

CERTXPlayground::CERTXPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync) : CEPlayground(
	name, width, height, vSync),
	m_viewPort(CD3DX12_VIEWPORT(
		0.0f,
		0.0f,
		static_cast<float>(width),
		static_cast<float>(height))
	),
	m_scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX)), m_RenderScale(1.0f) {
}

CERTXPlayground::~CERTXPlayground() {
}

bool CERTXPlayground::LoadContent() {
}

void CERTXPlayground::UnloadContent() {
}

void CERTXPlayground::OnUpdate(UpdateEventArgs& e) {
}

void CERTXPlayground::OnRender() {
}

void CERTXPlayground::OnKeyPressed(KeyEventArgs& e) {
}

void CERTXPlayground::OnKeyReleased(KeyEventArgs& e) {
}

void CERTXPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
}

void CERTXPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
}

void CERTXPlayground::OnResize(ResizeEventArgs& e) {
}

void CERTXPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
}

void CERTXPlayground::OnGUI(const std::shared_ptr<GraphicsEngine::Direct3D::CECommandList>& commandList,
	const GraphicsEngine::Direct3D::CERenderTarget& renderTarget) {
}

void CERTXPlayground::RescaleHDRRenderTarget(float scale) {
}
