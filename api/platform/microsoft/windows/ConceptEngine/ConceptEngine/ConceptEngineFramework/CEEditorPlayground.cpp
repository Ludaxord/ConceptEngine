#include "CEEditorPlayground.h"

using namespace Concept::GameEngine::Playground;

CEEditorPlayground::CEEditorPlayground(const std::wstring& name, uint32_t width, uint32_t height,
                                       bool vSync): CEPlayground(name, width, height, vSync) {
}

CEEditorPlayground::~CEEditorPlayground() {
}

bool CEEditorPlayground::LoadContent() {

	return true;
}

void CEEditorPlayground::UnloadContent() {
}

void CEEditorPlayground::OnUpdate(UpdateEventArgs& e) {
}

void CEEditorPlayground::OnRender() {
}

void CEEditorPlayground::OnKeyPressed(KeyEventArgs& e) {
}

void CEEditorPlayground::OnKeyReleased(KeyEventArgs& e) {
}

void CEEditorPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
}

void CEEditorPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
}

void CEEditorPlayground::OnResize(ResizeEventArgs& e) {
}

void CEEditorPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
}
