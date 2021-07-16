#include "CEDXRayTracing.h"

#include "CEDXCommandContext.h"
#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingGeometry::CEDXRayTracingGeometry(CEDXDevice* device,
	uint32 flags): CERayTracingGeometry(flags), CEDXDeviceElement(device), VertexBuffer(nullptr), IndexBuffer(nullptr),
	               ResultBuffer(nullptr), ScratchBuffer(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingGeometry::Build(CEDXCommandContext& commandContext,
	bool update) {
	Assert(VertexBuffer != nullptr);

	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc;
	Memory::CEMemory::Memzero(&geometryDesc);

	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.VertexBuffer.StartAddress = VertexBuffer->GetResource()->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = VertexBuffer->GetStride();
	geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometryDesc.Triangles.VertexCount = VertexBuffer->GetNumVertices();
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	if (IndexBuffer) {
		CEIndexFormat IndexFormat = IndexBuffer->GetFormat();
		geometryDesc.Triangles.IndexFormat = IndexFormat == CEIndexFormat::uint32
			                                     ? DXGI_FORMAT_R32_UINT
			                                     : DXGI_FORMAT_R16_UINT;
		geometryDesc.Triangles.IndexBuffer = IndexBuffer->GetResource()->GetGPUVirtualAddress();
		geometryDesc.Triangles.IndexCount = IndexBuffer->GetNumIndices();
	}

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs;
	Memory::CEMemory::Memzero(&inputs);

	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &geometryDesc;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.Flags = ConvertAccelerationStructureBuildFlags(GetFlags());

	if (update) {
		inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
	}

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO preBuildInfo;
	Memory::CEMemory::Memzero(&preBuildInfo);
	GetDevice()->GetRayTracingAccelerationStructurePreBuildInfo(&inputs, &preBuildInfo);

	uint64 currentSize = ResultBuffer ? ResultBuffer->GetWidth() : 0;
	if (currentSize < preBuildInfo.ResultDataMaxSizeInBytes) {
		D3D12_RESOURCE_DESC desc;
		Memory::CEMemory::Memzero(&desc);

		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Width = preBuildInfo.ResultDataMaxSizeInBytes;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Alignment = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		CERef<CEDXResource> Buffer = new CEDXResource(GetDevice(), desc, D3D12_HEAP_TYPE_DEFAULT);
		if (!Buffer->Create(D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, nullptr)) {
			CEDebug::DebugBreak();
			return false;
		}

		ResultBuffer = Buffer;
	}

	uint64 requiredSize = Math::CEMath::Max(preBuildInfo.ScratchDataSizeInBytes,
	                                        preBuildInfo.UpdateScratchDataSizeInBytes);
	currentSize = ScratchBuffer ? ScratchBuffer->GetWidth() : 0;
	if (currentSize < requiredSize) {
		D3D12_RESOURCE_DESC desc;
		Memory::CEMemory::Memzero(&desc);

		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Width = requiredSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Alignment = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		CERef<CEDXResource> Buffer = new CEDXResource(GetDevice(), desc, D3D12_HEAP_TYPE_DEFAULT);
		if (!Buffer->Create(D3D12_RESOURCE_STATE_COMMON, nullptr)) {
			CEDebug::DebugBreak();
			return false;
		}

		ScratchBuffer = Buffer;

		commandContext.TransitionResource(ScratchBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
		                                  D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC accelerationStructureDesc;
	Memory::CEMemory::Memzero(&accelerationStructureDesc);

	accelerationStructureDesc.Inputs = inputs;
	accelerationStructureDesc.DestAccelerationStructureData = ResultBuffer->GetGPUVirtualAddress();
	accelerationStructureDesc.ScratchAccelerationStructureData = ScratchBuffer->GetGPUVirtualAddress();

	commandContext.FlushResourceBarriers();

	CEDXCommandListHandle& commandList = commandContext.GetCommandList();
	commandList.BuildRayTracingAccelerationStructure(&accelerationStructureDesc);

	commandContext.UnorderedAccessBarrier(ResultBuffer.Get());

	return true;
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXShaderBindingTableBuilder::CEDXShaderBindingTableBuilder(
	CEDXDevice* device): CEDXDeviceElement(device) {
	Reset();
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXShaderBindingTableBuilder::PopulateEntry(
	CEDXRayTracingPipelineState* pipelineState, CEDXRootSignature* rootSignature,
	CEDXOnlineDescriptorHeap* resourceHeap, CEDXOnlineDescriptorHeap* samplerHeap,
	CEDXShaderBindingTableEntry& shaderBindingEntry, const CERayTracingShaderResources& resources) {
	Assert(pipelineState != nullptr);
	Assert(rootSignature != nullptr);
	Assert(resourceHeap != nullptr);
	Assert(samplerHeap != nullptr);

	Memory::CEMemory::Memcpy(shaderBindingEntry.ShaderIdentifier,
	                         pipelineState->GetShaderIdentifier(resources.Identifier),
	                         D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

	if (!resources.ConstantBuffers.IsEmpty()) {
		uint32 rootIndex = rootSignature->GetRootParameterIndex(ShaderVisibility_All, ResourceType_CBV);
		Assert(rootIndex < 4);

		uint32 numDescriptors = resources.ConstantBuffers.Size();
		uint32 handle = resourceHeap->AllocateHandles(numDescriptors);
		shaderBindingEntry.RootDescriptorTables[rootIndex] = resourceHeap->GetGPUDescriptorHandleAt(handle);

		GPUResourceHandles[GPUResourceIndex] = resourceHeap->GetCPUDescriptorHandleAt(handle);
		GPUResourceHandleSizes[GPUResourceIndex++] = numDescriptors;

		for (CEConstantBuffer* cbuffer : resources.ConstantBuffers) {
			CEDXConstantBuffer* dxConstantBuffer = static_cast<CEDXConstantBuffer*>(cbuffer);
			ResourceHandles[CPUResourceIndex++] = dxConstantBuffer->GetView().GetOfflineHandle();
		}
	}

	if (!resources.ShaderResourceViews.IsEmpty()) {
		uint32 rootIndex = rootSignature->GetRootParameterIndex(ShaderVisibility_All, ResourceType_SRV);
		Assert(rootIndex < 4);

		uint32 numDescriptors = resources.ShaderResourceViews.Size();
		uint32 handle = resourceHeap->AllocateHandles(numDescriptors);
		shaderBindingEntry.RootDescriptorTables[rootIndex] = resourceHeap->GetGPUDescriptorHandleAt(handle);

		GPUResourceHandles[GPUResourceIndex] = resourceHeap->GetCPUDescriptorHandleAt(handle);
		GPUResourceHandleSizes[GPUResourceIndex++] = numDescriptors;

		for (CEShaderResourceView* shaderResourceView : resources.ShaderResourceViews) {
			CEDXShaderResourceView* dxShaderResourceView = static_cast<CEDXShaderResourceView*>(shaderResourceView);
			ResourceHandles[CPUResourceIndex++] = dxShaderResourceView->GetOfflineHandle();
		}
	}

	if (!resources.UnorderedAccessViews.IsEmpty()) {
		uint32 rootIndex = rootSignature->GetRootParameterIndex(ShaderVisibility_All, ResourceType_UAV);
		Assert(rootIndex < 4);

		uint32 numDescriptors = resources.UnorderedAccessViews.Size();
		uint32 handle = resourceHeap->AllocateHandles(numDescriptors);
		shaderBindingEntry.RootDescriptorTables[rootIndex] = resourceHeap->GetGPUDescriptorHandleAt(handle);

		GPUResourceHandles[GPUResourceIndex] = resourceHeap->GetCPUDescriptorHandleAt(handle);
		GPUResourceHandleSizes[GPUResourceIndex++] = numDescriptors;

		for (CEUnorderedAccessView* unorderedAccessView : resources.UnorderedAccessViews) {
			CEDXUnorderedAccessView* dxUnorderedAccessView = static_cast<CEDXUnorderedAccessView*>(unorderedAccessView);
			ResourceHandles[CPUResourceIndex++] = dxUnorderedAccessView->GetOfflineHandle();
		}
	}

	if (!resources.SamplerStates.IsEmpty()) {
		uint32 rootIndex = rootSignature->GetRootParameterIndex(ShaderVisibility_All, ResourceType_Sampler);
		Assert(rootIndex < 4);

		uint32 numDescriptors = resources.SamplerStates.Size();
		uint32 handle = samplerHeap->AllocateHandles(numDescriptors);
		shaderBindingEntry.RootDescriptorTables[rootIndex] = samplerHeap->GetGPUDescriptorHandleAt(handle);

		GPUSamplerHandles[GPUSamplerIndex] = samplerHeap->GetCPUDescriptorHandleAt(handle);
		GPUSamplerHandleSizes[GPUSamplerIndex++] = numDescriptors;

		for (CESamplerState* sampler : resources.SamplerStates) {
			CEDXSamplerState* dxSampler = static_cast<CEDXSamplerState*>(sampler);
			SamplerHandles[CPUSamplerIndex++] = dxSampler->GetOfflineHandle();
		}
	}
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXShaderBindingTableBuilder::CopyDescriptors() {
	GetDevice()->CopyDescriptors(
		GPUResourceIndex, GPUResourceHandles, GPUResourceHandleSizes,
		CPUResourceIndex, ResourceHandles, CPUHandleSizes,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);
	GetDevice()->CopyDescriptors(
		GPUSamplerIndex, GPUSamplerHandles, GPUSamplerHandleSizes,
		CPUSamplerIndex, SamplerHandles, CPUHandleSizes,
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
	);
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXShaderBindingTableBuilder::Reset() {
	for (uint32 i = 0; i < sizeof(CPUHandleSizes) / sizeof(CPUHandleSizes[0]); i++) {
		CPUHandleSizes[i] = 1;
	}

	CPUResourceIndex = 0;
	CPUSamplerIndex = 0;
	GPUResourceIndex = 0;
	GPUSamplerIndex = 0;
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::CEDXRayTracingScene(CEDXDevice* device,
	uint32 flags): CERayTracingScene(flags),
	               CEDXDeviceElement(device),
	               ResultBuffer(nullptr),
	               ScratchBuffer(nullptr),
	               InstanceBuffer(nullptr),
	               BindingTable(nullptr),
	               BindingTableStride(0),
	               NumHitGroups(0),
	               View(nullptr),
	               Instances(),
	               ShaderBindingTableBuilder(device) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::Build(CEDXCommandContext& commandContext,
	const CERayTracingGeometryInstance* instances, uint32 numInstances, bool update) {
	Assert(instances != nullptr && numInstances != 0);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs;
	Memory::CEMemory::Memzero(&inputs);

	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = numInstances;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.Flags = ConvertAccelerationStructureBuildFlags(GetFlags());
	if (update) {
		Assert(GetFlags() & RayTracingStructureBuildFlag_AllowUpdate);
		inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
	}

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO preBuildInfo;
	Memory::CEMemory::Memzero(&preBuildInfo);
	GetDevice()->GetRayTracingAccelerationStructurePreBuildInfo(&inputs, &preBuildInfo);

	uint64 currentSize = ResultBuffer ? ResultBuffer->GetWidth() : 0;
	if (currentSize < preBuildInfo.ResultDataMaxSizeInBytes) {
		D3D12_RESOURCE_DESC desc;
		Memory::CEMemory::Memzero(&desc);

		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Width = preBuildInfo.ResultDataMaxSizeInBytes;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Alignment = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		CERef<CEDXResource> Buffer = new CEDXResource(GetDevice(), desc, D3D12_HEAP_TYPE_DEFAULT);
		if (!Buffer->Create(D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, nullptr)) {
			CEDebug::DebugBreak();
			return false;
		}

		ResultBuffer = Buffer;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		Memory::CEMemory::Memzero(&srvDesc);

		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.RaytracingAccelerationStructure.Location = ResultBuffer->GetGPUVirtualAddress();

		View = new CEDXShaderResourceView(GetDevice(), CastDXManager()->GetResourceOfflineDescriptorHeap());
		if (!View->Create()) {
			return false;
		}

		if (!View->CreateView(nullptr, srvDesc)) {
			return false;
		}
	}

	uint64 requiredSize = Math::CEMath::Max(preBuildInfo.ScratchDataSizeInBytes,
	                                        preBuildInfo.UpdateScratchDataSizeInBytes);
	currentSize = ScratchBuffer ? ScratchBuffer->GetWidth() : 0;
	if (currentSize < requiredSize) {
		D3D12_RESOURCE_DESC desc;
		Memory::CEMemory::Memzero(&desc);

		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Width = requiredSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Alignment = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		CERef<CEDXResource> Buffer = new CEDXResource(GetDevice(), desc, D3D12_HEAP_TYPE_DEFAULT);
		if (!Buffer->Create(D3D12_RESOURCE_STATE_COMMON, nullptr)) {
			CEDebug::DebugBreak();
			return false;
		}

		ScratchBuffer = Buffer;

		commandContext.TransitionResource(ScratchBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
		                                  D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	CEArray<D3D12_RAYTRACING_INSTANCE_DESC> InstanceDescs(numInstances);
	for (uint32 i = 0; i < InstanceDescs.Size(); i++) {
		CEDXRayTracingGeometry* dxGeometry = static_cast<CEDXRayTracingGeometry*>(instances[i].Instance.Get());
		Memory::CEMemory::Memcpy(&InstanceDescs[i].Transform, &instances[i].Transform.Native,
		                         sizeof(DirectX::XMFLOAT3X4));

		InstanceDescs[i].AccelerationStructure = dxGeometry->GetGPUVirtualAddress();
		InstanceDescs[i].InstanceID = instances[i].InstanceIndex;
		InstanceDescs[i].Flags = ConvertRayTracingInstanceFlags(instances[i].Flags);
		InstanceDescs[i].InstanceMask = instances[i].Mask;
		InstanceDescs[i].InstanceContributionToHitGroupIndex = instances[i].HitGroupIndex;
	}

	currentSize = InstanceBuffer ? InstanceBuffer->GetWidth() : 0;
	if (currentSize < InstanceDescs.SizeInBytes()) {
		D3D12_RESOURCE_DESC desc;
		Memory::CEMemory::Memzero(&desc);

		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Width = InstanceDescs.SizeInBytes();
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Alignment = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		CERef<CEDXResource> Buffer = new CEDXResource(GetDevice(), desc, D3D12_HEAP_TYPE_DEFAULT);
		if (!Buffer->Create(D3D12_RESOURCE_STATE_COMMON, nullptr)) {
			CEDebug::DebugBreak();
			return false;
		}

		InstanceBuffer = Buffer;

		commandContext.TransitionResource(InstanceBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
		                                  D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

	commandContext.TransitionResource(InstanceBuffer.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
	                                  D3D12_RESOURCE_STATE_COPY_DEST);
	commandContext.UpdateBuffer(InstanceBuffer.Get(), 0, InstanceDescs.SizeInBytes(), InstanceDescs.Data());
	commandContext.TransitionResource(InstanceBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
	                                  D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC accelerationStructureDesc;
	Memory::CEMemory::Memzero(&accelerationStructureDesc);

	accelerationStructureDesc.Inputs = inputs;
	accelerationStructureDesc.Inputs.InstanceDescs = InstanceBuffer->GetGPUVirtualAddress();
	accelerationStructureDesc.DestAccelerationStructureData = ResultBuffer->GetGPUVirtualAddress();
	accelerationStructureDesc.ScratchAccelerationStructureData = ScratchBuffer->GetGPUVirtualAddress();
	if (update) {
		Assert(GetFlags() & RayTracingStructureBuildFlag_AllowUpdate);
		accelerationStructureDesc.SourceAccelerationStructureData = ResultBuffer->GetGPUVirtualAddress();
	}

	commandContext.FlushResourceBarriers();

	CEDXCommandListHandle& commandList = commandContext.GetCommandList();
	commandList.BuildRayTracingAccelerationStructure(&accelerationStructureDesc);

	commandContext.UnorderedAccessBarrier(ResultBuffer.Get());

	Instances = CEArray<CERayTracingGeometryInstance>(instances, instances + numInstances);

	return true;
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::BuildBindingTable(
	CEDXCommandContext& commandContext, CEDXRayTracingPipelineState* pipelineState,
	CEDXOnlineDescriptorHeap* resourceHeap, CEDXOnlineDescriptorHeap* samplerHeap,
	const CERayTracingShaderResources* rayGenLocalResources, const CERayTracingShaderResources* missLocalResources,
	const CERayTracingShaderResources* hitGroupResources, uint32 numHitGroupResources) {

	Assert(resourceHeap != nullptr);
	Assert(samplerHeap != nullptr);
	Assert(pipelineState != nullptr);
	Assert(rayGenLocalResources != nullptr);

	CEDXShaderBindingTableEntry rayGenEntry;
	ShaderBindingTableBuilder.PopulateEntry(pipelineState, pipelineState->GetRayGenLocalRootSignature(), resourceHeap,
	                                        samplerHeap, rayGenEntry, *rayGenLocalResources);

	Assert(missLocalResources != nullptr);

	CEDXShaderBindingTableEntry missEntry;
	ShaderBindingTableBuilder.PopulateEntry(
		pipelineState,
		pipelineState->GetMissLocalRootSignature(),
		resourceHeap,
		samplerHeap,
		missEntry,
		*missLocalResources
	);

	Assert(hitGroupResources != nullptr);
	Assert(numHitGroupResources <= D3D12_MAX_HIT_GROUPS);

	CEDXShaderBindingTableEntry hitGroupEntries[D3D12_MAX_HIT_GROUPS];
	for (uint32 i = 0; i < numHitGroupResources; i++) {
		ShaderBindingTableBuilder.PopulateEntry(
			pipelineState,
			pipelineState->GetHitLocalRootSignature(),
			resourceHeap,
			samplerHeap,
			hitGroupEntries[i],
			hitGroupResources[i]
		);
	}

	ShaderBindingTableBuilder.CopyDescriptors();

	uint32 tableEntrySize = sizeof(CEDXShaderBindingTableEntry);
	uint64 bindingTableSize = tableEntrySize + tableEntrySize + (tableEntrySize * numHitGroupResources);

	uint64 currentSize = BindingTable ? BindingTable->GetWidth() : 0;
	if (currentSize < bindingTableSize) {
		D3D12_RESOURCE_DESC desc;
		Memory::CEMemory::Memzero(&desc);

		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Width = bindingTableSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Alignment = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		CERef<CEDXResource> Buffer = new CEDXResource(GetDevice(), desc, D3D12_HEAP_TYPE_DEFAULT);
		if (!Buffer->Create(D3D12_RESOURCE_STATE_COMMON, nullptr)) {
			CEDebug::DebugBreak();
			return false;
		}

		BindingTable = Buffer;
		BindingTable->SetName(GetName() + "BindingTable");

		commandContext.TransitionResource(BindingTable.Get(), D3D12_RESOURCE_STATE_COMMON,
		                                  D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

	commandContext.TransitionResource(BindingTable.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
	                                  D3D12_RESOURCE_STATE_COPY_DEST);
	commandContext.UpdateBuffer(BindingTable.Get(), 0, tableEntrySize, &rayGenEntry);
	commandContext.UpdateBuffer(BindingTable.Get(), tableEntrySize, tableEntrySize, &missEntry);
	commandContext.UpdateBuffer(BindingTable.Get(), tableEntrySize * 2, numHitGroupResources * tableEntrySize,
	                            hitGroupEntries);
	commandContext.TransitionResource(BindingTable.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
	                                  D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	ShaderBindingTableBuilder.Reset();
	BindingTableHeaps[0] = resourceHeap->GetNativeHeap();
	BindingTableHeaps[1] = samplerHeap->GetNativeHeap();

	BindingTableStride = sizeof(CEDXShaderBindingTableEntry);
	NumHitGroups = numHitGroupResources;

	return true;
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::SetName(const std::string& name) {
	CEResource::SetName(name);
	ResultBuffer->SetName(name);
	if (ScratchBuffer) {
		ScratchBuffer->SetName(name + "Scratch");
	}
	if (InstanceBuffer) {
		InstanceBuffer->SetName(name + "Instance");
	}
	if (BindingTable) {
		BindingTable->SetName(name + "BindingTable");
	}
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::
GetRayGenShaderRecord() const {
	Assert(BindingTable != nullptr);
	Assert(BindingTableStride != 0);

	const uint64 bindingTableAddress = BindingTable->GetGPUVirtualAddress();
	return {bindingTableAddress, BindingTableStride};
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::
GetMissShaderTable() const {
	Assert(BindingTable != nullptr);
	Assert(BindingTableStride != 0);

	uint64 bindingTableAddress = BindingTable->GetGPUVirtualAddress();
	uint64 addressOffset = BindingTableStride;
	return {bindingTableAddress + addressOffset, BindingTableStride, BindingTableStride};
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::
GetHitGroupTable() const {
	Assert(BindingTable != nullptr);
	Assert(BindingTableStride != 0);

	uint64 bindingTableAddress = BindingTable->GetGPUVirtualAddress();
	uint64 addressOffset = BindingTableStride * 2;
	uint64 sizeInBytes = BindingTableStride * NumHitGroups;

	return {bindingTableAddress + addressOffset, sizeInBytes, BindingTableStride};
}
