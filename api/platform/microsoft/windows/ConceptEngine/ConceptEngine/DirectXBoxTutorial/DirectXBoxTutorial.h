#pragma once
#include <d3d12.h>


#include "../ConceptEngineRTFramework/GraphicsEngine/DirectX/Libs/d3dx12.h"
#include "../DirectXFrameworkTutorial/Tutorial.h"
#include "../DirectXFrameworkTutorial/Types.h"

class DirectXBoxTutorial : public Tutorial {

public:
	DirectXBoxTutorial(UINT width, UINT height);
	void OnInit() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnSizeChanged(UINT width, UINT height, bool minimized) override;
	void OnKeyDown(KeyCode key) override;
	void OnKeyUp(KeyCode key) override;
	void OnWindowMoved(int x, int y) override;
	void OnMouseMove(UINT x, UINT y) override;
	void OnMouseButtonDown(KeyCode key, UINT x, UINT y) override;
	void OnMouseButtonUp(KeyCode key, UINT x, UINT y) override;
	void OnDisplayChanged() override;
	void CreateDeviceDependentResources() override;
	void CreateWindowSizeDependentResources() override;
	void ReleaseDeviceDependentResources() override;
	void ReleaseWindowSizeDependentResources() override;
protected:
private:
	void ReCreateDirect3D();
	void MakeRayTracing();
	void CreateRayTracingInterfaces();
	void SerializeAndCreateRayTracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc,
	                                               wrl::ComPtr<ID3D12RootSignature>* rootSig);
	void CreateRootSignatures();
	void CreateLocalRootSignatureSubObjects(CD3DX12_STATE_OBJECT_DESC* rayTracingPipelineState);
	void CreateRayTracingPipelineStateObject();
	void CreateDescriptorHeap();
	void CreateRayTracingOutputResource();
	void BuildGeometry();
	void BuildAccelerationStructures();
	void BuildShaderTables();
	void CopyRayTracingOutputToBackBuffer();
	void CalculateFrameStats();
	UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse = UINT_MAX);

private:
	//TODO: Move to DirectXResources class just now place here to follow along tutorial
	//DirectX Raytracing (DXR) attributes
	wrl::ComPtr<ID3D12Device5> m_dxrDevice;
	wrl::ComPtr<ID3D12GraphicsCommandList4> m_dxrCommandList;
	wrl::ComPtr<ID3D12StateObject> m_dxrStateObject;

	//Root Signatures
	wrl::ComPtr<ID3D12RootSignature> m_rayTracingGlobalRootSignature;
	wrl::ComPtr<ID3D12RootSignature> m_rayTracingLocalRootSignature;

	//Descriptors
	wrl::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	UINT m_descriptorsAllocated;
	UINT m_descriptorSize;

	//Ray tracing scene
	RayGenConstantBuffer m_rayGenCB;

	//Geometry
	typedef UINT16 Index;

	struct Vertex {
		float v1;
		float v2;
		float v3;
	};

	wrl::ComPtr<ID3D12Resource> m_indexBuffer;
	wrl::ComPtr<ID3D12Resource> m_vertexBuffer;

	//Acceleration structure
	wrl::ComPtr<ID3D12Resource> m_accelerationStructure;
	wrl::ComPtr<ID3D12Resource> m_bottomLevelAccelerationStructure;
	wrl::ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;

	//Ray Tracing Output
	wrl::ComPtr<ID3D12Resource> m_rayTracingOutput;
	D3D12_GPU_DESCRIPTOR_HANDLE m_rayTracingOutputReosurceUAVFGPUDescriptor;
	UINT m_rayTracingOutputResourceUAVDescriptorHeapIndex;

	//Shader tables
	static const wchar_t* c_hitGroupName;
	static const wchar_t* c_rayGenShaderName;
	static const wchar_t* c_closestHitShaderName;
	static const wchar_t* c_missShaderName;

	wrl::ComPtr<ID3D12Resource> m_missShaderTable;
	wrl::ComPtr<ID3D12Resource> m_hitGroupShaderTable;
	wrl::ComPtr<ID3D12Resource> m_rayGenShaderTable;


};
