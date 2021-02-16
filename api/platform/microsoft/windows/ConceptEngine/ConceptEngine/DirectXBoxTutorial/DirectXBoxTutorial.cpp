#include "DirectXBoxTutorial.h"

#include <d3dcompiler.h>

#include "../DirectXFrameworkTutorial/ConceptEngineRunner.h"
#include "../DirectXFrameworkTutorial/DirectXHelper.h"

const wchar_t* DirectXBoxTutorial::c_hitGroupName = L"BoxHitGroup";
const wchar_t* DirectXBoxTutorial::c_rayGenShaderName = L"BoxRayGenShader";
const wchar_t* DirectXBoxTutorial::c_closestHitShaderName = L"BoxClosestHitShader";
const wchar_t* DirectXBoxTutorial::c_missShaderName = L"BoxMissShader";

// #include "x64/Debug/CompiledShaders/BoxRayTracing.hlsl.h"

DirectXBoxTutorial::DirectXBoxTutorial(UINT width, UINT height) : Tutorial(width, height, L"Box Tutorial"),
                                                                  m_rayTracingOutputResourceUAVDescriptorHeapIndex(
	                                                                  UINT_MAX) {
	m_rayGenCB.viewport = {-1.0f, -1.0f, 1.0f, 1.0f};
	UpdateForSizeChange(width, height);
}

void DirectXBoxTutorial::OnInit() {
	m_deviceResources = std::make_unique<DirectXResources>(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		FrameCount,
		D3D_FEATURE_LEVEL_11_0,
		DirectXResources::c_requireTearingSupport,
		m_adapterID
	);

	m_deviceResources->RegisterDeviceNotify(this);
	m_deviceResources->SetWindow(ConceptEngineRunner::GetHWnd(), m_width, m_height);
	m_deviceResources->InitializeDXGIAdapter();

	ThrowIfFalse(DirectXResources::IsDirectXRayTracingSupported(m_deviceResources->GetAdapter()),
	             L"Error: DirectX Ray Tracing is not supported in this Machine");

	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void DirectXBoxTutorial::OnUpdate() {
}

void DirectXBoxTutorial::OnRender() {
}

void DirectXBoxTutorial::OnSizeChanged(UINT width, UINT height, bool minimized) {
}

void DirectXBoxTutorial::OnKeyDown(KeyCode key) {
}

void DirectXBoxTutorial::OnKeyUp(KeyCode key) {
}

void DirectXBoxTutorial::OnWindowMoved(int x, int y) {
}

void DirectXBoxTutorial::OnMouseMove(UINT x, UINT y) {
}

void DirectXBoxTutorial::OnMouseButtonDown(KeyCode key, UINT x, UINT y) {
}

void DirectXBoxTutorial::OnMouseButtonUp(KeyCode key, UINT x, UINT y) {
}

void DirectXBoxTutorial::OnDisplayChanged() {
}

//Create resources that depend on device
void DirectXBoxTutorial::CreateDeviceDependentResources() {
	//Initialize RayTracing Pipeline

	//Create RayTracing interfaces:
	// * Ray Tracing Device
	// * Ray Tracing Command List
	CreateRayTracingInterfaces();

	//Create Root Signatures for shaders
	CreateRootSignatures();

	//Create Ray Tracing Pipeline State Object Which defines binding of shaders
	CreateRayTracingPipelineStateObject();

	//Create heap for descriptors
	CreateDescriptorHeap();

	//Build geometry to be used in sample
	BuildGeometry();

	//Build RayTracing acceleration structures from generated geometry
	BuildAccelerationStructures();

	//Build shader tables, which define shaders and their local root arguments.
	BuildShaderTables();

	//Create output 2D texture to store RayTracing result to
	CreateRayTracingOutputResource();
}

void DirectXBoxTutorial::CreateWindowSizeDependentResources() {
}

void DirectXBoxTutorial::ReleaseDeviceDependentResources() {
}

void DirectXBoxTutorial::ReleaseWindowSizeDependentResources() {
}

void DirectXBoxTutorial::ReCreateDirect3D() {
}

void DirectXBoxTutorial::MakeRayTracing() {
}

void DirectXBoxTutorial::CreateRayTracingInterfaces() {
	auto device = m_deviceResources->GetD3DDevice();
	auto commandList = m_deviceResources->GetCommandList();

	ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&m_dxrDevice)),
	              L"Couldn't get DirectX Ray Tracing interface for the device.");
	ThrowIfFailed(commandList->QueryInterface(IID_PPV_ARGS(&m_dxrCommandList)),
	              L"Couldn't get DirectX Ray Tracing interface for the command list.");
}

