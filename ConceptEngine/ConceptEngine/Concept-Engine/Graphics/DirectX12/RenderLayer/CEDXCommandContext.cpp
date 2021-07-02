#include "CEDXCommandContext.h"

#include "CEDXBuffer.h"
#include "CEDXRayTracing.h"
#include "CEDXSamplerState.h"
#include "CEDXShaderCompiler.h"
#include "CEDXTexture.h"

#include "../../../Core/Debug/CEDebug.h"

#include "../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

CEDXGPUResourceUploader::CEDXGPUResourceUploader(CEDXDevice* device): CEDXDeviceElement(device),
                                                                      MappedMemory(nullptr),
                                                                      SizeInBytes(0),
                                                                      OffsetInBytes(0),
                                                                      Resource(nullptr),
                                                                      GarbageResources() {
}

CEDXGPUResourceUploader::~CEDXGPUResourceUploader() {
}

bool CEDXGPUResourceUploader::Reserve(uint32 sizeInBytes) {
	if (sizeInBytes == SizeInBytes) {
		return true;
	}

	if (Resource) {
		Resource->Unmap(0, nullptr);
		GarbageResources.EmplaceBack(Resource);
		Resource.Reset();
	}

	D3D12_HEAP_PROPERTIES heapProperties;
	Memory::CEMemory::Memzero(&heapProperties);

	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags = D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Width = sizeInBytes;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	HRESULT hResult = GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
	                                                       D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
	                                                       IID_PPV_ARGS(&Resource));

	if (SUCCEEDED(hResult)) {
		Resource->SetName(L"[CEDXGpuResourceUploader] Buffer");
		Resource->Map(0, nullptr, reinterpret_cast<void**>(&MappedMemory));

		SizeInBytes = sizeInBytes;
		OffsetInBytes = 0;
		return true;
	}

	return false;
}

void CEDXGPUResourceUploader::Reset() {
	constexpr uint32 MAX_RESERVED_GARBAGE_RESOURCES = 5;
	constexpr uint32 NEW_RESERVED_GARBAGE_RESOURCES = 2;

	GarbageResources.Clear();
	if (GarbageResources.Capacity() >= MAX_RESERVED_GARBAGE_RESOURCES) {
		GarbageResources.Reserve(NEW_RESERVED_GARBAGE_RESOURCES);
	}

	OffsetInBytes = 0;
}

CEDXUploadAllocation CEDXGPUResourceUploader::LinearAllocate(uint32 sizeInBytes) {
	constexpr uint32 EXTRA_BYTES_ALLOCATED = 1024;

	const uint32 requiredSize = OffsetInBytes + sizeInBytes;
	if (requiredSize > SizeInBytes) {
		Reserve(requiredSize + EXTRA_BYTES_ALLOCATED);
	}

	CEDXUploadAllocation allocation;
	allocation.MappedPtr = MappedMemory + OffsetInBytes;
	allocation.ResourceOffset = OffsetInBytes;
	OffsetInBytes += sizeInBytes;
	return allocation;
}

CEDXCommandBatch::CEDXCommandBatch(CEDXDevice* device): Device(device), CommandAllocator(device),
                                                        GPUResourceUploader(device),
                                                        OnlineResourceDescriptorHeap(nullptr),
                                                        OnlineSamplerDescriptorHeap(nullptr), Resources() {
}

bool CEDXCommandBatch::Create() {
	if (!CommandAllocator.Create(D3D12_COMMAND_LIST_TYPE_DIRECT)) {
		CEDebug::DebugBreak();
		return false;
	}

	OnlineResourceDescriptorHeap = new CEDXOnlineDescriptorHeap(Device, D3D12_DEFAULT_ONLINE_RESOURCE_DESCRIPTOR_COUNT,
	                                                            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (!OnlineResourceDescriptorHeap->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	OnlineSamplerDescriptorHeap = new CEDXOnlineDescriptorHeap(Device, D3D12_DEFAULT_ONLINE_SAMPLER_DESCRIPTOR_COUNT,
	                                                           D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	if (!OnlineSamplerDescriptorHeap->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	OnlineRayTracingResourceDescriptorHeap = new CEDXOnlineDescriptorHeap(
		Device, D3D12_DEFAULT_ONLINE_RESOURCE_DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (!OnlineRayTracingResourceDescriptorHeap->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	OnlineRayTracingSamplerDescriptorHeap = new CEDXOnlineDescriptorHeap(
		Device, D3D12_DEFAULT_ONLINE_SAMPLER_DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	if (!OnlineRayTracingSamplerDescriptorHeap->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	GPUResourceUploader.Reserve(1024);
	return true;
}

void CEDXResourceBarrierBatcher::AddTransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState,
                                                      D3D12_RESOURCE_STATES afterState) {
	Assert(resource != nullptr);

	if (beforeState != afterState) {
		for (CEArray<D3D12_RESOURCE_BARRIER>::Iterator it = Barriers.Begin(); it != Barriers.End(); it++) {
			if ((*it).Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
				if ((*it).Transition.pResource == resource) {
					if ((*it).Transition.StateBefore == afterState) {
						it = Barriers.Erase(it);
					}
					else {
						(*it).Transition.StateAfter = afterState;
					}
					return;
				}
			}
		}
	}

	D3D12_RESOURCE_BARRIER barrier;
	Memory::CEMemory::Memzero(&barrier);

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = resource;
	barrier.Transition.StateAfter = afterState;
	barrier.Transition.StateBefore = beforeState;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	Barriers.EmplaceBack(barrier);
}

CEDXCommandContext::CEDXCommandContext(CEDXDevice* device): CEICommandContext(), CEDXDeviceElement(device),
                                                            CommandQueue(device), CommandList(device), Fence(device),
                                                            DescriptorCache(device), CommandBatches(),
                                                            BarrierBatcher() {
}

CEDXCommandContext::~CEDXCommandContext() {
	Flush();
}

bool CEDXCommandContext::Create() {
	if (!CommandQueue.Create(D3D12_COMMAND_LIST_TYPE_DIRECT)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to Create CommandQueue of type : D3D12_COMMAND_LIST_TYPE_DIRECT");
		return false;
	}

	for (uint32 i = 0; i < 3; i++) {
		CEDXCommandBatch& batch = CommandBatches.EmplaceBack(GetDevice());
		if (!batch.Create()) {
			CE_LOG_ERROR("[CEDXCommandContext]: Failed to Create CommandBatch of index: " + std::to_string(i));
			return false;
		}
	}

	if (!CommandList.Create(D3D12_COMMAND_LIST_TYPE_DIRECT, CommandBatches[0].GetCommandAllocator(), nullptr)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to Create CommandList of type: D3D12_COMMAND_LIST_TYPE_DIRECT");
		return false;
	}

	FenceValue = 0;
	if (!Fence.Create(FenceValue)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to Create Fence");
		return false;
	}

	if (!DescriptorCache.Create()) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to Create DescriptorCache");
		return false;
	}

	CEArray<uint8> code;
	if (!ShaderCompiler->
		CompileFromFile(GetGraphicsContentDirectory("DirectX12\\Shaders\\GenerateMipsTex2D.hlsl"), "Main", nullptr,
		                CEShaderStage::Compute,
		                CEShaderModel::SM_6_0, code)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to compile GenerateMipsTex2D shader");
		return false;
	}

	CERef<CEDXComputeShader> shader = new CEDXComputeShader(GetDevice(), code);
	if (!shader->Create()) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to Create CEDXComputeShader for GenerateMipsTex2D");
		return false;
	}

	GenerateMipsTex2D_PSO = new CEDXComputePipelineState(GetDevice(), shader);
	if (!GenerateMipsTex2D_PSO->Create()) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to create GenerateMipsTex2D PipelineState");
		return false;
	}

	GenerateMipsTex2D_PSO->SetName("GenerateMipsTex2D Gen PSO");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12\\Shaders\\GenerateMipsTexCube.hlsl"),
	                                     "Main", nullptr,
	                                     CEShaderStage::Compute, CEShaderModel::SM_6_0, code)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to compile GenerateMipsTexCube Shader");
		return false;
	}

	shader = new CEDXComputeShader(GetDevice(), code);
	if (!shader->Create()) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to Create CEDXComputeShader for GenerateMipsTexCube");
		return false;
	}

	GenerateMipsTexCube_PSO = new CEDXComputePipelineState(GetDevice(), shader);
	if (!GenerateMipsTexCube_PSO->Create()) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to create GenerateMipsTexCube Pipeline");
		return false;
	}

	GenerateMipsTexCube_PSO->SetName("GenerateMipsTexCube Gen PSO");

	return true;
}

