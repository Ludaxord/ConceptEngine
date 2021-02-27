#include "CEDX12ShapesPlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Tools/CEUtils.h"

using namespace ConceptEngine::Playgrounds::DirectX12;
using namespace ConceptEngineFramework::Game;

CEDX12ShapesPlayground::CEDX12ShapesPlayground(): CEDX12Playground() {
}

void CEDX12ShapesPlayground::Create() {
	CEDX12Playground::Create();

	m_dx12manager->ResetCommandList();

	//Build root signature

	//Build shaders and input layout

	//build geometry

	//build render items

	//build frame resources

	//build descriptor heaps

	//build constant buffer views

	//build pipeline state object

	//execute command list
	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	auto commandList = m_dx12manager->GetD3D12CommandList();

	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* commandLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12ShapesPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);

	UpdateObjectCBs(gt);
	UpdateMainPassCB(gt);
}

void CEDX12ShapesPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12ShapesPlayground::Resize() {
	CEDX12Playground::Resize();
}

void CEDX12ShapesPlayground::UpdateCamera(const CETimer& gt) {
	CEDX12Playground::UpdateCamera(gt);
}

void CEDX12ShapesPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
}

void CEDX12ShapesPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
}

void CEDX12ShapesPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
}

void CEDX12ShapesPlayground::OnKeyUp(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyUp(key, keyChar);
}

void CEDX12ShapesPlayground::OnKeyDown(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyDown(key, keyChar);
}

void CEDX12ShapesPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12ShapesPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);
}

void CEDX12ShapesPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);
}

void CEDX12ShapesPlayground::BuildShadersAndInputLayout() {
}

void CEDX12ShapesPlayground::BuildShapesGeometry() {
}

void CEDX12ShapesPlayground::BuildRenderItems() {
}

void CEDX12ShapesPlayground::BuildFrameResources() {
}

void CEDX12ShapesPlayground::BuildConstantBufferViews() {
}

void CEDX12ShapesPlayground::BuildPSOs() {
}
