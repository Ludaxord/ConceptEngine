//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "DirectXBoxTutorial.h"

#include <d3dcompiler.h>


#include "../DirectXFrameworkTutorial/DirectXRayTracingHelper.h"

const wchar_t* DirectXBoxTutorial::c_hitGroupName = L"BoxHitGroup";
const wchar_t* DirectXBoxTutorial::c_rayGenShaderName = L"BoxRayGenShader";
const wchar_t* DirectXBoxTutorial::c_closestHitShaderName = L"BoxClosestHitShader";
const wchar_t* DirectXBoxTutorial::c_missShaderName = L"BoxMissShader";

DirectXBoxTutorial::DirectXBoxTutorial(UINT width, UINT height) :
	Tutorial(width, height, L"Box Tutorial"),
	m_rayTracingOutputResourceUAVDescriptorHeapIndex(UINT_MAX) {
	m_rayGenCB.viewport = {-1.0f, -1.0f, 1.0f, 1.0f};
	UpdateForSizeChange(width, height);
}

void DirectXBoxTutorial::OnInit() {
	m_deviceResources = std::make_unique<DirectXResources>(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		FrameCount,
		D3D_FEATURE_LEVEL_11_0,
		// Sample shows handling of use cases with tearing support, which is OS dependent and has been supported since TH2.
		// Since the sample requires build 1809 (RS5) or higher, we don't need to handle non-tearing cases.
		DirectXResources::c_requireTearingSupport,
		m_adapterIDoverride
	);
	m_deviceResources->RegisterDeviceNotify(this);
	m_deviceResources->SetWindow(ConceptEngineRunner::GetHWnd(), m_width, m_height);
	m_deviceResources->InitializeDXGIAdapter();

	ThrowIfFalse(IsDirectXRayTracingSupported(m_deviceResources->GetAdapter().Get()),
	             L"ERROR: DirectX Raytracing is not supported by your OS, GPU and/or driver.\n\n");

	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Create resources that depend on the device.
void DirectXBoxTutorial::CreateDeviceDependentResources() {
	// Initialize raytracing pipeline.

	// Create raytracing interfaces: raytracing device and commandlist.
	CreateRayTracingInterfaces();

	// Create root signatures for the shaders.
	CreateRootSignatures();

	// Create a raytracing pipeline state object which defines the binding of shaders, state and resources to be used during raytracing.
	CreateRayTracingPipelineStateObject();

	// Create a heap for descriptors.
	CreateDescriptorHeap();

	// Build geometry to be used in the sample.
	BuildGeometry();

	// Build raytracing acceleration structures from the generated geometry.
	BuildAccelerationStructures();

	// Build shader tables, which define shaders and their local root arguments.
	BuildShaderTables();

	// Create an output 2D texture to store the raytracing result to.
	CreateRayTracingOutputResource();
}

void DirectXBoxTutorial::SerializeAndCreateRayTracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc,
                                                                   wrl::ComPtr<ID3D12RootSignature>* rootSig) {
	auto device = m_deviceResources->GetD3DDevice();
	wrl::ComPtr<ID3DBlob> blob;
	wrl::ComPtr<ID3DBlob> error;

	ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error),
	              error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr);
	ThrowIfFailed(device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(),
	                                          IID_PPV_ARGS(&(*rootSig))));
}

void DirectXBoxTutorial::CreateRootSignatures() {
	// Global Root Signature
	// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
	{
		CD3DX12_DESCRIPTOR_RANGE UAVDescriptor;
		UAVDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
		rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &UAVDescriptor);
		rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		SerializeAndCreateRayTracingRootSignature(globalRootSignatureDesc, &m_rayTracingGlobalRootSignature);
	}

	// Local Root Signature
	// This is a root signature that enables a shader to have unique arguments that come from shader tables.
	{
		CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignatureParams::Count];
		rootParameters[LocalRootSignatureParams::ViewportConstantSlot].
			InitAsConstants(SizeOfInUint32(m_rayGenCB), 0, 0);
		CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		SerializeAndCreateRayTracingRootSignature(localRootSignatureDesc, &m_rayTracingLocalRootSignature);
	}
}