void CEDXCommandContext::UpdateBuffer(CEDXResource* resource, uint64 offsetInBytes, uint64 sizeInBytes,
                                      const void* sourceData) {
	if (sizeInBytes != 0) {
		FlushResourceBarriers();

		CEDXGPUResourceUploader& gpuResourceUploader = CommandBatch->GetGpuResourceUploader();

		CEDXUploadAllocation allocation = gpuResourceUploader.LinearAllocate((uint32)sizeInBytes);
		Memory::CEMemory::Memcpy(allocation.MappedPtr, sourceData, sizeInBytes);

		CommandList.CopyBufferRegion(resource->GetResource(),
		                             offsetInBytes,
		                             gpuResourceUploader.GetGPUResource(),
		                             allocation.ResourceOffset,
		                             sizeInBytes);
		CommandBatch->AddInUseResource(resource);
	}
}

void CEDXCommandContext::Begin() {
	Assert(IsReady == false);

	CommandBatch = &CommandBatches[NextCommandBatch];
	NextCommandBatch = (NextCommandBatch + 1) % CommandBatches.Size();

	if (FenceValue >= CommandBatches.Size()) {
		const uint64 WaitValue = Math::CEMath::Max<uint64>(FenceValue - (CommandBatches.Size() - 1), 0);
		Fence.WaitForValue(WaitValue);
	}

	if (!CommandBatch->Reset()) {
		CE_LOG_ERROR("[CEDXCommandContext] Failed to Reset CommandBatch");
		return;
	}

	InternalClearState();

	if (!CommandList.Reset(CommandBatch->GetCommandAllocator())) {
		CE_LOG_ERROR("[CEDXCommandContext] Failed to Reset GetCommandAllocator");
		return;
	}

	IsReady = true;
}

void CEDXCommandContext::End() {
	Assert(IsReady == true);

	FlushResourceBarriers();

	CommandBatch = nullptr;
	IsReady = false;

	const uint64 newFenceValue = ++FenceValue;
	for (uint32 i = 0; i < ResolveProfilers.Size(); i++) {
		ResolveProfilers[i]->ResolveQueries(*this);
	}
	ResolveProfilers.Clear();

	if (!CommandList.Close()) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to Close CommandList ")
		return;
	}

	CommandQueue.ExecuteCommandList(&CommandList);

	if (!CommandQueue.SignalFence(Fence, newFenceValue)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to SignalFence for value " + std::to_string(newFenceValue))
		return;
	}

}

void CEDXCommandContext::BeginTimeStamp(CEGPUProfiler* profiler, uint32 index) {
	ID3D12GraphicsCommandList* commandList = CommandList.GetGraphicsCommandList();
	CEDXGPUProfiler* dxProfiler = static_cast<CEDXGPUProfiler*>(profiler);
	Assert(profiler);
	dxProfiler->BeginQuery(commandList, index);
	ResolveProfilers.EmplaceBack(MakeSharedRef<CEDXGPUProfiler>(dxProfiler));
}

void CEDXCommandContext::EndTimeStamp(CEGPUProfiler* profiler, uint32 index) {
	Assert(profiler);
	ID3D12GraphicsCommandList* commandList = CommandList.GetGraphicsCommandList();
	CEDXGPUProfiler* dxProfiler = static_cast<CEDXGPUProfiler*>(profiler);
	Assert(profiler);
	dxProfiler->EndQuery(commandList, index);
}

void CEDXCommandContext::DispatchRays(CERayTracingScene* rayTracingScene, CERayTracingPipelineState* pipelineState,
                                      uint32 width, uint32 height, uint32 depth) {
	CEDXRayTracingScene* dxrScene = static_cast<CEDXRayTracingScene*>(rayTracingScene);
	CEDXRayTracingPipelineState* dxrPipelineState = static_cast<CEDXRayTracingPipelineState*>(pipelineState);
	Assert(dxrScene != nullptr);
	Assert(dxrPipelineState != nullptr);

	ID3D12GraphicsCommandList4* dxrCommandList = CommandList.GetDXRCommandList();

	FlushResourceBarriers();

	D3D12_DISPATCH_RAYS_DESC rayDispatchDesc;
	Memory::CEMemory::Memzero(&rayDispatchDesc);

	rayDispatchDesc.RayGenerationShaderRecord = dxrScene->GetRayGenShaderRecord();
	rayDispatchDesc.MissShaderTable = dxrScene->GetMissShaderTable();
	rayDispatchDesc.HitGroupTable = dxrScene->GetHitGroupTable();

	rayDispatchDesc.Width = width;
	rayDispatchDesc.Height = height;
	rayDispatchDesc.Depth = depth;

	dxrCommandList->SetPipelineState1(dxrPipelineState->GetStateObject());
	dxrCommandList->DispatchRays(&rayDispatchDesc);
}

