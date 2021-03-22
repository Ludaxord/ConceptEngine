#include "CESSAO.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CESSAO::CESSAO(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT width,
               UINT height) : m_d3dDevice(device) {
	Resize(width, height);

	BuildOffsetVectors();
	BuildRandomVectorTexture(cmdList);
}

UINT CESSAO::SSAOMapWidth() const {
	return m_renderTargetWidth / 2;
}

UINT CESSAO::SSAOMapHeight() const {
	return m_renderTargetHeight / 2;
}

void CESSAO::GetOffsetVectors(DirectX::XMFLOAT4 offsets[14]) {
	std::copy(&m_offsets[0], &m_offsets[14], &offsets[0]);
}

std::vector<float> CESSAO::CalcGaussWeights(float sigma) {
}

ID3D12Resource* CESSAO::NormalMap() {
}

ID3D12Resource* CESSAO::AmbientMap() {
}

CD3DX12_CPU_DESCRIPTOR_HANDLE CESSAO::NormalMapRtv() const {
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CESSAO::NormalMapSrv() const {
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CESSAO::AmbientMapSrv() const {
}

void CESSAO::BuildDescriptors(ID3D12Resource* depthStencilBuffer, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
                              CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hCpuRtv,
                              UINT cbvSrvUavDescriptorSize,
                              UINT rtvDescriptorSize) {
}

void CESSAO::RebuildDescriptors(ID3D12Resource* depthStencilBuffer) {
}

void CESSAO::SetPSOs(ID3D12PipelineState* ssaoPso, ID3D12PipelineState* ssaoBlurPso) {
}

void CESSAO::Resize(UINT newWidth, UINT newHeight) {
}

void CESSAO::ComputeSSAO(ID3D12GraphicsCommandList* cmdList, CEFrameResource* currFrame, int blurCount) {
}

void CESSAO::BlurAmbientMap(ID3D12GraphicsCommandList* cmdList, CEFrameResource currFrame, int blurCount) {
}

void CESSAO::BlurAmbientMap(ID3D12GraphicsCommandList* cmdList, bool horzBlur) {
}

void CESSAO::BuildResources() {
}

void CESSAO::BuildRandomVectorTexture(ID3D12GraphicsCommandList* cmdList) {
}

void CESSAO::BuildOffsetVectors() {
}