// Create raytracing device and command list.
void DirectXBoxTutorial::CreateRayTracingInterfaces() {
	auto device = m_deviceResources->GetD3DDevice();
	auto commandList = m_deviceResources->GetCommandList();

	ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&m_dxrDevice)),
	              L"Couldn't get DirectX Raytracing interface for the device.\n");
	ThrowIfFailed(commandList->QueryInterface(IID_PPV_ARGS(&m_dxrCommandList)),
	              L"Couldn't get DirectX Raytracing interface for the command list.\n");
}

// Local root signature and shader association
// This is a root signature that enables a shader to have unique arguments that come from shader tables.
void DirectXBoxTutorial::CreateLocalRootSignatureSubObjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline) {
	// Hit group and miss shaders in this sample are not using a local root signature and thus one is not associated with them.

	// Local root signature to be used in a ray gen shader.
	{
		auto localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		localRootSignature->SetRootSignature(m_rayTracingLocalRootSignature.Get());
		// Shader association
		auto rootSignatureAssociation = raytracingPipeline->CreateSubobject<
			CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
		rootSignatureAssociation->AddExport(c_rayGenShaderName);
	}
}

// Create a raytracing pipeline state object (RTPSO).
// An RTPSO represents a full set of shaders reachable by a DispatchRays() call,
// with all configuration options resolved, such as local signatures and other state.
void DirectXBoxTutorial::CreateRayTracingPipelineStateObject() {
	// Create 7 subobjects that combine into a RTPSO:
	// Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
	// Default association applies to every exported shader entrypoint that doesn't have any of the same type of subobject associated with it.
	// This simple sample utilizes default shader association except for local root signature subobject
	// which has an explicit association specified purely for demonstration purposes.
	// 1 - DXIL library
	// 1 - Triangle hit group
	// 1 - Shader config
	// 2 - Local root signature and association
	// 1 - Global root signature
	// 1 - Pipeline config
	CD3DX12_STATE_OBJECT_DESC raytracingPipeline{D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE};


	// DXIL library
	// This contains the shaders and their entrypoints for the state object.
	// Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
	wrl::ComPtr<ID3DBlob> libShaderBlob;
	{
		WCHAR assetsPath[512];
		GetAssetsPath(assetsPath, _countof(assetsPath));
		std::wstring m_assetsPath = assetsPath;
		std::wstring lib_assetsPath = m_assetsPath + L"BoxRayTracing.cso";
		ThrowIfFailed(D3DReadFileToBlob(lib_assetsPath.c_str(), &libShaderBlob));
	}
	auto lib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE(libShaderBlob.Get());
	lib->SetDXILLibrary(&libdxil);
	// Define which shader exports to surface from the library.
	// If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
	// In this sample, this could be omitted for convenience since the sample uses all shaders in the library. 
	{
		lib->DefineExport(c_rayGenShaderName);
		lib->DefineExport(c_closestHitShaderName);
		lib->DefineExport(c_missShaderName);
	}

	// Triangle hit group
	// A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
	// In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
	auto hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
	hitGroup->SetHitGroupExport(c_hitGroupName);
	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	// Shader config
	// Defines the maximum sizes in bytes for the ray payload and attribute structure.
	auto shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	UINT payloadSize = 4 * sizeof(float); // float4 color
	UINT attributeSize = 2 * sizeof(float); // float2 barycentrics
	shaderConfig->Config(payloadSize, attributeSize);

	// Local root signature and shader association
	CreateLocalRootSignatureSubObjects(&raytracingPipeline);
	// This is a root signature that enables a shader to have unique arguments that come from shader tables.

	// Global root signature
	// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
	auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	globalRootSignature->SetRootSignature(m_rayTracingGlobalRootSignature.Get());

	// Pipeline config
	// Defines the maximum TraceRay() recursion depth.
	auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	// PERFOMANCE TIP: Set max recursion depth as low as needed 
	// as drivers may apply optimization strategies for low recursion depths. 
	UINT maxRecursionDepth = 1; // ~ primary rays only. 
	pipelineConfig->Config(maxRecursionDepth);

#if _DEBUG
	PrintStateObjectDesc(raytracingPipeline);
#endif

	// Create the state object.
	ThrowIfFailed(m_dxrDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_dxrStateObject)),
	              L"Couldn't create DirectX Raytracing state object.\n");
}