void CEDXCommandContext::SetRayTracingBindings(CERayTracingScene* rayTracingScene,
                                               CERayTracingPipelineState* pipelineState,
                                               const CERayTracingShaderResources* globalResource,
                                               const CERayTracingShaderResources* rayGenLocalResources,
                                               const CERayTracingShaderResources* missLocalResources,
                                               const CERayTracingShaderResources* hitGroupResources,
                                               uint32 numHitGroupResources) {
	CEDXRayTracingScene* dxrScene = static_cast<CEDXRayTracingScene*>(rayTracingScene);
	CEDXRayTracingPipelineState* dxrPipelineState = static_cast<CEDXRayTracingPipelineState*>(pipelineState);

	Assert(dxrScene != nullptr);
	Assert(dxrPipelineState != nullptr);

	uint32 numDescriptorsNeeded = 0;
	uint32 numSamplersNeeded = 0;
	if (globalResource) {
		numDescriptorsNeeded += globalResource->NumResources();
		numSamplersNeeded += globalResource->NumSamplers();
	}
	if (rayGenLocalResources) {
		numDescriptorsNeeded += rayGenLocalResources->NumResources();
		numSamplersNeeded += rayGenLocalResources->NumSamplers();
	}
	if (missLocalResources) {
		numDescriptorsNeeded += missLocalResources->NumResources();
		numSamplersNeeded += missLocalResources->NumSamplers();
	}
	for (uint32 i = 0; i < numHitGroupResources; i++) {
		numDescriptorsNeeded += hitGroupResources[i].NumResources();
		numSamplersNeeded += hitGroupResources[i].NumSamplers();
	}

	Assert(numDescriptorsNeeded < D3D12_MAX_ONLINE_DESCRIPTOR_COUNT);
	CEDXOnlineDescriptorHeap* resourceHeap = CommandBatch->GetOnlineResourceDescriptorHeap();
	if (!resourceHeap->HasSpace(numDescriptorsNeeded)) {
		resourceHeap->AllocateFreshHeap();
	}

	Assert(numSamplersNeeded < D3D12_MAX_ONLINE_DESCRIPTOR_COUNT);
	CEDXOnlineDescriptorHeap* samplerHeap = CommandBatch->GetOnlineSamplerDescriptorHeap();
	if (!samplerHeap->HasSpace(numSamplersNeeded)) {
		samplerHeap->AllocateFreshHeap();
	}

	if (!dxrScene->BuildBindingTable(*this,
	                                 dxrPipelineState,
	                                 resourceHeap,
	                                 samplerHeap,
	                                 rayGenLocalResources,
	                                 missLocalResources,
	                                 hitGroupResources,
	                                 numHitGroupResources)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to build shader binding table");
	}

	Assert(globalResource != nullptr);

	if (!globalResource->ConstantBuffers.IsEmpty()) {
		for (uint32 i = 0; i < globalResource->ConstantBuffers.Size(); i++) {
			CEDXConstantBufferView& dxConstantBufferView = static_cast<CEDXConstantBuffer*>(globalResource->
				ConstantBuffers[i])->GetView();
			DescriptorCache.SetConstantBufferView(&dxConstantBufferView, ShaderVisibility_All, i);
		}
	}

	if (!globalResource->ShaderResourceViews.IsEmpty()) {
		for (uint32 i = 0; i < globalResource->ShaderResourceViews.Size(); i++) {
			CEDXShaderResourceView* dxShaderResourceView = static_cast<CEDXShaderResourceView*>(globalResource->
				ShaderResourceViews[i]);
			DescriptorCache.SetShaderResourceView(dxShaderResourceView, ShaderVisibility_All, i);
		}
	}

	if (!globalResource->UnorderedAccessViews.IsEmpty()) {
		for (uint32 i = 0; i < globalResource->UnorderedAccessViews.Size(); i++) {
			CEDXUnorderedAccessView* dxUnorderedAccessView = static_cast<CEDXUnorderedAccessView*>(globalResource->
				UnorderedAccessViews[i]);
			DescriptorCache.SetUnorderedAccessView(dxUnorderedAccessView, ShaderVisibility_All, i);
		}
	}

	if (!globalResource->SamplerStates.IsEmpty()) {
		for (uint32 i = 0; i < globalResource->SamplerStates.Size(); i++) {
			CEDXSamplerState* dxSampler = static_cast<CEDXSamplerState*>(globalResource->SamplerStates[i]);
			DescriptorCache.SetSamplerState(dxSampler, ShaderVisibility_All, i);
		}
	}

	ID3D12GraphicsCommandList4* dxrCommandList = CommandList.GetDXRCommandList();

	CEDXRootSignature* globalRootSignature = dxrPipelineState->GetGlobalRootSignature();
	CurrentRootSignature = MakeSharedRef<CEDXRootSignature>(globalRootSignature);
	dxrCommandList->SetComputeRootSignature(CurrentRootSignature->GetRootSignature());

	DescriptorCache.CommitComputeDescriptors(CommandList, CommandBatch, CurrentRootSignature.Get());

}

void CEDXCommandContext::InsertMaker(const std::string& message) {
	if (Base::CEDirectXHandler::CESetMarkerOnCommandList) {
		Base::CEDirectXHandler::CESetMarkerOnCommandList(CommandList.GetGraphicsCommandList(), PIX_COLOR(255, 255, 255),
		                                                 message.c_str());
	}
}

void CEDXCommandContext::ClearRenderTargetView(CERenderTargetView* renderTargetView, Math::CEColorF& clearColor) {
	Assert(renderTargetView != nullptr);
	FlushResourceBarriers();

	CEDXRenderTargetView* dxRenderTargetView = static_cast<CEDXRenderTargetView*>(renderTargetView);
	CommandBatch->AddInUseResource(dxRenderTargetView);

	CommandList.ClearRenderTargetView(dxRenderTargetView->GetOfflineHandle(), clearColor.Elements, 0, nullptr);
}

void CEDXCommandContext::ClearDepthStencilView(CEDepthStencilView* depthStencilView,
                                               const CEDepthStencilF& clearColor) {
	Assert(depthStencilView != nullptr);
	FlushResourceBarriers();

	CEDXDepthStencilView* dxDepthStencilView = static_cast<CEDXDepthStencilView*>(depthStencilView);
	CommandBatch->AddInUseResource(dxDepthStencilView);

	CommandList.ClearDepthStencilView(dxDepthStencilView->GetOfflineHandle(),
	                                  D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearColor.Depth,
	                                  clearColor.Stencil);
}

