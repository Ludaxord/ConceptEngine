#include "CEDirect3D12Manager.h"

#include "CED3D12Device.h"

CEDirect3D12Manager::CEDirect3D12Manager(const std::wstring& name, int width, int height, bool vSync): CEManager(
	name, width, height, vSync) {
}

CEDevice CEDirect3D12Manager::GetDevice() {
	return CED3D12Device();
}

bool CEDirect3D12Manager::LoadContent() {
	return false;
}

void CEDirect3D12Manager::UnloadContent() {
}

void CEDirect3D12Manager::OnUpdate() {
}

void CEDirect3D12Manager::OnRender() {
}

void CEDirect3D12Manager::OnKeyPressed() {
}

void CEDirect3D12Manager::OnMouseWheel() {
}

void CEDirect3D12Manager::OnResize() {
}

void CEDirect3D12Manager::TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
                                             Microsoft::WRL::ComPtr<ID3D12Resource> resource,
                                             D3D12_RESOURCE_STATES beforeState,
                                             D3D12_RESOURCE_STATES afterState) {
}

void CEDirect3D12Manager::ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
                                   D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor) {
}

void CEDirect3D12Manager::ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
                                     D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth) {
}

void CEDirect3D12Manager::UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
                                               ID3D12Resource** pDestinationResource,
                                               ID3D12Resource** pIntermediateResource, size_t numElements,
                                               size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags) {
}

void CEDirect3D12Manager::ResizeDepthBuffer(int width, int height) {
}

bool CEDirect3D12Manager::Initialize() {
	return false;
}

void CEDirect3D12Manager::Destroy() {
}

void CEDirect3D12Manager::OnKeyReleased() {
}

void CEDirect3D12Manager::OnMouseMoved() {
}

void CEDirect3D12Manager::OnMouseButtonPressed() {
}

void CEDirect3D12Manager::OnMouseButtonReleased() {
}

void CEDirect3D12Manager::OnWindowDestroy() {
}
