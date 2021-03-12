#include "CEDX12PickingPlayground.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12PickingPlayground::CEDX12PickingPlayground(): CEDX12Playground() {
}

void CEDX12PickingPlayground::Create() {
	CEDX12Playground::Create();
}

void CEDX12PickingPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
}

void CEDX12PickingPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12PickingPlayground::Resize() {
	CEDX12Playground::Resize();
}

void CEDX12PickingPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
}

void CEDX12PickingPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
}

void CEDX12PickingPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
}

void CEDX12PickingPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12PickingPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyDown(key, keyChar, gt);
}

void CEDX12PickingPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12PickingPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);
}

void CEDX12PickingPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);
}

void CEDX12PickingPlayground::UpdateMaterialCBs(const CETimer& gt) {
	CEDX12Playground::UpdateMaterialCBs(gt);
}

void CEDX12PickingPlayground::BuildPSOs(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature) {
	CEDX12Playground::BuildPSOs(rootSignature);
}

void CEDX12PickingPlayground::LoadTextures() {
}

void CEDX12PickingPlayground::BuildDescriptorHeaps() {
}

void CEDX12PickingPlayground::BuildShapesGeometry() {
}

void CEDX12PickingPlayground::BuildFrameResources() {
}

void CEDX12PickingPlayground::BuildMaterials() {
}

void CEDX12PickingPlayground::BuildRenderItems() {
}

void CEDX12PickingPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                                              std::vector<Resources::RenderItem*>& ritems) const {
}