void CEDXCommandContext::ClearUnorderedAccessViewFloat(CEUnorderedAccessView* unorderedAccessView,
                                                       const Math::CEColorF& clearColor) {
	FlushResourceBarriers();

	CEDXUnorderedAccessView* dxUnorderedAccessView = static_cast<CEDXUnorderedAccessView*>(unorderedAccessView);
	CommandBatch->AddInUseResource(dxUnorderedAccessView);

	CEDXOnlineDescriptorHeap* onlineDescriptorHeap = CommandBatch->GetOnlineResourceDescriptorHeap();
	const uint32 onlineDescriptorHandleIndex = onlineDescriptorHeap->AllocateHandles(1);

	const D3D12_CPU_DESCRIPTOR_HANDLE offlineHandle = dxUnorderedAccessView->GetOfflineHandle();
	const D3D12_CPU_DESCRIPTOR_HANDLE onlineHandleCPU = onlineDescriptorHeap->GetCPUDescriptorHandleAt(
		onlineDescriptorHandleIndex);
	GetDevice()->CopyDescriptorsSimple(1, onlineHandleCPU, offlineHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	const D3D12_GPU_DESCRIPTOR_HANDLE onlineHandleGPU = onlineDescriptorHeap->GetGPUDescriptorHandleAt(
		onlineDescriptorHandleIndex);

	CommandList.ClearUnorderedAccessViewFloat(onlineHandleGPU, dxUnorderedAccessView, clearColor.Elements);
}

void CEDXCommandContext::SetShadingRate(CEShadingRate shadingRate) {
	D3D12_SHADING_RATE dxShadingRate = ConvertShadingRate(shadingRate);

	D3D12_SHADING_RATE_COMBINER combiners[] = {
		D3D12_SHADING_RATE_COMBINER_OVERRIDE,
		D3D12_SHADING_RATE_COMBINER_OVERRIDE,
	};

	CommandList.SetShadingRate(dxShadingRate, combiners);
}

void CEDXCommandContext::SetShadingRateImage(CETexture2D* shadingImage) {
	FlushResourceBarriers();
	if (shadingImage) {
		CEDXBaseTexture* dxTexture = TextureCast(shadingImage);
		CommandList.SetShadingRateImage(dxTexture->GetResource()->GetResource());
		CommandBatch->AddInUseResource(shadingImage);
	}
	else {
		CommandList.SetShadingRateImage(nullptr);
	}
}

void CEDXCommandContext::SetViewport(float width, float height, float minDepth, float maxDepth, float x, float y) {
	D3D12_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = minDepth;
	viewport.MaxDepth = maxDepth;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;

	CommandList.SetViewports(&viewport, 1);
}

void CEDXCommandContext::SetScissorRect(float width, float height, float x, float y) {
	D3D12_RECT scissorRect;
	scissorRect.top = LONG(y);
	scissorRect.bottom = LONG(height);
	scissorRect.left = LONG(x);
	scissorRect.right = LONG(y);

	CommandList.SetScissorRects(&scissorRect, 1);
}

void CEDXCommandContext::SetBlendFactor(const Math::CEColorF& color) {
	CommandList.SetBlendFactor(color.Elements);
}

void CEDXCommandContext::BeginRenderPass(const Math::CEColorF& color, CERenderTargetView* renderTargetView,
                                         CEDepthStencilView* depthStencilView) {
	const D3D12_CLEAR_VALUE optimizedClearValue = {DXGI_FORMAT_R32G32B32_FLOAT, {color.r, color.g, color.b, color.a}};

	CEDXRenderTargetView* dxRenderTargetView = static_cast<CEDXRenderTargetView*>(renderTargetView);
	CEDXDepthStencilView* dxDepthStencilView = static_cast<CEDXDepthStencilView*>(depthStencilView);

	D3D12_RENDER_PASS_BEGINNING_ACCESS renderPassBeginningAccessClear{
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR,
		{optimizedClearValue}
	};
	D3D12_RENDER_PASS_ENDING_ACCESS renderPassEndingAccessPreserve{
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}
	};
	D3D12_RENDER_PASS_RENDER_TARGET_DESC renderPassRenderTargetDesc{
		dxRenderTargetView->GetOfflineHandle(), renderPassBeginningAccessClear, renderPassEndingAccessPreserve
	};

	D3D12_RENDER_PASS_BEGINNING_ACCESS renderPassBeginningAccessNoAccess{
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS, {}
	};
	D3D12_RENDER_PASS_ENDING_ACCESS renderPassEndingAccessNoAccess{
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS, {}
	};
	D3D12_RENDER_PASS_DEPTH_STENCIL_DESC renderPassDepthStencilDesc{
		dxDepthStencilView->GetOfflineHandle(),
		renderPassBeginningAccessNoAccess,
		renderPassBeginningAccessNoAccess,
		renderPassEndingAccessNoAccess,
		renderPassEndingAccessNoAccess
	};

	CommandList.BeginRenderPass(renderPassRenderTargetDesc, renderPassDepthStencilDesc, 1);
}

void CEDXCommandContext::EndRenderPass() {
	CommandList.EndRenderPass();
}

void CEDXCommandContext::SetPrimitiveTopology(CEPrimitiveTopology primitiveTopologyType) {
	const D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = ConvertPrimitiveTopology(primitiveTopologyType);
	CommandList.SetPrimitiveTopology(primitiveTopology);
}

void CEDXCommandContext::SetVertexBuffers(CEVertexBuffer* const* vertexBuffers, uint32 bufferCount, uint32 bufferSlot) {
	Assert(bufferSlot + bufferCount < D3D12_MAX_VERTEX_BUFFER_SLOTS);

	for (uint32 i = 0; i < bufferCount; i++) {
		uint32 slot = bufferSlot + 1;
		CEDXVertexBuffer* dxVertexBuffer = static_cast<CEDXVertexBuffer*>(vertexBuffers[i]);
		DescriptorCache.SetVertexBuffer(dxVertexBuffer, slot);
		CommandBatch->AddInUseResource(dxVertexBuffer);
	}

}

void CEDXCommandContext::SetIndexBuffer(CEIndexBuffer* indexBuffer) {
	CEDXIndexBuffer* dxIndexBuffer = static_cast<CEDXIndexBuffer*>(indexBuffer);
	DescriptorCache.SetIndexBuffer(dxIndexBuffer);
	CommandBatch->AddInUseResource(dxIndexBuffer);
}

void CEDXCommandContext::SetRenderTargets(CERenderTargetView* const* renderTargetViews,
                                          uint32 renderTargetCount,
                                          CEDepthStencilView* depthStencilView) {
	for (uint32 i = 0; i < renderTargetCount; i++) {
		CEDXRenderTargetView* dxRenderTargetView = static_cast<CEDXRenderTargetView*>(renderTargetViews[i]);
		DescriptorCache.SetRenderTargetView(dxRenderTargetView, i);
		CommandBatch->AddInUseResource(dxRenderTargetView);
	}

	CEDXDepthStencilView* dxDepthStencilView = static_cast<CEDXDepthStencilView*>(depthStencilView);
	DescriptorCache.SetDepthStencilView(dxDepthStencilView);
	CommandBatch->AddInUseResource(dxDepthStencilView);
}

void CEDXCommandContext::SetGraphicsPipelineState(CEGraphicsPipelineState* pipelineState) {
	Assert(pipelineState != nullptr);

	CEDXGraphicsPipelineState* dxPipelineState = static_cast<CEDXGraphicsPipelineState*>(pipelineState);
	if (dxPipelineState != CurrentGraphicsPipelineState.Get()) {
		CurrentGraphicsPipelineState = MakeSharedRef<CEDXGraphicsPipelineState>(dxPipelineState);
		CommandList.SetPipelineState(CurrentGraphicsPipelineState->GetPipeline());
	}

	CEDXRootSignature* dxRootSignature = dxPipelineState->GetRootSignature();
	if (dxRootSignature != CurrentRootSignature.Get()) {
		CurrentRootSignature = MakeSharedRef<CEDXRootSignature>(dxRootSignature);
		CommandList.SetGraphicsRootSignature(CurrentRootSignature.Get());
	}

}

