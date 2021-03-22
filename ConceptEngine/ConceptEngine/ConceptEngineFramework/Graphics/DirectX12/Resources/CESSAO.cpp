#include "CESSAO.h"

#include "../CEDX12Manager.h"

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
	float twoSigma2 = 2.0f * sigma * sigma;

	/*
	 * Estimate blur radius based on sigma since sigma controls the "width" of bell curve.
	 */
	int blurRadius = (int)ceil(2.0f * sigma);

	assert(blurRadius <= MaxBlurRadius);

	std::vector<float> weights;
	weights.resize(2 * blurRadius + 1);

	float weightSum = 0.0f;

	for (int i = -blurRadius; i <= blurRadius; ++i) {
		float x = (float)i;
		weights[i + blurRadius] = expf(-x * x / twoSigma2);
		weightSum += weights[i + blurRadius];
	}

	//Divide by sume so all weights add up to 1.0
	for (int i = 0; i < weights.size(); ++i) {
		weights[i] /= weightSum;
	}

	return weights;
}

ID3D12Resource* CESSAO::NormalMap() {
	return m_normalMap.Get();
}

ID3D12Resource* CESSAO::AmbientMap() {
	return m_ambientMap0.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE CESSAO::NormalMapRtv() const {
	return mhNormalMapCpuRtv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CESSAO::NormalMapSrv() const {
	return mhNormalMapGpuSrv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CESSAO::AmbientMapSrv() const {
	return mhAmbientMap0GpuSrv;
}

void CESSAO::BuildDescriptors(ID3D12Resource* depthStencilBuffer,
                              CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
                              CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
                              CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv,
                              UINT cbvSrvUavDescriptorSize,
                              UINT rtvDescriptorSize) {
	/*
	 * Save references to descriptors. The SSAO reserves heap space for 5 contiguous SRV's
	 */
	mhAmbientMap0CpuSrv = hCpuSrv;
	mhAmbientMap1CpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhNormalMapCpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhDepthMapCpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhRandomVectorMapCpuSrv = hCpuSrv.Offset(1, cbvSrvUavDescriptorSize);

	mhAmbientMap0GpuSrv = hGpuSrv;
	mhAmbientMap1GpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhNormalMapGpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhDepthMapGpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);
	mhRandomVectorMapGpuSrv = hGpuSrv.Offset(1, cbvSrvUavDescriptorSize);

	mhNormalMapCpuRtv = hCpuRtv;
	mhAmbientMap0CpuRtv = hCpuRtv.Offset(1, rtvDescriptorSize);
	mhAmbientMap1CpuRtv = hCpuRtv.Offset(1, rtvDescriptorSize);

	//Create descriptors
	RebuildDescriptors(depthStencilBuffer);
}

void CESSAO::RebuildDescriptors(ID3D12Resource* depthStencilBuffer) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = NormalMapFormat;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	m_d3dDevice->CreateShaderResourceView(m_normalMap.Get(), &srvDesc, mhNormalMapCpuSrv);

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	m_d3dDevice->CreateShaderResourceView(depthStencilBuffer, &srvDesc, mhDepthMapCpuSrv);

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dDevice->CreateShaderResourceView(m_randomVectorMap.Get(), &srvDesc, mhRandomVectorMapCpuSrv);

	srvDesc.Format = AmbientMapFormat;
	m_d3dDevice->CreateShaderResourceView(m_ambientMap0.Get(), &srvDesc, mhAmbientMap0CpuSrv);
	m_d3dDevice->CreateShaderResourceView(m_ambientMap1.Get(), &srvDesc, mhAmbientMap1CpuSrv);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = NormalMapFormat;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	m_d3dDevice->CreateRenderTargetView(m_normalMap.Get(), &rtvDesc, mhNormalMapCpuRtv);

	rtvDesc.Format = AmbientMapFormat;
	m_d3dDevice->CreateRenderTargetView(m_ambientMap0.Get(), &rtvDesc, mhAmbientMap0CpuRtv);
	m_d3dDevice->CreateRenderTargetView(m_ambientMap1.Get(), &rtvDesc, mhAmbientMap1CpuRtv);
}

void CESSAO::SetPSOs(ID3D12PipelineState* ssaoPso, ID3D12PipelineState* ssaoBlurPso) {
	m_SSAOPso = ssaoPso;
	m_blurPso = ssaoBlurPso;
}

void CESSAO::Resize(UINT newWidth, UINT newHeight) {
	if (m_renderTargetWidth != newWidth || m_renderTargetHeight != newHeight) {
		m_renderTargetWidth = newWidth;
		m_renderTargetHeight = newHeight;

		//We render to ambient map at half the resolution
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = m_renderTargetWidth / 2.0f;
		m_viewport.Height = m_renderTargetHeight / 2.0f;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_scissorRect = {0, 0, (int)m_renderTargetWidth / 2, (int)m_renderTargetHeight / 2};

		BuildResources();
	}
}

void CESSAO::ComputeSSAO(ID3D12GraphicsCommandList* cmdList, CEFrameResource* currFrame, int blurCount) {
	cmdList->RSSetViewports(1, &m_viewport);
	cmdList->RSSetScissorRects(1, &m_scissorRect);

	//We compute the initial SSAO to AmbientMap0

	//Change to RENDER_TARGET
	auto transitionGenericReadRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
		m_ambientMap0.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	cmdList->ResourceBarrier(1, &transitionGenericReadRenderTarget);

	float clearValue[] = {1.0f, 1.0f, 1.0f, 1.0f};
	cmdList->ClearRenderTargetView(mhAmbientMap0CpuRtv, clearValue, 0, nullptr);

	//Specify buffer we are going to render to
	cmdList->OMSetRenderTargets(1, &mhAmbientMap0CpuRtv, true, nullptr);

	//Bind constant buffer for this pass
	auto ssaoCBAddress = currFrame->SSAOCB->Resource()->GetGPUVirtualAddress();
	cmdList->SetGraphicsRootConstantBufferView(0, ssaoCBAddress);
	cmdList->SetGraphicsRoot32BitConstant(1, 0, 0);

	//Bind normal and depth maps
	cmdList->SetGraphicsRootDescriptorTable(2, mhNormalMapGpuSrv);

	//Bind random vector map
	cmdList->SetGraphicsRootDescriptorTable(3, mhRandomVectorMapGpuSrv);

	cmdList->SetPipelineState(m_SSAOPso);

	//Draw fullScreen quad
	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->DrawInstanced(6, 1, 0, 0);

	//Change back to GENERIC_READ so we can read the texture in a shader
	auto transitionRenderTargetGenericRead = CD3DX12_RESOURCE_BARRIER::Transition(m_ambientMap0.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);
	cmdList->ResourceBarrier(1, &transitionRenderTargetGenericRead);

	BlurAmbientMap(cmdList, currFrame, blurCount);
}

void CESSAO::BlurAmbientMap(ID3D12GraphicsCommandList* cmdList, CEFrameResource* currFrame, int blurCount) {
	cmdList->SetPipelineState(m_blurPso);

	auto ssaoCBAddress = currFrame->SSAOCB->Resource()->GetGPUVirtualAddress();
	cmdList->SetGraphicsRootConstantBufferView(0, ssaoCBAddress);

	for (int i = 0; i < blurCount; ++i) {
		BlurAmbientMap(cmdList, true);
		BlurAmbientMap(cmdList, false);
	}
}

void CESSAO::BlurAmbientMap(ID3D12GraphicsCommandList* cmdList, bool horzBlur) {
	ID3D12Resource* output = nullptr;
	CD3DX12_GPU_DESCRIPTOR_HANDLE inputSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE outputRtv;

	/*
	 * Ping-pong the two ambient map textures as we apply
	 * horizontal and vertical blur passes
	 */
	if (horzBlur == true) {
		output = m_ambientMap1.Get();
		inputSrv = mhAmbientMap1GpuSrv;
		outputRtv = mhAmbientMap0CpuRtv;
		cmdList->SetGraphicsRoot32BitConstant(1, 1, 0);
	}
	else {
		output = m_ambientMap0.Get();
		inputSrv = mhAmbientMap1GpuSrv;
		outputRtv = mhAmbientMap0CpuRtv;
		cmdList->SetGraphicsRoot32BitConstant(1, 0, 0);
	}

	auto transitionGenericReadRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(output,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	cmdList->ResourceBarrier(1, &transitionGenericReadRenderTarget);

	float clearValue[] = {1.0f, 1.0f, 1.0f, 1.0f};
	cmdList->ClearRenderTargetView(outputRtv, clearValue, 0, nullptr);

	cmdList->OMSetRenderTargets(1, &outputRtv, true, nullptr);

	//Normal/depth map still bound

	//Bind normal and depth maps
	cmdList->SetGraphicsRootDescriptorTable(2, mhNormalMapGpuSrv);

	//Bind the input ambient map to second texture table
	cmdList->SetGraphicsRootDescriptorTable(3, inputSrv);

	//Draw fullScreen quad.
	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->DrawInstanced(6, 1, 0, 0);

	auto transitionRenderTargetGenericRead = CD3DX12_RESOURCE_BARRIER::Transition(
		output,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_GENERIC_READ);
	cmdList->ResourceBarrier(1, &transitionRenderTargetGenericRead);
}

void CESSAO::BuildResources() {
	//Free old resources if they exist
	m_normalMap = nullptr;
	m_ambientMap0 = nullptr;
	m_ambientMap1 = nullptr;

	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = m_renderTargetWidth;
	texDesc.Height = m_renderTargetHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = NormalMapFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	float normalClearColor[] = {0.0f, 0.0f, 1.0f, 0.0f};
	CD3DX12_CLEAR_VALUE optClear(NormalMapFormat, normalClearColor);
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&optClear,
			IID_PPV_ARGS(&m_normalMap)
		)
	);

	//Ambient occlusion map are at half resolution
	texDesc.Width = m_renderTargetWidth / 2;
	texDesc.Height = m_renderTargetHeight / 2;
	texDesc.Format = AmbientMapFormat;

	float ambientClearColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	optClear = CD3DX12_CLEAR_VALUE(AmbientMapFormat, ambientClearColor);

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&m_ambientMap0)
	));

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&m_ambientMap1)
	));
}

