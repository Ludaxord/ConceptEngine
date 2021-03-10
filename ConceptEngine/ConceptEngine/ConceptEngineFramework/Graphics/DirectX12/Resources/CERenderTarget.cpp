#include "CERenderTarget.h"

#include "../../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CERenderTarget::CERenderTarget(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format) {
	m_d3dDevice = device;

	m_width = width;
	m_height = height;
	Format = format;

	BuildResource();
}

ID3D12Resource* CERenderTarget::Resource() {
	return mOffscreenTex.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CERenderTarget::Srv() {
	return mhGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE CERenderTarget::Rtv() {
	return mhCpuRtv;
}

void CERenderTarget::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
                                      CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
                                      CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv) {
	mhCpuSrv = hCpuSrv;
	mhGpuSrv = hGpuSrv;
	mhCpuRtv = hCpuRtv;

	BuildDescriptors();
}

void CERenderTarget::OnResize(UINT newWidth, UINT newHeight) {
	if ((m_width != newWidth) || (m_height != newHeight)) {
		m_width = newWidth;
		m_height = newHeight;

		BuildResource();

		// New resource, so we need new descriptors to that resource.
		BuildDescriptors();
	}
}

void CERenderTarget::BuildDescriptors() {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	m_d3dDevice->CreateShaderResourceView(mOffscreenTex.Get(), &srvDesc, mhCpuSrv);
	m_d3dDevice->CreateRenderTargetView(mOffscreenTex.Get(), nullptr, mhCpuRtv);

	mOffscreenTex->SetName(L"OffScreen Render Target");
}

void CERenderTarget::BuildResource() {
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
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc,
	                                                   D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
	                                                   IID_PPV_ARGS(&mOffscreenTex)));
}
