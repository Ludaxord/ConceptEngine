#include "CEDX12LitShapesPlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12LitShapesPlayground::CEDX12LitShapesPlayground() : CEDX12Playground() {
}

void CEDX12LitShapesPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list

	//Build root signature

	//Build shaders and inputs layout

	//build shapes geometry
	BuildGeometry();

	//build materials

	//build render items

	//build frame resources

	//build pso

	//execute command list

	//flush command queue
}

void CEDX12LitShapesPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
}

void CEDX12LitShapesPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12LitShapesPlayground::Resize() {
	CEDX12Playground::Resize();
}

void CEDX12LitShapesPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
}

void CEDX12LitShapesPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
}

void CEDX12LitShapesPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
}

void CEDX12LitShapesPlayground::OnKeyUp(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyUp(key, keyChar);
}

void CEDX12LitShapesPlayground::OnKeyDown(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyDown(key, keyChar);
}

void CEDX12LitShapesPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12LitShapesPlayground::UpdateCamera(const CETimer& gt) {
	CEDX12Playground::UpdateCamera(gt);
}

void CEDX12LitShapesPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);
}

void CEDX12LitShapesPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);
}

void CEDX12LitShapesPlayground::BuildGeometry() {
	auto node = m_dx12manager->LoadNode("BrickLandspeeder4501.obj");
	spdlog::info("NODES: {}", node.size());
}