// Create 2D output texture for raytracing.
void DirectXBoxTutorial::CreateRayTracingOutputResource() {
	auto device = m_deviceResources->GetD3DDevice();
	auto backbufferFormat = m_deviceResources->GetBackBufferFormat();

	// Create the output resource. The dimensions and format should match the swap-chain.
	auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(backbufferFormat, m_width, m_height, 1, 1, 1, 0,
	                                            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &uavDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr,
		IID_PPV_ARGS(&m_rayTracingOutput)));
	NAME_D3D12_OBJECT(m_rayTracingOutput);

	D3D12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle;
	m_rayTracingOutputResourceUAVDescriptorHeapIndex = AllocateDescriptor(
		&uavDescriptorHandle, m_rayTracingOutputResourceUAVDescriptorHeapIndex);
	D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	device->CreateUnorderedAccessView(m_rayTracingOutput.Get(), nullptr, &UAVDesc, uavDescriptorHandle);
	m_rayTracingOutputResourceUAVGPUDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_rayTracingOutputResourceUAVDescriptorHeapIndex,
		m_descriptorSize);
}

void DirectXBoxTutorial::CreateDescriptorHeap() {
	auto device = m_deviceResources->GetD3DDevice();

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	// Allocate a heap for a single descriptor:
	// 1 - raytracing output texture UAV
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
	NAME_D3D12_OBJECT(m_descriptorHeap);

	m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// Build geometry used in the sample.
void DirectXBoxTutorial::BuildGeometry() {
	auto device = m_deviceResources->GetD3DDevice();
	Index indices[] =
	{
		0, 1, 2
	};

	float depthValue = 1.0;
	float offset = 0.7f;
	Vertex vertices[] =
	{
		// The sample raytraces in screen space coordinates.
		// Since DirectX screen space coordinates are right handed (i.e. Y axis points down).
		// Define the vertices in counter clockwise order ~ clockwise in left handed.
		{0, -offset, depthValue},
		{-offset, offset, depthValue},
		{offset, offset, depthValue}
	};

	AllocateUploadBuffer(device.Get(), vertices, sizeof(vertices), &m_vertexBuffer);
	AllocateUploadBuffer(device.Get(), indices, sizeof(indices), &m_indexBuffer);
}

// Build acceleration structures needed for raytracing.
void DirectXBoxTutorial::BuildAccelerationStructures() {
	auto device = m_deviceResources->GetD3DDevice();
	auto commandList = m_deviceResources->GetCommandList();
	auto commandQueue = m_deviceResources->GetCommandQueue();
	auto commandAllocator = m_deviceResources->GetCommandAllocator();

	// Reset the command list for the acceleration structure construction.
	commandList->Reset(commandAllocator.Get(), nullptr);

	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.IndexBuffer = m_indexBuffer->GetGPUVirtualAddress();
	geometryDesc.Triangles.IndexCount = static_cast<UINT>(m_indexBuffer->GetDesc().Width) / sizeof(Index);
	geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
	geometryDesc.Triangles.Transform3x4 = 0;
	geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometryDesc.Triangles.VertexCount = static_cast<UINT>(m_vertexBuffer->GetDesc().Width) / sizeof(Vertex);
	geometryDesc.Triangles.VertexBuffer.StartAddress = m_vertexBuffer->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);

	// Mark the geometry as opaque. 
	// PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
	// Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	// Get required sizes for an acceleration structure.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
	topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	topLevelInputs.Flags = buildFlags;
	topLevelInputs.NumDescs = 1;
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
	m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
	ThrowIfFalse(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = topLevelInputs;
	bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	bottomLevelInputs.pGeometryDescs = &geometryDesc;
	m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
	ThrowIfFalse(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

	wrl::ComPtr<ID3D12Resource> scratchResource;
	AllocateUAVBuffer(device.Get(),
	                  max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes),
	                  &scratchResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

	// Allocate resources for acceleration structures.
	// Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
	// Default heap is OK since the application doesn’t need CPU read/write access to them. 
	// The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
	// and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
	//  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
	//  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
	{
		D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

		AllocateUAVBuffer(device.Get(), bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes,
		                  &m_bottomLevelAccelerationStructure, initialResourceState,
		                  L"BottomLevelAccelerationStructure");
		AllocateUAVBuffer(device.Get(), topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_topLevelAccelerationStructure,
		                  initialResourceState, L"TopLevelAccelerationStructure");
	}

	// Create an instance desc for the bottom-level acceleration structure.
	wrl::ComPtr<ID3D12Resource> instanceDescs;
	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
	instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
	instanceDesc.InstanceMask = 1;
	instanceDesc.AccelerationStructure = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
	AllocateUploadBuffer(device.Get(), &instanceDesc, sizeof(instanceDesc), &instanceDescs, L"InstanceDescs");

	// Bottom Level Acceleration Structure desc
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
	{
		bottomLevelBuildDesc.Inputs = bottomLevelInputs;
		bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
		bottomLevelBuildDesc.DestAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
	}

	// Top Level Acceleration Structure desc
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	{
		topLevelInputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();
		topLevelBuildDesc.Inputs = topLevelInputs;
		topLevelBuildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
		topLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
	}

	auto BuildAccelerationStructure = [&](auto* raytracingCommandList) {
		raytracingCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
		const D3D12_RESOURCE_BARRIER uav = CD3DX12_RESOURCE_BARRIER::UAV(m_bottomLevelAccelerationStructure.Get());
		commandList->ResourceBarrier(1, &uav);
		raytracingCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
	};

	// Build acceleration structure.
	BuildAccelerationStructure(m_dxrCommandList.Get());

	// Kick off acceleration structure construction.
	m_deviceResources->ExecuteCommandList();

	// Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
	m_deviceResources->WaitForGPU();
}

// Build shader tables.
// This encapsulates all shader records - shaders and the arguments for their local root signatures.
void DirectXBoxTutorial::BuildShaderTables() {
	auto device = m_deviceResources->GetD3DDevice();

	void* rayGenShaderIdentifier;
	void* missShaderIdentifier;
	void* hitGroupShaderIdentifier;

	auto GetShaderIdentifiers = [&](auto* stateObjectProperties) {
		rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_rayGenShaderName);
		missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_missShaderName);
		hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_hitGroupName);
	};

	// Get shader identifiers.
	UINT shaderIdentifierSize;
	{
		wrl::ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
		ThrowIfFailed(m_dxrStateObject.As(&stateObjectProperties));
		GetShaderIdentifiers(stateObjectProperties.Get());
		shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	}

	// Ray gen shader table
	{
		struct RootArguments {
			RayGenConstantBuffer cb;
		} rootArguments;
		rootArguments.cb = m_rayGenCB;

		UINT numShaderRecords = 1;
		UINT shaderRecordSize = shaderIdentifierSize + sizeof(rootArguments);
		ShaderTable rayGenShaderTable(device.Get(), numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
		rayGenShaderTable.push_back(ShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize, &rootArguments,
		                                         sizeof(rootArguments)));
		m_rayGenShaderTable = rayGenShaderTable.GetResource();
	}

	// Miss shader table
	{
		UINT numShaderRecords = 1;
		UINT shaderRecordSize = shaderIdentifierSize;
		ShaderTable missShaderTable(device.Get(), numShaderRecords, shaderRecordSize, L"MissShaderTable");
		missShaderTable.push_back(ShaderRecord(missShaderIdentifier, shaderIdentifierSize));
		m_missShaderTable = missShaderTable.GetResource();
	}

	// Hit group shader table
	{
		UINT numShaderRecords = 1;
		UINT shaderRecordSize = shaderIdentifierSize;
		ShaderTable hitGroupShaderTable(device.Get(), numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
		hitGroupShaderTable.push_back(ShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize));
		m_hitGroupShaderTable = hitGroupShaderTable.GetResource();
	}
}

