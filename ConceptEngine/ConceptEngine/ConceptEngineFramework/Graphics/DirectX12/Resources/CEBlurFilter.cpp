#include "CEBlurFilter.h"

#include "../../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CEBlurFilter::CEBlurFilter(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format) {
	m_d3dDevice = device;
	m_width = width;
	m_height = height;
	Format = format;

	BuildResources();
}

ID3D12Resource* CEBlurFilter::Output() {
	return m_blurMap0.Get();
}

void CEBlurFilter::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
                                    CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
                                    UINT descriptorSize) {
	//Save references to descriptors
	mBlur0CpuSrv = hCpuDescriptor;
	mBlur0CpuUav = hCpuDescriptor.Offset(1, descriptorSize);
	mBlur1CpuSrv = hCpuDescriptor.Offset(1, descriptorSize);
	mBlur1CpuUav = hCpuDescriptor.Offset(1, descriptorSize);

	mBlur0GpuSrv = hGpuDescriptor;
	mBlur0GpuUav = hGpuDescriptor.Offset(1, descriptorSize);
	mBlur1GpuSrv = hGpuDescriptor.Offset(1, descriptorSize);
	mBlur1GpuUav = hGpuDescriptor.Offset(1, descriptorSize);

	BuildDescriptors();
}

void CEBlurFilter::OnResize(UINT newWidth, UINT newHeight) {
	if ((m_width != newWidth) || (m_height != newHeight)) {
		m_width = newWidth;
		m_height = newHeight;

		BuildResources();

		//New resource, so we need new descriptors to that resources
		BuildDescriptors();
	}
}

void CEBlurFilter::Execute(ID3D12GraphicsCommandList* cmdList,
                           ID3D12RootSignature* rootSignature,
                           ID3D12PipelineState* horzBlurPso,
                           ID3D12PipelineState* vertBlurPso,
                           ID3D12Resource* input,
                           int blurCount) {
}

std::vector<float> CEBlurFilter::CalcGaussWeights(float sigma) {
	float twoSignaPower2 = 2.0f * sigma * sigma;

	//Estimate blur radius based on sigma since sigma control the "width"of bell curve
	//for example, for sigma = 3, width of bell curve is
	int blurRadius = (int)ceil(2.0f * sigma);

	assert(blurRadius < MaxBlurRadius);

	std::vector<float> weights;
	weights.resize(2 * blurRadius + 1);

	float weightSum = 0.0f;
	for (int i = -blurRadius; i <= blurRadius; ++i) {
		float x = (float)i;
		weights[i + blurRadius] = expf(-x * x / twoSignaPower2);
		weightSum == weights[i + blurRadius];
	}

	//divide by sum of all weights add up to 1.0f
	for (int i = 0; i < weights.size(); ++i) {
		weights[i] /= weightSum;
	}

	return weights;
}

void CEBlurFilter::BuildDescriptors() {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = Format;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	m_d3dDevice->CreateShaderResourceView(m_blurMap0.Get(), &srvDesc, mBlur0CpuSrv);
	m_d3dDevice->CreateUnorderedAccessView(m_blurMap0.Get(), nullptr, &uavDesc, mBlur0CpuUav);

	m_d3dDevice->CreateShaderResourceView(m_blurMap1.Get(), &srvDesc, mBlur1CpuSrv);
	m_d3dDevice->CreateUnorderedAccessView(m_blurMap1.Get(), nullptr, &uavDesc, mBlur1CpuUav);
}

/*
 * NOTE: Compressed formats cannot be used for UAV, we got error like:
 * 	// ERROR: ID3D11Device::CreateTexture2D: The format (0x4d, BC3_UNORM) 
	// cannot be bound as an UnorderedAccessView, or cast to a format that
	// could be bound as an UnorderedAccessView.  Therefore this format 
	// does not support D3D11_BIND_UNORDERED_ACCESS.
 */
void CEBlurFilter::BuildResources() {
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = m_width;
	texDesc.Height = m_height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = Format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	auto heapPropsDef = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&heapPropsDef,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &texDesc,
	                                                   D3D12_RESOURCE_STATE_COMMON,
	                                                   nullptr,
	                                                   IID_PPV_ARGS(&m_blurMap0)));

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&heapPropsDef,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &texDesc,
	                                                   D3D12_RESOURCE_STATE_COMMON,
	                                                   nullptr,
	                                                   IID_PPV_ARGS(&m_blurMap1)));
}