void CEDXCommandContext::SetComputePipelineState(CEComputePipelineState* pipelineState) {
	Assert(pipelineState != nullptr);

	CEDXComputePipelineState* DxPipelineState = static_cast<CEDXComputePipelineState*>(pipelineState);
	if (DxPipelineState != CurrentComputePipelineState.Get()) {
		CurrentComputePipelineState = MakeSharedRef<CEDXComputePipelineState>(DxPipelineState);
		CommandList.SetPipelineState(CurrentComputePipelineState->GetPipeline());
	}

	CEDXRootSignature* DxRootSignature = DxPipelineState->GetRootSignature();

	if (DxRootSignature != CurrentRootSignature.Get()) {
		CurrentRootSignature = MakeSharedRef<CEDXRootSignature>(DxRootSignature);
		CommandList.SetComputeRootSignature(CurrentRootSignature.Get());
	}
}

void CEDXCommandContext::Set32BitShaderConstants(CEShader* shader, const void* shader32BitConstants,
                                                 uint32 num32BitConstants) {
	(void)shader;
	Assert(num32BitConstants <= D3D12_MAX_32BIT_SHADER_CONSTANTS_COUNT);
	ShaderConstantsCache.Set32BitShaderConstants((uint32*)shader32BitConstants, num32BitConstants);
}

void CEDXCommandContext::SetShaderResourceView(CEShader* shader, CEShaderResourceView* shaderResourceView,
                                               uint32 parameterIndex) {
	CEDXBaseShader* dxShader = ShaderCast(shader);
	Assert(dxShader != nullptr);

	CEDXShaderParameter parameterInfo = dxShader->GetShaderResourceParameter(parameterIndex);
	Assert(parameterInfo.Space == 0);
	Assert(parameterInfo.NumDescriptors == 1);

	CEDXShaderResourceView* dxShaderResourceView = static_cast<CEDXShaderResourceView*>(shaderResourceView);
	DescriptorCache.SetShaderResourceView(dxShaderResourceView,
	                                      dxShader->GetShaderVisibility(),
	                                      parameterInfo.Register);
}

void CEDXCommandContext::SetShaderResourceViews(CEShader* shader, CEShaderResourceView* const* shaderResourceView,
                                                uint32 numShaderResourceViews, uint32 parameterIndex) {
	CEDXBaseShader* dxShader = ShaderCast(shader);
	Assert(dxShader != nullptr);

	CEDXShaderParameter parameterInfo = dxShader->GetShaderResourceParameter(parameterIndex);
	Assert(parameterInfo.Space == 0);
	Assert(parameterInfo.NumDescriptors == numShaderResourceViews);

	for (uint32 i = 0; i < numShaderResourceViews; i++) {
		CEDXShaderResourceView* dxShaderResourceView = static_cast<CEDXShaderResourceView*>(shaderResourceView[i]);
		DescriptorCache.SetShaderResourceView(dxShaderResourceView, dxShader->GetShaderVisibility(),
		                                      parameterInfo.Register + i);
	}
}

void CEDXCommandContext::SetUnorderedAccessView(CEShader* shader, CEUnorderedAccessView* unorderedAccessView,
                                                uint32 parameterIndex) {
	CEDXBaseShader* dxShader = ShaderCast(shader);
	Assert(dxShader != nullptr);

	CEDXShaderParameter parameterInfo = dxShader->GetUnorderedAccessParameter(parameterIndex);
	Assert(parameterInfo.Space == 0);
	Assert(parameterInfo.NumDescriptors == 1);

	CEDXUnorderedAccessView* dxUnorderedAccessView = static_cast<CEDXUnorderedAccessView*>(unorderedAccessView);
	DescriptorCache.SetUnorderedAccessView(dxUnorderedAccessView, dxShader->GetShaderVisibility(),
	                                       parameterInfo.Register);
}

void CEDXCommandContext::SetUnorderedAccessViews(CEShader* shader, CEUnorderedAccessView* const* unorderedAccessViews,
                                                 uint32 numUnorderedAccessViews, uint32 parameterIndex) {
	CEDXBaseShader* dxShader = ShaderCast(shader);
	Assert(dxShader != nullptr);

	CEDXShaderParameter parameterInfo = dxShader->GetUnorderedAccessParameter(parameterIndex);
	Assert(parameterInfo.Space == 0);
	Assert(parameterInfo.NumDescriptors == numUnorderedAccessViews);

	for (uint32 i = 0; i < numUnorderedAccessViews; i++) {
		CEDXUnorderedAccessView* dxUnorderedAccessView = static_cast<CEDXUnorderedAccessView*>(unorderedAccessViews[i]);
		DescriptorCache.SetUnorderedAccessView(dxUnorderedAccessView, dxShader->GetShaderVisibility(),
		                                       parameterInfo.Register + i);
	}
}

void CEDXCommandContext::SetConstantBuffer(CEShader* shader, CEConstantBuffer* constantBuffer, uint32 parameterIndex) {
	CEDXBaseShader* dxShader = ShaderCast(shader);
	Assert(dxShader != nullptr);

	CEDXShaderParameter parameterInfo = dxShader->GetConstantBufferParameter(parameterIndex);
	Assert(parameterInfo.Space == 0);
	Assert(parameterInfo.NumDescriptors == 1);

	if (constantBuffer) {
		CEDXConstantBufferView& dxConstantBufferView = static_cast<CEDXConstantBuffer*>(constantBuffer)->GetView();
		DescriptorCache.SetConstantBufferView(&dxConstantBufferView, dxShader->GetShaderVisibility(),
		                                      parameterInfo.Register);
	}
	else {
		DescriptorCache.SetConstantBufferView(nullptr, dxShader->GetShaderVisibility(), parameterInfo.Register);
	}

}

void CEDXCommandContext::SetConstantBuffers(CEShader* shader, CEConstantBuffer* const* constantBuffers,
                                            uint32 numConstantBuffers, uint32 parameterIndex) {
	CEDXBaseShader* dxShader = ShaderCast(shader);
	Assert(dxShader != nullptr);

	CEDXShaderParameter parameterInfo = dxShader->GetConstantBufferParameter(parameterIndex);
	Assert(parameterInfo.Space == 0);
	Assert(parameterInfo.NumDescriptors == numConstantBuffers);

	for (uint32 i = 0; i < numConstantBuffers; i++) {
		if (constantBuffers[i]) {
			CEDXConstantBufferView& dxConstantBufferView = static_cast<CEDXConstantBuffer*>(constantBuffers[i])->
				GetView();
			//TODO: Probably add index to register but keep note that it is thing to check
			DescriptorCache.SetConstantBufferView(&dxConstantBufferView, dxShader->GetShaderVisibility(),
			                                      parameterInfo.Register + 1);
		}
		else {
			DescriptorCache.SetConstantBufferView(nullptr, dxShader->GetShaderVisibility(), parameterInfo.Register + 1);
		}
	}
}