// Update frame-based values.
void DirectXBoxTutorial::OnUpdate() {
	m_timer.Tick();
	CalculateFrameStats();
}

void DirectXBoxTutorial::MakeRayTracing() {
	auto commandList = m_deviceResources->GetCommandList();

	auto DispatchRays = [&](auto* commandList, auto* stateObject, auto* dispatchDesc) {
		// Since each shader table has only one shader record, the stride is same as the size.
		dispatchDesc->HitGroupTable.StartAddress = m_hitGroupShaderTable->GetGPUVirtualAddress();
		dispatchDesc->HitGroupTable.SizeInBytes = m_hitGroupShaderTable->GetDesc().Width;
		dispatchDesc->HitGroupTable.StrideInBytes = dispatchDesc->HitGroupTable.SizeInBytes;
		dispatchDesc->MissShaderTable.StartAddress = m_missShaderTable->GetGPUVirtualAddress();
		dispatchDesc->MissShaderTable.SizeInBytes = m_missShaderTable->GetDesc().Width;
		dispatchDesc->MissShaderTable.StrideInBytes = dispatchDesc->MissShaderTable.SizeInBytes;
		dispatchDesc->RayGenerationShaderRecord.StartAddress = m_rayGenShaderTable->GetGPUVirtualAddress();
		dispatchDesc->RayGenerationShaderRecord.SizeInBytes = m_rayGenShaderTable->GetDesc().Width;
		dispatchDesc->Width = m_width;
		dispatchDesc->Height = m_height;
		dispatchDesc->Depth = 1;
		commandList->SetPipelineState1(stateObject);
		commandList->DispatchRays(dispatchDesc);
	};

	commandList->SetComputeRootSignature(m_rayTracingGlobalRootSignature.Get());

	// Bind the heaps, acceleration structure and dispatch rays.    
	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
	commandList->SetDescriptorHeaps(1, m_descriptorHeap.GetAddressOf());
	commandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot,
	                                           m_rayTracingOutputResourceUAVGPUDescriptor);
	commandList->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot,
	                                              m_topLevelAccelerationStructure->GetGPUVirtualAddress());
	DispatchRays(m_dxrCommandList.Get(), m_dxrStateObject.Get(), &dispatchDesc);
}

