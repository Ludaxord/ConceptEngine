#include "CESobelFilter.h"

#include "../../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CESobelFilter::CESobelFilter(ID3D12Device* device, UINT width,
                             UINT height, DXGI_FORMAT format) {
	m_d3dDevice = device;

	m_width = width;
	m_height = height;
	Format = format;

	BuildResource();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CESobelFilter::OutputSrv() const {
	return mhGpuSrv;
}

UINT CESobelFilter::DescriptorCount() const {
	return 2;
}

void CESobelFilter::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
                                     CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
                                     UINT descriptorSize) {
	//Save references to descriptors
	mhCpuSrv = hCpuDescriptor;
	mhCpuUav = hCpuDescriptor.Offset(1, descriptorSize);
	mhGpuSrv = hGpuDescriptor;
	mhGpuUav = hGpuDescriptor.Offset(1, descriptorSize);

	BuildDescriptors();
}

void CESobelFilter::OnResize(UINT newWidth, UINT newHeight) {
	if ((m_width != newWidth) || (m_height != newHeight)) {
		m_width = newWidth;
		m_height = newHeight;

		BuildResource();

		//New resource, so we need new descriptors to that resource
		BuildDescriptors();
	}
}

void CESobelFilter::Execute(ID3D12GraphicsCommandList* cmdList,
                            ID3D12RootSignature* rootSignature,
                            ID3D12PipelineState* Pso,
                            CD3DX12_GPU_DESCRIPTOR_HANDLE input) {
	cmdList->SetComputeRootSignature(rootSignature);
	cmdList->SetPipelineState(Pso);

	cmdList->SetComputeRootDescriptorTable(0, input);
	cmdList->SetComputeRootDescriptorTable(2, mhGpuUav);

	auto trReadUA = CD3DX12_RESOURCE_BARRIER::Transition(mOutput.Get(),
	                                                     D3D12_RESOURCE_STATE_GENERIC_READ,
	                                                     D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cmdList->ResourceBarrier(1, &trReadUA);

	//How many groups do we need to dispatch to cover image, where each group covers 16x16 pixels
	int numGroupX = (UINT)ceilf(m_width / 16.0f);
	int numGroupY = (UINT)ceilf(m_height / 16.0f);
	cmdList->Dispatch(numGroupX, numGroupY, 1);

	auto trUARead = CD3DX12_RESOURCE_BARRIER::Transition(mOutput.Get(),
	                                                     D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
	                                                     D3D12_RESOURCE_STATE_GENERIC_READ);
	cmdList->ResourceBarrier(1, &trUARead);
}

void CESobelFilter::BuildDescriptors() {
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

	m_d3dDevice->CreateShaderResourceView(mOutput.Get(), &srvDesc, mhCpuSrv);
	m_d3dDevice->CreateUnorderedAccessView(mOutput.Get(), nullptr, &uavDesc, mhCpuUav);
	mOutput->SetName(L"Sobel Filter");
}

void CESobelFilter::BuildResource() {
	// Note, compressed formats cannot be used for UAV.  We get error like:
	// ERROR: ID3D11Device::CreateTexture2D: The format (0x4d, BC3_UNORM) 
	// cannot be bound as an UnorderedAccessView, or cast to a format that
	// could be bound as an UnorderedAccessView.  Therefore this format 
	// does not support D3D11_BIND_UNORDERED_ACCESS.
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

	auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&defaultHeap,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &texDesc,
	                                                   D3D12_RESOURCE_STATE_GENERIC_READ,
	                                                   nullptr,
	                                                   IID_PPV_ARGS(&mOutput)));
}