void CEDXCommandContext::SetSamplerState(CEShader* shader, CESamplerState* samplerState, uint32 parameterIndex) {
	CEDXBaseShader* dxShader = ShaderCast(shader);
	Assert(dxShader != nullptr);

	CEDXShaderParameter parameterInfo = dxShader->GetSamplerStateParameter(parameterIndex);
	Assert(parameterInfo.Space == 0);
	Assert(parameterInfo.NumDescriptors == 1);

	CEDXSamplerState* dxSamplerState = static_cast<CEDXSamplerState*>(samplerState);
	DescriptorCache.SetSamplerState(dxSamplerState, dxShader->GetShaderVisibility(), parameterInfo.Register);
}

void CEDXCommandContext::SetSamplerStates(CEShader* shader, CESamplerState* const* samplerStates,
                                          uint32 numSamplerStates, uint32 parameterIndex) {
	CEDXBaseShader* dxShader = ShaderCast(shader);
	Assert(dxShader != nullptr);

	CEDXShaderParameter parameterInfo = dxShader->GetSamplerStateParameter(parameterIndex);
	Assert(parameterInfo.Space == 0);
	Assert(parameterInfo.NumDescriptors == 1);

	for (uint32 i = 0; i < numSamplerStates; i++) {
		CEDXSamplerState* dxSamplerState = static_cast<CEDXSamplerState*>(samplerStates[i]);

		//TODO: Probably add index to register but keep note that it is thing to check
		DescriptorCache.SetSamplerState(dxSamplerState, dxShader->GetShaderVisibility(), parameterInfo.Register + i);
	}
}

void CEDXCommandContext::ResolveTexture(CETexture* destination, CETexture* source) {
	FlushResourceBarriers();

	CEDXBaseTexture* dxDestination = TextureCast(destination);
	CEDXBaseTexture* dxSource = TextureCast(source);
	const DXGI_FORMAT destinationFormat = dxDestination->GetNativeFormat();
	const DXGI_FORMAT sourceFormat = dxSource->GetNativeFormat();

	Assert(destinationFormat == sourceFormat);

	CommandList.ResolveSubresource(dxDestination->GetResource(), dxSource->GetResource(), destinationFormat);

	CommandBatch->AddInUseResource(destination);
	CommandBatch->AddInUseResource(source);
}

void CEDXCommandContext::UpdateBuffer(CEBuffer* destination, uint64 offsetInBytes, uint64 sizeInBytes,
                                      const void* sourceData) {
	CEDXBaseBuffer* dxDestination = CEDXBufferCast(destination);
	UpdateBuffer(dxDestination->GetResource(), offsetInBytes, sizeInBytes, sourceData);

	CommandBatch->AddInUseResource(destination);
}