// Update the application state with the new resolution.
void DirectXBoxTutorial::UpdateForSizeChange(UINT width, UINT height) {
	Tutorial::UpdateForSizeChange(width, height);
	float border = 0.1f;
	if (m_width <= m_height) {
		m_rayGenCB.stencil =
		{
			-1 + border, -1 + border * m_aspectRatio,
			1.0f - border, 1 - border * m_aspectRatio
		};
	}
	else {
		m_rayGenCB.stencil =
		{
			-1 + border / m_aspectRatio, -1 + border,
			1 - border / m_aspectRatio, 1.0f - border
		};

	}
}

// Copy the raytracing output to the backbuffer.
void DirectXBoxTutorial::CopyRayTracingOutputToBackBuffer() {
	auto commandList = m_deviceResources->GetCommandList();
	auto renderTarget = m_deviceResources->GetRenderTarget();

	D3D12_RESOURCE_BARRIER preCopyBarriers[2];
	preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
	                                                          D3D12_RESOURCE_STATE_COPY_DEST);
	preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_rayTracingOutput.Get(),
	                                                          D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
	                                                          D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

	commandList->CopyResource(renderTarget.Get(), m_rayTracingOutput.Get());

	D3D12_RESOURCE_BARRIER postCopyBarriers[2];
	postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
	                                                           D3D12_RESOURCE_STATE_PRESENT);
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_rayTracingOutput.Get(),
	                                                           D3D12_RESOURCE_STATE_COPY_SOURCE,
	                                                           D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	commandList->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
}

