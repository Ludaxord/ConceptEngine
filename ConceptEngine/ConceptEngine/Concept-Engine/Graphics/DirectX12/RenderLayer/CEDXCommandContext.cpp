#include "CEDXCommandContext.h"

#include "CEDXBuffer.h"
#include "CEDXRayTracing.h"
#include "CEDXSamplerState.h"
#include "CEDXShaderCompiler.h"
#include "CEDXTexture.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;
using namespace ConceptEngine::Core::Containers;

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
	if (requiredSize > sizeInBytes) {
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
		return false;
	}

	OnlineResourceDescriptorHeap = new CEDXOnlineDescriptorHeap(Device, D3D12_DEFAULT_ONLINE_RESOURCE_DESCRIPTOR_COUNT,
	                                                            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (!OnlineResourceDescriptorHeap->Create()) {
		return false;
	}

	OnlineSamplerDescriptorHeap = new CEDXOnlineDescriptorHeap(Device, D3D12_DEFAULT_ONLINE_SAMPLER_DESCRIPTOR_COUNT,
	                                                           D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	if (!OnlineSamplerDescriptorHeap->Create()) {
		return false;
	}

	OnlineRayTracingResourceDescriptorHeap = new CEDXOnlineDescriptorHeap(
		Device, D3D12_DEFAULT_ONLINE_RESOURCE_DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (!OnlineRayTracingResourceDescriptorHeap->Create()) {
		return false;
	}

	OnlineRayTracingSamplerDescriptorHeap = new CEDXOnlineDescriptorHeap(
		Device, D3D12_DEFAULT_ONLINE_SAMPLER_DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	if (!OnlineRayTracingSamplerDescriptorHeap->Create()) {
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
		return false;
	}

	for (uint32 i = 0; i < 3; i++) {
		CEDXCommandBatch& batch = CommandBatches.EmplaceBack(GetDevice());
		if (!batch.Create()) {
			return false;
		}
	}

	if (!CommandList.Create(D3D12_COMMAND_LIST_TYPE_DIRECT, CommandBatches[0].GetCommandAllocator(), nullptr)) {
		return false;
	}

	FenceValue = 0;
	if (!Fence.Create(FenceValue)) {
		return false;
	}

	if (!DescriptorCache.Create()) {
		return false;
	}

	CEArray<uint8> code;
	if (!CED3DShaderCompiler->
		CompileFromFile("DirectX12/Shaders/GenerateMipsTex2D.hlsl", "Main", nullptr, CEShaderStage::Compute,
		                CEShaderModel::SM_6_0, code)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to compile GenerateMipsTex2D shader");
		return false;
	}

	Core::Common::CERef<CEDXComputeShader> shader = new CEDXComputeShader(GetDevice(), code);
	if (shader->Create()) {
		return false;
	}

	GenerateMipsTex2D_PSO = new CEDXComputePipelineState(GetDevice(), shader);
	if (!GenerateMipsTex2D_PSO->Create()) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to create GenerateMipsTex2D PipelineState");
		return false;
	}

	GenerateMipsTex2D_PSO->SetName("GenerateMipsTex2D Gen PSO");

	if (CED3DShaderCompiler->CompileFromFile("DirectX12/Shaders/GenerateMipsTexCube.hlsl", "Main", nullptr,
	                                         CEShaderStage::Compute, CEShaderModel::SM_6_0, code)) {
		CE_LOG_ERROR("[CEDXCommandContext]: Failed to compile GenerateMipsTexCube Shader");
		return false;
	}

	shader = new CEDXComputeShader(GetDevice(), code);
	if (!shader->Create()) {
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
		const uint64 waitValue = Math::CEMath::Max<uint64>(FenceValue - (CommandBatches.Size() - 1), 0);
		Fence.WaitForValue(waitValue);
	}

	if (!CommandBatch->Reset()) {
		return;
	}

	InternalClearState();

	if (!CommandList.Reset(CommandBatch->GetCommandAllocator())) {
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
		return;
	}

	CommandQueue.ExecuteCommandList(&CommandList);

	if (!CommandQueue.SignalFence(Fence, newFenceValue)) {
		return;
	}
}

void CEDXCommandContext::BeginTimeStamp(CEGPUProfiler* profiler, uint32 index) {
	ID3D12GraphicsCommandList* commandList = CommandList.GetGraphicsCommandList();
	CEDXGPUProfiler* dxProfiler = static_cast<CEDXGPUProfiler*>(profiler);
	Assert(profiler);
	dxProfiler->BeginQuery(commandList, index);
	ResolveProfilers.EmplaceBack(Core::Common::MakeSharedRef<CEDXGPUProfiler>(dxProfiler));
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
		CEDXBaseTexture* dxTexture = D3D12TextureCast(shadingImage);
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
		CurrentGraphicsPipelineState = Core::Common::MakeSharedRef<CEDXGraphicsPipelineState>(dxPipelineState);
		CommandList.SetPipelineState(CurrentGraphicsPipelineState->GetPipeline());
	}

	CEDXRootSignature* dxRootSignature = dxPipelineState->GetRootSignature();
	if (dxRootSignature != CurrentRootSignature.Get()) {
		CurrentRootSignature = Core::Common::MakeSharedRef<CEDXRootSignature>(dxRootSignature);
		CommandList.SetGraphicsRootSignature(CurrentRootSignature.Get());
	}

}

void CEDXCommandContext::SetComputePipelineState(CEComputePipelineState* pipelineState) {
	Assert(pipelineState != nullptr);
	CEDXComputePipelineState* dxPipelineState = static_cast<CEDXComputePipelineState*>(pipelineState);
	if (dxPipelineState != CurrentComputePipelineState.Get()) {
		CurrentComputePipelineState = Core::Common::MakeSharedRef<CEDXComputePipelineState>(dxPipelineState);
		CommandList.SetPipelineState(CurrentComputePipelineState->GetPipeline());
	}

	CEDXRootSignature* dxRootSignature = dxPipelineState->GetRootSignature();
	if (dxRootSignature != CurrentRootSignature.Get()) {
		CurrentRootSignature = Core::Common::MakeSharedRef<CEDXRootSignature>(dxRootSignature);
		CommandList.SetComputeRootSignature(CurrentRootSignature.Get());
	}
}

void CEDXCommandContext::Set32BitShaderConstants(CEShader* shader, const void* shader32BitConstants,
                                                 uint32 num32BitConstants) {
}

void CEDXCommandContext::SetShaderResourceView(CEShader* shader, CEShaderResourceView* shaderResourceView,
                                               uint32 parameterIndex) {
}

void CEDXCommandContext::SetShaderResourceViews(CEShader* shader, CEShaderResourceView* const* shaderResourceView,
                                                uint32 numShaderResourceViews, uint32 parameterIndex) {
}

void CEDXCommandContext::SetUnorderedAccessView(CEShader* shader, CEUnorderedAccessView* unorderedAccessView,
                                                uint32 parameterIndex) {
}

void CEDXCommandContext::SetUnorderedAccessViews(CEShader* shader, CEUnorderedAccessView* const* unorderedAccessViews,
                                                 uint32 numUnorderedAccessViews, uint32 parameterIndex) {
}

void CEDXCommandContext::SetConstantBuffer(CEShader* shader, CEConstantBuffer* constantBuffer, uint32 parameterIndex) {
}

void CEDXCommandContext::SetConstantBuffers(CEShader* shader, CEConstantBuffer* const* constantBuffers,
                                            uint32 numConstantBuffers, uint32 parameterIndex) {
}

void CEDXCommandContext::SetSamplerState(CEShader* shader, CESamplerState* samplerState, uint32 parameterIndex) {
}

void CEDXCommandContext::SetSamplerStates(CEShader* shader, CESamplerState* const* samplerStates,
                                          uint32 numSamplerStates, uint32 parameterIndex) {
}

void CEDXCommandContext::ResolveTexture(CETexture* destination, CETexture* source) {
}

void CEDXCommandContext::UpdateBuffer(CEBuffer* destination, uint64 offsetInBytes, uint64 sizeInBytes,
                                      const void* sourceData) {
}

void CEDXCommandContext::UpdateTexture2D(CETexture2D* destination, uint32 width, uint32 height, uint32 mipLevel,
                                         const void* sourceData) {
}

void CEDXCommandContext::CopyBuffer(CEBuffer* destination, Main::RenderLayer::CEBuffer* source,
                                    const CECopyBufferInfo& copyInfo) {
}

void CEDXCommandContext::CopyTexture(CETexture* destination, CETexture* source) {
}

void CEDXCommandContext::CopyTextureRegion(CETexture* destination, CETexture* source,
                                           const CECopyTextureInfo& copyTexture) {
}

void CEDXCommandContext::DiscardResource(CEResource* resource) {
}

void CEDXCommandContext::BuildRayTracingGeometry(CERayTracingGeometry* geometry, CEVertexBuffer* vertexBuffer,
                                                 CEIndexBuffer* indexBuffer, bool update) {
}

void CEDXCommandContext::BuildRayTracingScene(CERayTracingScene* scene, const CERayTracingGeometryInstance* instances,
                                              uint32 numInstances, bool update) {
}

void CEDXCommandContext::GenerateMips(CETexture* texture) {
}

void CEDXCommandContext::
TransitionTexture(CETexture* texture, CEResourceState beforeState, CEResourceState afterState) {
}

void CEDXCommandContext::TransitionBuffer(CEBuffer* buffer, CEResourceState beforeState, CEResourceState afterState) {
}

void CEDXCommandContext::UnorderedAccessTextureBarrier(CETexture* texture) {
}

void CEDXCommandContext::UnorderedAccessBufferBarrier(CEBuffer* buffer) {
}

void CEDXCommandContext::Draw(uint32 vertexCount, uint32 startVertexLocation) {
}

void CEDXCommandContext::DrawIndexed(uint32 indexCount, uint32 startIndexLocation, int32 baseVertexLocation) {
}

void CEDXCommandContext::DrawInstanced(uint32 vertexPerCountInstance, uint32 instanceCount, uint32 startVertexLocation,
                                       uint32 startInstanceLocation) {
}

void CEDXCommandContext::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount,
                                              uint32 startIndexLocation, uint32 baseVertexLocation,
                                              uint32 startInstanceLocation) {
}

void CEDXCommandContext::Dispatch(uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ) {
}

void CEDXCommandContext::ClearState() {
}

void CEDXCommandContext::Flush() {
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
