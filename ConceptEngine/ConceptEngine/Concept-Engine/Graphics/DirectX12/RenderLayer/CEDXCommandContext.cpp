#include "CEDXCommandContext.h"

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
