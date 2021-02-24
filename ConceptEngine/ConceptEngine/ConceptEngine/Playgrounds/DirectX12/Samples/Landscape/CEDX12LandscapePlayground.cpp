#include "CEDX12LandscapePlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"

using namespace ConceptEngine::Playgrounds::DirectX12;
using namespace ConceptEngineFramework::Game;

void CEDX12LandscapePlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	//Create basic waves

	//Create Root Signature

	//Create Shaders and input layout

	//Create Landscape geometry

	//Create geometry buffers

	//Create Render items

	//Create Frame resources

	//Create Pipeline State Objects
}

void CEDX12LandscapePlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);

}

void CEDX12LandscapePlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12LandscapePlayground::Resize() {
	CEDX12Playground::Resize();
}

void CEDX12LandscapePlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
}

void CEDX12LandscapePlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
}

void CEDX12LandscapePlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
}

void CEDX12LandscapePlayground::OnKeyUp(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyUp(key, keyChar);
}

void CEDX12LandscapePlayground::OnKeyDown(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyDown(key, keyChar);
}

void CEDX12LandscapePlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}