void CEDXCommandContext::UpdateTexture2D(CETexture2D* destination, uint32 width, uint32 height, uint32 mipLevel,
                                         const void* sourceData) {
	if (width > 0 && height > 0) {
		FlushResourceBarriers();

		CEDXBaseTexture* dxDestination = TextureCast(destination);
		const DXGI_FORMAT nativeFormat = dxDestination->GetNativeFormat();
		const uint32 stride = GetFormatStride(nativeFormat);
		const uint32 rowPitch = ((width * stride) + (D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u)) & ~(
			D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
		const uint32 sizeInBytes = height * rowPitch;

		CEDXGPUResourceUploader& gpuResourceUploader = CommandBatch->GetGpuResourceUploader();
		CEDXUploadAllocation allocation = gpuResourceUploader.LinearAllocate(sizeInBytes);

		const uint8* source = reinterpret_cast<const uint8*>(sourceData);
		for (uint32 y = 0; y < height; y++) {
			Memory::CEMemory::Memcpy(allocation.MappedPtr + (y * rowPitch), source + (y * width * stride),
			                         width * stride);
		}

		D3D12_TEXTURE_COPY_LOCATION sourceLocation;
		Memory::CEMemory::Memzero(&sourceLocation);

		sourceLocation.pResource = gpuResourceUploader.GetGPUResource();
		sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		sourceLocation.PlacedFootprint.Footprint.Format = nativeFormat;
		sourceLocation.PlacedFootprint.Footprint.Width = width;
		sourceLocation.PlacedFootprint.Footprint.Height = height;
		sourceLocation.PlacedFootprint.Footprint.Depth = 1;
		sourceLocation.PlacedFootprint.Footprint.RowPitch = rowPitch;
		sourceLocation.PlacedFootprint.Offset = allocation.ResourceOffset;

		D3D12_TEXTURE_COPY_LOCATION destLocation;
		Memory::CEMemory::Memzero(&destLocation);

		destLocation.pResource = dxDestination->GetResource()->GetResource();
		destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		destLocation.SubresourceIndex = mipLevel;

		CommandList.CopyTextureRegion(&destLocation, 0, 0, 0, &sourceLocation, nullptr);

		CommandBatch->AddInUseResource(destination);
	}
}

void CEDXCommandContext::CopyBuffer(CEBuffer* destination, Main::RenderLayer::CEBuffer* source,
                                    const CECopyBufferInfo& copyInfo) {
	FlushResourceBarriers();

	CEDXBaseBuffer* dxDestination = CEDXBufferCast(destination);
	CEDXBaseBuffer* dxSource = CEDXBufferCast(source);
	CommandList.CopyBufferRegion(dxDestination->GetResource(), copyInfo.DestinationOffset, dxSource->GetResource(),
	                             copyInfo.SourceOffset, copyInfo.SizeInBytes);

	CommandBatch->AddInUseResource(destination);
	CommandBatch->AddInUseResource(source);
}

void CEDXCommandContext::CopyTexture(CETexture* destination, CETexture* source) {
	FlushResourceBarriers();
	CEDXBaseTexture* dxDestination = TextureCast(destination);
	CEDXBaseTexture* dxSource = TextureCast(source);
	CommandList.CopyResource(dxDestination->GetResource(), dxSource->GetResource());

	CommandBatch->AddInUseResource(destination);
	CommandBatch->AddInUseResource(source);
}

void CEDXCommandContext::CopyTextureRegion(CETexture* destination, CETexture* source,
                                           const CECopyTextureInfo& copyTexture) {

	CEDXBaseTexture* dxDestination = TextureCast(destination);
	CEDXBaseTexture* dxSource = TextureCast(source);

	D3D12_TEXTURE_COPY_LOCATION sourceLocation;
	Memory::CEMemory::Memzero(&sourceLocation);

	sourceLocation.pResource = dxSource->GetResource()->GetResource();
	sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	sourceLocation.SubresourceIndex = copyTexture.Source.subresourceIndex;

	D3D12_BOX sourceBox;
	sourceBox.left = copyTexture.Source.x;
	sourceBox.right = copyTexture.Source.x + copyTexture.Width;
	sourceBox.bottom = copyTexture.Source.y;
	sourceBox.top = copyTexture.Source.y + copyTexture.Height;
	sourceBox.front = copyTexture.Source.z;
	sourceBox.back = copyTexture.Source.z + copyTexture.Depth;

	D3D12_TEXTURE_COPY_LOCATION destinationLocation;
	Memory::CEMemory::Memzero(&destinationLocation);

	destinationLocation.pResource = dxDestination->GetResource()->GetResource();
	destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	destinationLocation.SubresourceIndex = copyTexture.Destination.subresourceIndex;

	FlushResourceBarriers();

	CommandList.CopyTextureRegion(&destinationLocation, copyTexture.Destination.x, copyTexture.Destination.y,
	                              copyTexture.Destination.z, &sourceLocation, &sourceBox);

	CommandBatch->AddInUseResource(destination);
	CommandBatch->AddInUseResource(source);
}

void CEDXCommandContext::DiscardResource(CEResource* resource) {
	CommandBatch->AddInUseResource(resource);
}

void CEDXCommandContext::BuildRayTracingGeometry(CERayTracingGeometry* geometry, CEVertexBuffer* vertexBuffer,
                                                 CEIndexBuffer* indexBuffer, bool update) {
	FlushResourceBarriers();

	CEDXVertexBuffer* dxVertexBuffer = static_cast<CEDXVertexBuffer*>(vertexBuffer);
	CEDXIndexBuffer* dxIndexBuffer = static_cast<CEDXIndexBuffer*>(indexBuffer);
	CEDXRayTracingGeometry* dxGeometry = static_cast<CEDXRayTracingGeometry*>(geometry);
	dxGeometry->VertexBuffer = dxVertexBuffer;
	dxGeometry->IndexBuffer = dxIndexBuffer;
	dxGeometry->Build(*this, update);

	CommandBatch->AddInUseResource(geometry);
	CommandBatch->AddInUseResource(vertexBuffer);
	CommandBatch->AddInUseResource(indexBuffer);
}

void CEDXCommandContext::BuildRayTracingScene(CERayTracingScene* scene, const CERayTracingGeometryInstance* instances,
                                              uint32 numInstances, bool update) {
	FlushResourceBarriers();

	CEDXRayTracingScene* dxScene = static_cast<CEDXRayTracingScene*>(scene);
	dxScene->Build(*this, instances, numInstances, update);

	CommandBatch->AddInUseResource(scene);
}

void CEDXCommandContext::GenerateMips(CETexture* texture) {
	CEDXBaseTexture* dxTexture = TextureCast(texture);
	Assert(dxTexture != nullptr);

	D3D12_RESOURCE_DESC desc = dxTexture->GetResource()->GetDesc();
	desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	Assert(desc.MipLevels > 1);

	CERef<CEDXResource> stagingTexture = new CEDXResource(
		GetDevice(), desc, dxTexture->GetResource()->GetHeapType());
	if (!stagingTexture->Create(D3D12_RESOURCE_STATE_COMMON, nullptr)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to create StagingTexture for GenerateMips");
		return;
	}
	stagingTexture->SetName("GenerateMips StagingTexture");

	const bool isTextureCube = texture->AsTextureCube();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	Memory::CEMemory::Memzero(&srvDesc);

	srvDesc.Format = desc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (isTextureCube) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = desc.MipLevels;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	Memory::CEMemory::Memzero(&uavDesc);

	uavDesc.Format = desc.Format;
	if (isTextureCube) {
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.ArraySize = 6;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.PlaneSlice = 0;
	}
	else {
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.PlaneSlice = 0;
	}

	const uint32 mipLevelsPerDispatch = 4;
	const uint32 uavDescriptorHandleCount = Math::CEMath::AlignUp<uint32>(desc.MipLevels, mipLevelsPerDispatch);
	const uint32 numDispatches = uavDescriptorHandleCount / mipLevelsPerDispatch;

	CEDXOnlineDescriptorHeap* resourceHeap = CommandBatch->GetOnlineResourceDescriptorHeap();

	const uint32 startDescriptorHandleIndex = resourceHeap->AllocateHandles(uavDescriptorHandleCount + 1);
	const D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU = resourceHeap->GetCPUDescriptorHandleAt(startDescriptorHandleIndex);
	GetDevice()->CreateShaderResourceView(dxTexture->GetResource()->GetResource(), &srvDesc, srvHandleCPU);

	const uint32 uavStartDescriptorHandleIndex = startDescriptorHandleIndex + 1;
	for (uint32 i = 0; i < desc.MipLevels; i++) {
		if (isTextureCube) {
			uavDesc.Texture2DArray.MipSlice = i;
		}
		else {
			uavDesc.Texture2D.MipSlice = i;
		}

		const D3D12_CPU_DESCRIPTOR_HANDLE uavHandleCPU = resourceHeap->GetCPUDescriptorHandleAt(
			uavStartDescriptorHandleIndex + i);
		GetDevice()->CreateUnorderedAccessView(stagingTexture->GetResource(), nullptr, &uavDesc, uavHandleCPU);
	}

	for (uint32 i = desc.MipLevels; i < uavDescriptorHandleCount; i ++) {
		if (isTextureCube) {
			uavDesc.Texture2DArray.MipSlice = 0;
		}
		else {
			uavDesc.Texture2D.MipSlice = 0;
		}

		const D3D12_CPU_DESCRIPTOR_HANDLE uavHandleCPU = resourceHeap->GetCPUDescriptorHandleAt(
			uavStartDescriptorHandleIndex + i);
		GetDevice()->CreateUnorderedAccessView(nullptr, nullptr, &uavDesc, uavHandleCPU);
	}

	//TODO: it always should be state of: RESOURCE_STATE_COPY_DEST but maybe add parameter for more flexibility
	TransitionResource(dxTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE);
	TransitionResource(stagingTexture.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	FlushResourceBarriers();

	CommandList.CopyResource(stagingTexture.Get(), dxTexture->GetResource());

	TransitionResource(dxTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE,
	                   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	TransitionResource(stagingTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	FlushResourceBarriers();

	if (isTextureCube) {
		CommandList.SetPipelineState(GenerateMipsTexCube_PSO->GetPipeline());
		CommandList.SetComputeRootSignature(GenerateMipsTexCube_PSO->GetRootSignature());
	}
	else {
		CommandList.SetPipelineState(GenerateMipsTex2D_PSO->GetPipeline());
		CommandList.SetComputeRootSignature(GenerateMipsTex2D_PSO->GetRootSignature());
	}

	const D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU = resourceHeap->GetGPUDescriptorHandleAt(startDescriptorHandleIndex);
	ID3D12DescriptorHeap* onlineResourceHeap = resourceHeap->GetHeap()->GetHeap();
	CommandList.SetDescriptorHeaps(&onlineResourceHeap, 1);

	struct ConstantBuffer {
		uint32 SrcMipLevel;
		uint32 NumMipLevels;
		DirectX::XMFLOAT2 TexelSize;
	} ConstantBufferData;

	uint32 destinationWidth = static_cast<uint32>(desc.Width);
	uint32 destinationHeight = desc.Height;
	ConstantBufferData.SrcMipLevel = 0;

	const uint32 threadsZ = isTextureCube ? 6 : 1;
	uint32 remainingMipLevels = desc.MipLevels;
	for (uint32 i = 0; i < numDispatches; i++) {
		ConstantBufferData.TexelSize = DirectX::XMFLOAT2(1.0f / static_cast<float>(destinationWidth),
		                                                 1.0f / static_cast<float>(destinationHeight));
		ConstantBufferData.NumMipLevels = Math::CEMath::Min<uint32>(4, remainingMipLevels);

		CommandList.SetComputeRoot32BitConstants(&ConstantBufferData, 4, 0, 0);

		CommandList.SetComputeRootDescriptorTable(1, srvHandleGPU);

		const uint32 gpuDescriptorHandleIndex = i * mipLevelsPerDispatch;
		const D3D12_GPU_DESCRIPTOR_HANDLE uavHandleGPU = resourceHeap->GetGPUDescriptorHandleAt(
			uavStartDescriptorHandleIndex + gpuDescriptorHandleIndex);
		CommandList.SetComputeRootDescriptorTable(2, uavHandleGPU);

		constexpr uint32 threadCount = 8;
		const uint32 threadsX = Math::CEMath::DivideByMultiple(destinationWidth, threadCount);
		const uint32 threadsY = Math::CEMath::DivideByMultiple(destinationHeight, threadCount);
		CommandList.Dispatch(threadsX, threadsY, threadsZ);

		UnorderedAccessBarrier(stagingTexture.Get());

		TransitionResource(dxTexture->GetResource(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		                   D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionResource(stagingTexture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		                   D3D12_RESOURCE_STATE_COPY_SOURCE);
		FlushResourceBarriers();

		CommandList.CopyResource(dxTexture->GetResource(), stagingTexture.Get());

		TransitionResource(dxTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST,
		                   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		TransitionResource(stagingTexture.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE,
		                   D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		FlushResourceBarriers();

		destinationWidth = destinationWidth / 16;
		destinationHeight = destinationHeight / 16;

		ConstantBufferData.SrcMipLevel += 3;
		remainingMipLevels -= mipLevelsPerDispatch;
	}

	TransitionResource(dxTexture->GetResource(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
	                   D3D12_RESOURCE_STATE_COPY_DEST);
	FlushResourceBarriers();

	CommandBatch->AddInUseResource(texture);
	CommandBatch->AddInUseResource(stagingTexture.Get());
}

void CEDXCommandContext::TransitionTexture(CETexture* texture, CEResourceState beforeState,
                                           CEResourceState afterState) {
	const D3D12_RESOURCE_STATES dxBeforeState = ConvertResourceState(beforeState);
	const D3D12_RESOURCE_STATES dxAfterState = ConvertResourceState(afterState);

	CEDXBaseTexture* resource = TextureCast(texture);
	TransitionResource(resource->GetResource(), dxBeforeState, dxAfterState);
	CommandBatch->AddInUseResource(texture);
}

void CEDXCommandContext::TransitionBuffer(CEBuffer* buffer, CEResourceState beforeState, CEResourceState afterState) {
	const D3D12_RESOURCE_STATES dxBeforeState = ConvertResourceState(beforeState);
	const D3D12_RESOURCE_STATES dxAfterState = ConvertResourceState(afterState);

	CEDXBaseBuffer* resource = CEDXBufferCast(buffer);
	TransitionResource(resource->GetResource(), dxBeforeState, dxAfterState);
	CommandBatch->AddInUseResource(buffer);
}

void CEDXCommandContext::UnorderedAccessTextureBarrier(CETexture* texture) {
	CEDXBaseTexture* resource = TextureCast(texture);
	UnorderedAccessBarrier(resource->GetResource());

	CommandBatch->AddInUseResource(texture);
}

void CEDXCommandContext::UnorderedAccessBufferBarrier(CEBuffer* buffer) {
	CEDXBaseBuffer* resource = CEDXBufferCast(buffer);
	UnorderedAccessBarrier(resource->GetResource());

	CommandBatch->AddInUseResource(buffer);
}

void CEDXCommandContext::Draw(uint32 vertexCount, uint32 startVertexLocation) {
	FlushResourceBarriers();

	ShaderConstantsCache.CommitGraphics(CommandList, CurrentRootSignature.Get());
	DescriptorCache.CommitGraphicsDescriptors(CommandList, CommandBatch, CurrentRootSignature.Get());

	CommandList.DrawInstanced(vertexCount, 1, startVertexLocation, 0);
}

void CEDXCommandContext::DrawIndexed(uint32 indexCount, uint32 startIndexLocation, int32 baseVertexLocation) {
	FlushResourceBarriers();

	ShaderConstantsCache.CommitGraphics(CommandList, CurrentRootSignature.Get());
	DescriptorCache.CommitGraphicsDescriptors(CommandList, CommandBatch, CurrentRootSignature.Get());

	CommandList.DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
}

void CEDXCommandContext::DrawInstanced(uint32 vertexPerCountInstance, uint32 instanceCount, uint32 startVertexLocation,
                                       uint32 startInstanceLocation) {
	FlushResourceBarriers();

	ShaderConstantsCache.CommitGraphics(CommandList, CurrentRootSignature.Get());
	DescriptorCache.CommitGraphicsDescriptors(CommandList, CommandBatch, CurrentRootSignature.Get());

	CommandList.DrawInstanced(vertexPerCountInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void CEDXCommandContext::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount,
                                              uint32 startIndexLocation, uint32 baseVertexLocation,
                                              uint32 startInstanceLocation) {
	FlushResourceBarriers();

	ShaderConstantsCache.CommitGraphics(CommandList, CurrentRootSignature.Get());
	DescriptorCache.CommitComputeDescriptors(CommandList, CommandBatch, CurrentRootSignature.Get());

	CommandList.DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation,
	                                 startInstanceLocation);
}

void CEDXCommandContext::Dispatch(uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ) {
	FlushResourceBarriers();

	ShaderConstantsCache.CommitCompute(CommandList, CurrentRootSignature.Get());
	DescriptorCache.CommitComputeDescriptors(CommandList, CommandBatch, CurrentRootSignature.Get());

	CommandList.Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void CEDXCommandContext::ClearState() {
	Flush();

	for (CEDXCommandBatch& batch : CommandBatches) {
		batch.Reset();
	}

	InternalClearState();
}

void CEDXCommandContext::Flush() {
	const uint64 newFenceValue = ++FenceValue;
	if (!CommandQueue.SignalFence(Fence, newFenceValue)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to SignalFence for value " + std::to_string(newFenceValue))
		return;
	}

	if (!Fence.WaitForValue(FenceValue)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to WaitForValue " + std::to_string(FenceValue))
	}
}

void CEDXCommandContext::BeginExternalCapture() {
	IDXGraphicsAnalysis* pix = GetDevice()->GetPIXCaptureInterface();
	if (pix && !IsCapturing) {
		pix->BeginCapture();
		IsCapturing = true;
	}
}

void CEDXCommandContext::EndExternalCapture() {
	IDXGraphicsAnalysis* pix = GetDevice()->GetPIXCaptureInterface();
	if (pix && IsCapturing) {
		pix->EndCapture();
		IsCapturing = false;
	}
}

void CEDXCommandContext::InternalClearState() {
	DescriptorCache.Reset();
	ShaderConstantsCache.Reset();

	CurrentGraphicsPipelineState.Reset();
	CurrentRootSignature.Reset();
	CurrentComputePipelineState.Reset();
}
