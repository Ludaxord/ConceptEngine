#include "CERayTracingPlayground.h"

Concept::GameEngine::Playground::CERayTracingPlayground::CERayTracingPlayground(const std::wstring& name,
	uint32_t width, uint32_t height, bool vSync): CEPlayground(name, width, height, vSync) {
}

bool Concept::GameEngine::Playground::CERayTracingPlayground::LoadContent() {
	return true;
}

void Concept::GameEngine::Playground::CERayTracingPlayground::UnloadContent() {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnUpdate(UpdateEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnRender() {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnKeyPressed(KeyEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnKeyReleased(KeyEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnResize(ResizeEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
}