void DirectXBoxTutorial::SerializeAndCreateRayTracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc,
                                                                   wrl::ComPtr<ID3D12RootSignature>* rootSig) {
	auto device = m_deviceResources->GetD3DDevice();
	wrl::ComPtr<ID3DBlob> blob;
	wrl::ComPtr<ID3DBlob> error;

	ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error),
	              error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr);
	ThrowIfFailed(device->CreateRootSignature(1,
	                                          blob->GetBufferPointer(),
	                                          blob->GetBufferSize(),
	                                          IID_PPV_ARGS(&(*rootSig))
	));
}

void DirectXBoxTutorial::CreateRootSignatures() {
	//Global root signature
	//This is root signature that is shared across all Ray Tracing shaders invoked during a DispatchRays() call.
	{
		CD3DX12_DESCRIPTOR_RANGE UAVDescriptor;
		UAVDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
		rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &UAVDescriptor);
		rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		SerializeAndCreateRayTracingRootSignature(globalRootSignatureDesc, &m_rayTracingGlobalRootSignature);
	}
}

//Local root signature shader association
//Root signature that enables shader to have unique arguments that come from shader tables
void DirectXBoxTutorial::CreateLocalRootSignatureSubObjects(CD3DX12_STATE_OBJECT_DESC* rayTracingPipelineState) {
	//Hit group and miss shaders,
	{
		auto localRootSignature = rayTracingPipelineState->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		localRootSignature->SetRootSignature(m_rayTracingLocalRootSignature.Get());

		auto rootSignatureAssociation = rayTracingPipelineState->CreateSubobject<
			CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
		rootSignatureAssociation->AddExport(c_rayGenShaderName);
	}
}