void CESSAO::BuildRandomVectorTexture(ID3D12GraphicsCommandList* cmdList) {
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = 256;
	texDesc.Height = 256;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	auto heapPropsDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto heapPropsUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&heapPropsDefault,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &texDesc,
	                                                   D3D12_RESOURCE_STATE_GENERIC_READ,
	                                                   nullptr,
	                                                   IID_PPV_ARGS(&m_randomVectorMap)));

	/*
	 * In order to copy CPU memory data into out default buffer, we need to create an intermediate upload heap
	 */
	const UINT num2DSubResources = texDesc.DepthOrArraySize * texDesc.MipLevels;
	const UINT uploadBufferSize = GetRequiredIntermediateSize(m_randomVectorMap.Get(), 0, num2DSubResources);

	auto buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapPropsUpload,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_randomVectorMapUploadBuffer.GetAddressOf())
	));

	PackedVector::XMCOLOR initData[256 * 256];
	for (int i = 0; i < 256; ++i) {
		for (int j = 0; j < 256; ++j) {

		}
	}

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = 256 * sizeof(PackedVector::XMCOLOR);
	subResourceData.SlicePitch = subResourceData.RowPitch * 256;

	/*
	 * Schedule to cpy dat ato default resource, and change states
	 * NOTE: mCurrSol is put in GENERIC_READ state so it can be read by shader
	 */
	auto transitionGenericReadCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
		m_randomVectorMap.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	cmdList->ResourceBarrier(1, &transitionGenericReadCopyDest);

	UpdateSubresources(cmdList,
	                   m_randomVectorMap.Get(),
	                   m_randomVectorMapUploadBuffer.Get(),
	                   0,
	                   0,
	                   num2DSubResources,
	                   &subResourceData);

	auto transitionCopyDestGenericRead = CD3DX12_RESOURCE_BARRIER::Transition(
		m_randomVectorMap.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);
	cmdList->ResourceBarrier(1, &transitionCopyDestGenericRead);
}

void CESSAO::BuildOffsetVectors() {
	/*
	 * Start with 14 uniformly distributed vectors. We choose the 8 corners of cube
	 * and the 6 center points along  each cube face. We always alternate the points on opposites sides of cubes.
	 * This way we still get vectors spread out even if we choose to use less than 14 samples
	 */

	//8 cube corners
	m_offsets[0] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
	m_offsets[1] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);

	m_offsets[2] = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
	m_offsets[3] = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);

	m_offsets[4] = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
	m_offsets[5] = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);

	m_offsets[6] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
	m_offsets[7] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

	// 6 centers of cube faces
	m_offsets[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	m_offsets[9] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);

	m_offsets[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	m_offsets[11] = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);

	m_offsets[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	m_offsets[13] = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);

	for (int i = 0; i < 14; ++i) {
		//Create random lengths in [0.25, 1.0]
		float s = CEDX12Manager::RandF(0.25f, 1.0f);

		XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&m_offsets[i]));

		XMStoreFloat4(&m_offsets[i], v);
	}
}
