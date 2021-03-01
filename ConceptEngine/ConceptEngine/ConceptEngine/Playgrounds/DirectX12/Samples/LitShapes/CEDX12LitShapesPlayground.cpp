#include "CEDX12LitShapesPlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12LitShapesPlayground::CEDX12LitShapesPlayground() : CEDX12Playground() {
}

void CEDX12LitShapesPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list
	m_dx12manager->ResetCommandList();

	//Build root signature
	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	//Root parameter can be a table, root descriptor or root constants
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	slotRootParameter[2].InitAsConstantBufferView(2);

	//Root signature is an array of root parameters
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(3,
	                                              slotRootParameter,
	                                              0,
	                                              nullptr,
	                                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	m_rootSignature = m_dx12manager->CreateRootSignature(&rootSignatureDesc);

	//Build shaders and inputs layout
	BuildShadersAndInputLayout("CELitVertexShader.hlsl", "CELitPixelShader.hlsl");

	//build shapes geometry
	BuildGeometry();

	//build materials
	BuildMaterials();

	//build render items
	BuildRenderItems();

	//build frame resources
	BuildFrameResources();

	//build pso
	BuildPSOs(m_rootSignature);

	//execute command list
	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	auto commandList = m_dx12manager->GetD3D12CommandList();

	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* commandLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
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
	auto nodes = m_dx12manager->LoadNode("BrickLandspeeder4501.obj");
	spdlog::info("NODES: {}", nodes.size());
}

void CEDX12LitShapesPlayground::BuildMaterials() {
}

void CEDX12LitShapesPlayground::BuildRenderItems() {
}

void CEDX12LitShapesPlayground::BuildFrameResources() {
}