//Create Ray Tracing pipeline state object (RTPSO).
//An RTPSO represents a full set of shaders reachable by DispatchRays() call,
//With all configuration options resolved such as local signatures and other state.
void DirectXBoxTutorial::CreateRayTracingPipelineStateObject() {
	//Create 7 subobjects that combine into a RTPSO
	//Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations
	//Default association applies to every exported shader entryPoint that does not have any of same type of subobjects associated with it.
	//Simple sample utilizes default shader association except for local root signature subObject
	//which has an explicit association specified purely for demonstration purposes.
	//1 - DXIL Library
	//1 - Triangle hit group
	//1 - Shader config
	//1 - Local root signature and association
	//1 - Global root signature
	//1 - Pipeline config
	CD3DX12_STATE_OBJECT_DESC rayTracingPipeline{
		D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE
	};

	//DXIL Library
	//Contains shaders and their entrypoints for state objects
	//Since shaders are not considered a subobject, they need to be passed in via DXIL Library
	WCHAR assetsPath[512];
	GetAssetsPath(assetsPath, _countof(assetsPath));
	std::wstring m_assetsPath = assetsPath;
	std::wstring lib_assetsPath = m_assetsPath + L"BoxRayTracing.cso";
	Microsoft::WRL::ComPtr<ID3DBlob> libShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(lib_assetsPath.c_str(), &libShaderBlob));

	auto lib = rayTracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	//TODO: Try compile shader during runtime
	// D3D12_SHADER_BYTECODE libDXIL = CD3DX12_SHADER_BYTECODE((void*)g_pBoxRayTracing, ARRAYSIZE(g_pBoxRayTracing));
	D3D12_SHADER_BYTECODE libDXIL = CD3DX12_SHADER_BYTECODE(libShaderBlob.Get());
	lib->SetDXILLibrary(&libDXIL);
	//Define which shader exports to surface from libraryt
	// If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
	// In this sample, this could be omitted for convenience since the sample uses all shaders in the library.
	{
		lib->DefineExport(c_rayGenShaderName);
		lib->DefineExport(c_closestHitShaderName);
		lib->DefineExport(c_missShaderName);
	}

	//Triangle hit group
	// A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
	// In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
	auto hitGroup = rayTracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
	hitGroup->SetHitGroupExport(c_hitGroupName);
	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	//Shader config
	//Defines maxiumum sizes in bytes for ray payload and attribute structure
	auto shaderConfig = rayTracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	UINT payloadSize = 4 * sizeof(float);
	UINT attributeSize = 2 * sizeof(float);
	shaderConfig->Config(payloadSize, attributeSize);

	//Local root signature and shader association
	//This is a root signature that enables a shader to have unique arguments that comes from shader table
	CreateLocalRootSignatureSubObjects(&rayTracingPipeline);

	//Global root signature
	//This is a root signature that is shared across all RayTracing Shaders invoked during a DispatchRays() call.
	auto globalRootSignature = rayTracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	globalRootSignature->SetRootSignature(m_rayTracingGlobalRootSignature.Get());

	//Pipeline config
	//Defines maximum TraceRay() recursion depth.
	auto pipelineConfig = rayTracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	//PERFORMANCE TIP: Set max recursion depth as low as needed
	//as drivers may apply optimization strategies for low recursion depths.
	UINT maxRecursionDepth = 1; // ~ primary rays only
	pipelineConfig->Config(maxRecursionDepth);

#if _DEBUG
	PrintStateObjectDesc(rayTracingPipeline);
#endif

	//Create state object
	ThrowIfFailed(m_dxrDevice->CreateStateObject(rayTracingPipeline, IID_PPV_ARGS(&m_dxrStateObject)),
	              L"Couldn't create DirectX RayTracing state object.");
}

void DirectXBoxTutorial::CreateDescriptorHeap() {
	auto device = m_deviceResources->GetD3DDevice();

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	//Allocate heap for single descriptor:
	// 1 - RayTracing Output texture UAV
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.NodeMask = 0;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
}

//Create 2D output texture for RayTracing
void DirectXBoxTutorial::CreateRayTracingOutputResource() {
	auto device = m_deviceResources->GetD3DDevice();
	auto backBufferFormat = m_deviceResources->GetBackBufferFormat();

	//Create output resource. Dimensions and format should match swap-chain
	auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(backBufferFormat,
	                                            m_width,
	                                            m_height,
	                                            1,
	                                            1,
	                                            1,
	                                            0,
	                                            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&uavDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&m_rayTracingOutput)
	));
	NAME_D3D12_OBJECT(m_rayTracingOutput);

	D3D12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle;
	m_rayTracingOutputResourceUAVDescriptorHeapIndex = AllocateDescriptor(
		&uavDescriptorHandle, m_rayTracingOutputResourceUAVDescriptorHeapIndex);
	D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	device->CreateUnorderedAccessView(m_rayTracingOutput.Get(), nullptr, &UAVDesc, uavDescriptorHandle);
	m_rayTracingOutputReosurceUAVFGPUDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_rayTracingOutputResourceUAVDescriptorHeapIndex,
		m_descriptorSize);
}

void DirectXBoxTutorial::BuildGeometry() {
}

void DirectXBoxTutorial::BuildAccelerationStructures() {
}

void DirectXBoxTutorial::BuildShaderTables() {
}

void DirectXBoxTutorial::CopyRayTracingOutputToBackBuffer() {
}

void DirectXBoxTutorial::CalculateFrameStats() {
}

UINT DirectXBoxTutorial::AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse) {
	return 0;
}
