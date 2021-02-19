#include "CEDXInitPlayground.h"

using namespace ConceptEngineFramework::Game::Playgrounds;

CEDXInitPlayground::CEDXInitPlayground(const std::wstring& name,
                                       uint32_t width, uint32_t height, bool vSync) : CEPlayground(name, width, height, vSync) {
}

bool CEDXInitPlayground::LoadContent() {
	return true;
}

void CEDXInitPlayground::UnloadContent() {
}

void CEDXInitPlayground::OnUpdate(UpdateEventArgs& e) {
}

void CEDXInitPlayground::OnRender() {
}

void CEDXInitPlayground::OnKeyPressed(KeyEventArgs& e) {
}

void CEDXInitPlayground::OnKeyReleased(KeyEventArgs& e) {
}

void CEDXInitPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
}

void CEDXInitPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
}

void CEDXInitPlayground::OnResize(ResizeEventArgs& e) {
}

void CEDXInitPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
}
