#include "CEDX12Playground.h"

#include <spdlog/spdlog.h>


#include "../CEGraphicsManager.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;

CEDX12Playground::CEDX12Playground() {
};

void CEDX12Playground::Init(CEGraphicsManager* manager) {
	CEDX12Manager* dx12Manager = reinterpret_cast<CEDX12Manager*>(manager);
	m_dx12manager = dx12Manager;
}

void CEDX12Playground::Create() {
}

void CEDX12Playground::Update(const CETimer& gt) {
}

void CEDX12Playground::Render(const CETimer& gt) {
}

void CEDX12Playground::Resize() {
}

void CEDX12Playground::UpdateCamera(const CETimer& gt) {
}

void CEDX12Playground::OnMouseDown(Game::KeyCode key, int x, int y) {
}

void CEDX12Playground::OnMouseUp(Game::KeyCode key, int x, int y) {
}

void CEDX12Playground::OnMouseMove(Game::KeyCode key, int x, int y) {
}

void CEDX12Playground::OnKeyUp(Game::KeyCode key, char keyChar) {
}

void CEDX12Playground::OnKeyDown(Game::KeyCode key, char keyChar) {
}

void CEDX12Playground::OnMouseWheel(Game::KeyCode key, float wheelDelta, int x, int y) {
}

void CEDX12Playground::UpdateObjectCBs(const CETimer& gt) {
}

void CEDX12Playground::UpdateMainPassCB(const CETimer& gt) {
}
