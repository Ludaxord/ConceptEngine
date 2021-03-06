#include "CEBlurFilter.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CEBlurFilter::CEBlurFilter(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format) {
}

ID3D12Resource* CEBlurFilter::Output() {
}

void CEBlurFilter::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
                                    CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
                                    UINT descriptorSize) {
}

void CEBlurFilter::OnResize(UINT newWidth, UINT newHeight) {
}

void CEBlurFilter::Execute(ID3D12GraphicsCommandList* cmdList,
                           ID3D12RootSignature* rootSignature,
                           ID3D12PipelineState* horzBlurPso,
                           ID3D12PipelineState* vertBlurPso,
                           ID3D12Resource* input,
                           int blurCount) {
}

std::vector<float> CEBlurFilter::CalcGaussWeights(float sigma) {
}

void CEBlurFilter::BuildDescriptors() {
}

void CEBlurFilter::BuildResources() {
}