// Create resources that are dependent on the size of the main window.
void DirectXBoxTutorial::CreateWindowSizeDependentResources() {
	CreateRayTracingOutputResource();

	// For simplicity, we will rebuild the shader tables.
	BuildShaderTables();
}

// Release resources that are dependent on the size of the main window.
void DirectXBoxTutorial::ReleaseWindowSizeDependentResources() {
	m_rayGenShaderTable.Reset();
	m_missShaderTable.Reset();
	m_hitGroupShaderTable.Reset();
	m_rayTracingOutput.Reset();
}

// Release all resources that depend on the device.
void DirectXBoxTutorial::ReleaseDeviceDependentResources() {
	m_rayTracingGlobalRootSignature.Reset();
	m_rayTracingLocalRootSignature.Reset();

	m_dxrDevice.Reset();
	m_dxrCommandList.Reset();
	m_dxrStateObject.Reset();

	m_descriptorHeap.Reset();
	m_descriptorsAllocated = 0;
	m_rayTracingOutputResourceUAVDescriptorHeapIndex = UINT_MAX;
	m_indexBuffer.Reset();
	m_vertexBuffer.Reset();

	m_accelerationStructure.Reset();
	m_bottomLevelAccelerationStructure.Reset();
	m_topLevelAccelerationStructure.Reset();
}

void DirectXBoxTutorial::ReCreateDirect3D() {
	// Give GPU a chance to finish its execution in progress.
	try {
		m_deviceResources->WaitForGPU();
	}
	catch (HrException&) {
		// Do nothing, currently attached adapter is unresponsive.
	}
	m_deviceResources->HandleDeviceLost();
}

// Render the scene.
void DirectXBoxTutorial::OnRender() {
	if (!m_deviceResources->IsWindowVisible()) {
		return;
	}

	m_deviceResources->Prepare();
	MakeRayTracing();
	CopyRayTracingOutputToBackBuffer();

	m_deviceResources->Present(D3D12_RESOURCE_STATE_PRESENT);
}


// Compute the average frames per second and million rays per second.
void DirectXBoxTutorial::CalculateFrameStats() {
	static int frameCnt = 0;
	static double elapsedTime = 0.0f;
	double totalTime = m_timer.GetTotalSeconds();
	frameCnt++;

	// Compute averages over one second period.
	if ((totalTime - elapsedTime) >= 1.0f) {
		float diff = static_cast<float>(totalTime - elapsedTime);
		float fps = static_cast<float>(frameCnt) / diff; // Normalize to an exact second.

		frameCnt = 0;
		elapsedTime = totalTime;

		float MRaysPerSecond = (m_width * m_height * fps) / static_cast<float>(1e6);

		std::wstringstream windowText;

		windowText << std::setprecision(2) << std::fixed
			<< L"    fps: " << fps << L"     ~Million Primary Rays/s: " << MRaysPerSecond
			<< L"    GPU[" << m_deviceResources->GetAdapterID() << L"]: " << m_deviceResources->GetAdapterDescription();
		SetCustomWindowText(windowText.str().c_str());
	}
}

// Handle OnSizeChanged message event.
void DirectXBoxTutorial::OnSizeChanged(UINT width, UINT height, bool minimized) {
	if (!m_deviceResources->WindowSizeChanged(width, height, minimized)) {
		return;
	}

	UpdateForSizeChange(width, height);

	ReleaseWindowSizeDependentResources();
	CreateWindowSizeDependentResources();
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

// Allocate a descriptor and return its index. 
// If the passed descriptorIndexToUse is valid, it will be used instead of allocating a new one.
UINT DirectXBoxTutorial::AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse) {
	auto descriptorHeapCpuBase = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	if (descriptorIndexToUse >= m_descriptorHeap->GetDesc().NumDescriptors) {
		descriptorIndexToUse = m_descriptorsAllocated++;
	}
	*cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuBase, descriptorIndexToUse, m_descriptorSize);
	return descriptorIndexToUse;
}
