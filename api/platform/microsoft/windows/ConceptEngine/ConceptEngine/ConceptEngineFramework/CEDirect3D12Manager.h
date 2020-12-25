#pragma once
#include <d3d12.h>

#include "CEManager.h"

class CEDirect3D12Manager : public CEManager {
public:
	using super = CEManager;
	CEDirect3D12Manager(const std::wstring& name, int width, int height, bool vSync);
protected:
	CEDevice GetDevice() override;
	virtual bool LoadContent() override;
	virtual void UnloadContent() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnKeyPressed() override;
	virtual void OnMouseWheel() override;
	virtual void OnResize() override;
public:
	virtual bool Initialize() override;
	virtual void Destroy() override;
protected:
	virtual void OnKeyReleased() override;
	virtual void OnMouseMoved() override;
	virtual void OnMouseButtonPressed() override;
	virtual void OnMouseButtonReleased() override;
	virtual void OnWindowDestroy() override;

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
