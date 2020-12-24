#pragma once
#include <d3d12.h>

#include "CEManager.h"

class CEDirect3D12Manager : public CEManager {
protected:
	CEDevice GetDevice() override;
	
    // virtual void OnUpdate() override;
    // virtual void OnRender() override;
    // virtual void OnKeyPressed() override;
    // virtual void OnMouseWheel() override;
    // virtual void OnResize() override;

private:
	void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	                        Microsoft::WRL::ComPtr<ID3D12Resource> resource,
	                        D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);
	void ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	              D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor);
	void ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	                D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f);
	void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	                          ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource,
	                          size_t numElements, size_t elementSize, const void* bufferData,
	                          D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
	void ResizeDepthBuffer(int width, int height);
};
