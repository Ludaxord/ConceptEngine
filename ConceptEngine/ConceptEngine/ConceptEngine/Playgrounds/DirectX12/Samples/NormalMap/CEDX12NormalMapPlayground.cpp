#include "CEDX12NormalMapPlayground.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12NormalMapPlayground::CEDX12NormalMapPlayground(): CEDX12Playground() {
}

void CEDX12NormalMapPlayground::Create() {
	CEDX12Playground::Create();
}

void CEDX12NormalMapPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);

}

void CEDX12NormalMapPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12NormalMapPlayground::Resize() {
	CEDX12Playground::Resize();

}

void CEDX12NormalMapPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
}

void CEDX12NormalMapPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);

}

void CEDX12NormalMapPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);

}

void CEDX12NormalMapPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12NormalMapPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyDown(key, keyChar, gt);

}

void CEDX12NormalMapPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12NormalMapPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);
}

void CEDX12NormalMapPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);
}

void CEDX12NormalMapPlayground::UpdateMaterialCBs(const CETimer& gt) {
	CEDX12Playground::UpdateMaterialCBs(gt);
}

void CEDX12NormalMapPlayground::BuildPSOs(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature) {
	CEDX12Playground::BuildPSOs(rootSignature);
}

void CEDX12NormalMapPlayground::LoadTextures() {
}

void CEDX12NormalMapPlayground::BuildDescriptorHeaps() {
}

void CEDX12NormalMapPlayground::BuildShapesGeometry() {
}

void CEDX12NormalMapPlayground::BuildModelGeometry() {
}

void CEDX12NormalMapPlayground::BuildFrameResources() {
}

void CEDX12NormalMapPlayground::BuildMaterials() {
}

void CEDX12NormalMapPlayground::BuildRenderItems() {
}

void CEDX12NormalMapPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
	std::vector<Resources::RenderItem*>& ritems) const {
}

void CEDX12NormalMapPlayground::BuildCubeFaceCamera(float x, float y, float z) {
}

void CEDX12NormalMapPlayground::BuildCubeDepthStencil() {
}

void CEDX12NormalMapPlayground::DrawSceneToCubeMap() {
}

void CEDX12NormalMapPlayground::AnimateSkullMovement(const CETimer& gt) const {
}

void CEDX12NormalMapPlayground::UpdateCubeMapFacePassCBs() {
}
