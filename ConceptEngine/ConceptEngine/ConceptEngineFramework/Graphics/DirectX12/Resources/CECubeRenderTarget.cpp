#include "CECubeRenderTarget.h"

#include "../../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CECubeRenderTarget::CECubeRenderTarget(ID3D12Device* device,
                                       UINT width, UINT height, DXGI_FORMAT format) {
	m_d3dDevice = device;

	m_width = width;
	m_height = height;
	Format = format;

	m_viewport = {0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f};
	m_scissorRect = {0, 0, (long)width, (long)height};

	BuildResource();
}

ID3D12Resource* CECubeRenderTarget::Resource() {
	return m_cubeMap.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CECubeRenderTarget::Srv() {
	return mhGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE CECubeRenderTarget::Rtv(int faceIndex) {
	return mhCpuRtv[faceIndex];
}

D3D12_VIEWPORT CECubeRenderTarget::Viewport() const {
	return m_viewport;
}

D3D12_RECT CECubeRenderTarget::ScissorRect() const {
	return m_scissorRect;
}

void CECubeRenderTarget::BuildDescriptors(
	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv[6]) {
	mhCpuSrv = hCpuSrv;
	mhGpuSrv = hGpuSrv;

	for (int i = 0; i < 6; ++i)
		mhCpuRtv[i] = hCpuRtv[i];

	//  Create the descriptors
	BuildDescriptors();
}

void CECubeRenderTarget::Resize(UINT newWidth, UINT newHeight) {
	if ((m_width != newWidth) || (m_height != newHeight)) {
		m_width = newWidth;
		m_height = newHeight;

		BuildResource();

		//New resource so we need new descriptors to that resource
		BuildDescriptors();
	}
}

void CECubeRenderTarget::BuildDescriptors() {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	//Create SRV to entire CubeMap Resource;
	m_d3dDevice->CreateShaderResourceView(m_cubeMap.Get(), &srvDesc, mhCpuSrv);

	//Create RTV to each CubeMap Face
	for (int i = 0; i < 6; ++i) {
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Format = Format;
		rtvDesc.Texture2DArray.MipSlice = 0;
		rtvDesc.Texture2DArray.PlaneSlice = 0;

		//Render target to ith element
		rtvDesc.Texture2DArray.FirstArraySlice = i;

		//Only view one element of array
		rtvDesc.Texture2DArray.ArraySize = 1;

		//Create RTV to ith cubeMap face
		m_d3dDevice->CreateRenderTargetView(m_cubeMap.Get(), &rtvDesc, mhCpuRtv[i]);
	}
}

void CECubeRenderTarget::BuildResource() {
	/*
	 * NOTE: compressed formats cannot be used for UAV
	 */
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = m_width;
	texDesc.Height = m_height;
	texDesc.DepthOrArraySize = 6;
	texDesc.MipLevels = 1;
	texDesc.Format = Format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(
		m_d3dDevice->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_cubeMap)
		)
	);
}
