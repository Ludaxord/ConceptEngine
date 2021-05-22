#include "CEDXCommandContext.h"

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
}

CEDXUploadAllocation CEDXGPUResourceUploader::LinearAllocate(uint32 sizeInBytes) {
}

CEDXCommandBatch::CEDXCommandBatch(CEDXDevice* device): Device(device), CommandAllocator(device),
                                                        GPUResourceUploader(device),
                                                        OnlineResourceDescriptorHeap(nullptr),
                                                        OnlineSamplerDescriptorHeap(nullptr), Resources() {
}

bool CEDXCommandBatch::Create() {
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
}

bool CEDXCommandContext::Create() {
}

void CEDXCommandContext::UpdateBuffer(CEDXResource* Resource, uint64 offsetInBytes, uint64 sizeInBytes,
                                      const void* sourceData) {
}

void CEDXCommandContext::Begin() {
}

void CEDXCommandContext::End() {
}

void CEDXCommandContext::BeginTimeStamp(CEGPUProfiler* profiler, uint32 index) {
}

void CEDXCommandContext::EndTimeStamp(CEGPUProfiler* profiler, uint32 index) {
}

void CEDXCommandContext::DispatchRays(CERayTracingScene* rayTracingScene, CERayTracingPipelineState* pipelineState,
                                      uint32 width, uint32 height, uint32 depth) {
}

void CEDXCommandContext::SetRayTracingBindings(CERayTracingScene* rayTracingScene,
                                               CERayTracingPipelineState* pipelineState,
                                               const CERayTracingShaderResources* globalResource,
                                               const CERayTracingShaderResources* rayGenLocalResources,
                                               const CERayTracingShaderResources* missLocalResources,
                                               const CERayTracingShaderResources* hitGroupResources,
                                               uint32 numHitGroupResources) {
}

void CEDXCommandContext::InsertMaker(const std::string& message) {
}

void CEDXCommandContext::ClearRenderTargetView(CERenderTargetView* renderTargetView, Math::CEColorF& clearColor) {
}

void CEDXCommandContext::
ClearDepthStencilView(CEDepthStencilView* depthStencilView, const CEDepthStencilF& clearColor) {
}

void CEDXCommandContext::ClearUnorderedAccessViewFloat(CEUnorderedAccessView* unorderedAccessView,
                                                       const Math::CEColorF& clearColor) {
}

void CEDXCommandContext::SetShadingRate(CEShadingRate shadingRate) {
}

void CEDXCommandContext::SetShadingRateImage(CETexture2D* shadingImage) {
}

void CEDXCommandContext::SetViewport(float width, float height, float minDepth, float maxDepth, float x, float y) {
}

void CEDXCommandContext::SetScissorRect(float width, float height, float x, float y) {
}

void CEDXCommandContext::SetBlendFactor(const Math::CEColorF& color) {
}

void CEDXCommandContext::BeginRenderPass() {
}

void CEDXCommandContext::EndRenderPass() {
}

void CEDXCommandContext::SetPrimitiveTopology(CEPrimitiveTopology primitiveTopologyType) {
}

void CEDXCommandContext::SetVertexBuffers(CEVertexBuffer* const* vertexBuffers, uint32 bufferCount, uint32 bufferSlot) {
}

void CEDXCommandContext::SetIndexBuffer(CEIndexBuffer* indexBuffer) {
}

void CEDXCommandContext::SetRenderTargets(CERenderTargetView* const* renderTargetView, uint32 renderTargetCount,
                                          CEDepthStencilView* depthStencilView) {
}

void CEDXCommandContext::SetGraphicsPipelineState(CEGraphicsPipelineState* pipelineState) {
}

void CEDXCommandContext::SetComputePipelineState(CEComputePipelineState* pipelineState) {
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
}

void CEDXCommandContext::EndExternalCapture() {
}

void CEDXCommandContext::InternalClearState() {
}
