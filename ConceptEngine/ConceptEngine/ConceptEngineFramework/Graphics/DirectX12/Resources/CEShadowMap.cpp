#include "CEShadowMap.h"

#include "../../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CEShadowMap::CEShadowMap(ID3D12Device* device, UINT width, UINT height): m_d3dDevice(device),
                                                                         m_width(width),
                                                                         m_height(height),
                                                                         m_viewport({
	                                                                         0.0f,
	                                                                         0.0f,
	                                                                         (float)width,
	                                                                         (float)height,
	                                                                         0.0f,
	                                                                         1.0f
                                                                         }),
                                                                         m_scissorRect({
	                                                                         0,
	                                                                         0,
	                                                                         (int)width,
	                                                                         (int)height
                                                                         }) {
	BuildResource();
}

UINT CEShadowMap::Width() const {
	return m_width;
}

UINT CEShadowMap::Height() const {
	return m_height;
}

ID3D12Resource* CEShadowMap::Resource() {
	return mShadowMap.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CEShadowMap::Srv() const {
	return mhGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE CEShadowMap::Dsv() const {
	return mhCpuDsv;
}

D3D12_VIEWPORT CEShadowMap::Viewport() const {
	return m_viewport;
}

D3D12_RECT CEShadowMap::ScissorRect() const {
	return m_scissorRect;
}

void CEShadowMap::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
                                   CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
                                   CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv) {
	mhCpuSrv = hCpuSrv;
	mhGpuSrv = hGpuSrv;
	mhCpuDsv = hCpuDsv;

	BuildDescriptors();
}

void CEShadowMap::Resize(UINT newWidth, UINT newHeight) {
	if ((m_width != newWidth) || (m_height != newHeight)) {
		m_width = newWidth;
		m_height = newHeight;
		BuildResource();

		//New resource, so we need new descriptors to that resource
		BuildDescriptors();
	}
}

void CEShadowMap::BuildDescriptors() {
	//Create SRV to resource so we can sample the shadow map in shader program
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	m_d3dDevice->CreateShaderResourceView(mShadowMap.Get(), &srvDesc, mhCpuSrv);

	//Create DSV to resource so we can render to shadow map
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	m_d3dDevice->CreateDepthStencilView(mShadowMap.Get(), &dsvDesc, mhCpuDsv);
}

void CEShadowMap::BuildResource() {
	//NOTE: Compressed formats cannot be used for UAV
	D3D12_RESOURCE_DESC texDesc;
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
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	
	auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&defaultHeapProps,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &texDesc,
	                                                   D3D12_RESOURCE_STATE_GENERIC_READ,
	                                                   &optClear,
	                                                   IID_PPV_ARGS(&mShadowMap)));
}
