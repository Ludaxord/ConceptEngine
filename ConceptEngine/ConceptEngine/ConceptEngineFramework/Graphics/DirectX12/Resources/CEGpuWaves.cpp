#include "CEGpuWaves.h"

#include "../../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CEGpuWaves::CEGpuWaves(ID3D12Device* device,
                       ID3D12GraphicsCommandList* cmdList,
                       int m,
                       int n,
                       float dx,
                       float dt,
                       float speed,
                       float damping) {
	m_d3dDevice = device;

	m_numRows = m;
	m_numCols = n;

	assert((m * n) % 256 == 0);

	m_vertexCount = m * n;
	m_triangleCount = (m - 1) * (n - 1) * 2;

	m_timeStep = dt;
	m_spatialStep = dx;

	float d = damping * dt + 2.0f;
	float e = (speed * speed) * (dt * dt) / (dx * dx);
	mK[0] = (damping * dt - 2.0f) / d;
	mK[1] = (4.0f - 8.0f * e) / d;
	mK[2] = (2.0f * e) / d;

	BuildResources(cmdList);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CEGpuWaves::DisplacementMap() const {
	return mCurrSolSrv;
}

UINT CEGpuWaves::DescriptorCount() const {
	//Number of descriptors in heap to reserve for GpuWaves
	return 6;
}

void CEGpuWaves::BuildResources(ID3D12GraphicsCommandList* cmdList) {
	//All textures for wave simulation will be bound as a shader resource and unordered access view at some point since we ping pong buffers
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = m_numCols;
	texDesc.Height = m_numRows;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&defaultHeap,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &texDesc,
	                                                   D3D12_RESOURCE_STATE_COMMON,
	                                                   nullptr,
	                                                   IID_PPV_ARGS(&mPrevSol)));

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&defaultHeap,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &texDesc,
	                                                   D3D12_RESOURCE_STATE_COMMON,
	                                                   nullptr,
	                                                   IID_PPV_ARGS(&mCurrSol)));

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&defaultHeap,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &texDesc,
	                                                   D3D12_RESOURCE_STATE_COMMON,
	                                                   nullptr,
	                                                   IID_PPV_ARGS(&mNextSol)));


	//In order to copy CPU memory data into our default buffer, we need to create an intermediate upload heap

	const UINT num2DSubresources = texDesc.DepthOrArraySize * texDesc.MipLevels;
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mCurrSol.Get(), 0, num2DSubresources);

	auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto bufferResource = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&uploadHeap,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &bufferResource,
	                                                   D3D12_RESOURCE_STATE_GENERIC_READ,
	                                                   nullptr,
	                                                   IID_PPV_ARGS(mPrevUploadBuffer.GetAddressOf())
		)
	);

	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&uploadHeap,
	                                                   D3D12_HEAP_FLAG_NONE,
	                                                   &bufferResource,
	                                                   D3D12_RESOURCE_STATE_GENERIC_READ,
	                                                   nullptr,
	                                                   IID_PPV_ARGS(mCurrUploadBuffer.GetAddressOf())
		)
	);

	//Describe data we want to copy into default buffer
	std::vector<float> initData(m_numRows * m_numCols, 0.0f);
	for (int i = 0; i < initData.size(); ++i)
		initData[i] = 0.0f;

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData.data();
	subResourceData.RowPitch = m_numCols * sizeof(float);
	subResourceData.SlicePitch = subResourceData.RowPitch * m_numRows;

	/*
	 * Schedule to copy data to default resource, and change states.
	 * Note that mCurrSol is put in GENERIC_READ state so it can be read by shader
	 */


	D3D12_RESOURCE_BARRIER prev_common2copydest = CD3DX12_RESOURCE_BARRIER::Transition(mPrevSol.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	D3D12_RESOURCE_BARRIER prev_copydest2read = CD3DX12_RESOURCE_BARRIER::Transition(mPrevSol.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	cmdList->ResourceBarrier(1, &prev_common2copydest);
	UpdateSubresources(cmdList, mPrevSol.Get(), mPrevUploadBuffer.Get(), 0, 0, num2DSubresources, &subResourceData);
	cmdList->ResourceBarrier(1, &prev_copydest2read);

	D3D12_RESOURCE_BARRIER curr_common2copydest = CD3DX12_RESOURCE_BARRIER::Transition(mCurrSol.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	D3D12_RESOURCE_BARRIER curr_copydest2read = CD3DX12_RESOURCE_BARRIER::Transition(mCurrSol.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	cmdList->ResourceBarrier(1, &curr_common2copydest);
	UpdateSubresources(cmdList, mCurrSol.Get(), mCurrUploadBuffer.Get(), 0, 0, num2DSubresources, &subResourceData);
	cmdList->ResourceBarrier(1, &curr_copydest2read);

	D3D12_RESOURCE_BARRIER next_common2ua = CD3DX12_RESOURCE_BARRIER::Transition(mNextSol.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cmdList->ResourceBarrier(1, &next_common2ua);
}

void CEGpuWaves::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
                                  CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
                                  UINT descriptorSize) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	m_d3dDevice->CreateShaderResourceView(mPrevSol.Get(), &srvDesc, hCpuDescriptor);
	m_d3dDevice->CreateShaderResourceView(mCurrSol.Get(), &srvDesc, hCpuDescriptor.Offset(1, descriptorSize));
	m_d3dDevice->CreateShaderResourceView(mNextSol.Get(), &srvDesc, hCpuDescriptor.Offset(1, descriptorSize));

	m_d3dDevice->CreateUnorderedAccessView(mPrevSol.Get(), nullptr, &uavDesc, hCpuDescriptor.Offset(1, descriptorSize));
	m_d3dDevice->CreateUnorderedAccessView(mCurrSol.Get(), nullptr, &uavDesc, hCpuDescriptor.Offset(1, descriptorSize));
	m_d3dDevice->CreateUnorderedAccessView(mNextSol.Get(), nullptr, &uavDesc, hCpuDescriptor.Offset(1, descriptorSize));

	//Save references to GPU Descriptors
	mPrevSolSrv = hGpuDescriptor;
	mCurrSolSrv = hGpuDescriptor.Offset(1, descriptorSize);
	mNextSolSrv = hGpuDescriptor.Offset(1, descriptorSize);

	mPrevSolUav = hGpuDescriptor.Offset(1, descriptorSize);
	mCurrSolUav = hGpuDescriptor.Offset(1, descriptorSize);
	mNextSolUav = hGpuDescriptor.Offset(1, descriptorSize);

	mPrevSol->SetName(L"mPrevSol Resource");
	mCurrSol->SetName(L"mCurrSol Resource");
	mNextSol->SetName(L"mNextSol Resource");

	mCurrUploadBuffer->SetName(L"mCurrUploadBuffer Resource");
	mPrevUploadBuffer->SetName(L"mPrevUploadBuffer Resource");
}

void CEGpuWaves::Update(const CETimer& gt,
                        ID3D12GraphicsCommandList* cmdList,
                        ID3D12RootSignature* rootSignature,
                        ID3D12PipelineState* Pso) {
	static float t = 0.0f;

	//Accumulate time
	t += gt.DeltaTime();

	cmdList->SetPipelineState(Pso);
	cmdList->SetComputeRootSignature(rootSignature);

	//Only update simulation at specified time step
	if (t >= m_timeStep) {
		//Set update constants
		cmdList->SetComputeRoot32BitConstants(0, 3, mK, 0);

		cmdList->SetComputeRootDescriptorTable(1, mPrevSolUav);
		cmdList->SetComputeRootDescriptorTable(2, mCurrSolUav);
		cmdList->SetComputeRootDescriptorTable(3, mNextSolUav);

		//How many groups do we need to dispatch to cover wave grid
		//note that m_numRows and m_numCols should be divisible by 16
		//so there is no remainder
		UINT numGroupsX = (m_numCols + 15) / 16;
		UINT numGroupsY = (m_numRows + 15) / 16;
		cmdList->Dispatch(numGroupsX, numGroupsY, 1);

		//Ping pong buffers in preparation for next update
		//previous solution is no longer needed and becomes the target of next solution in next update
		//current solution becomes previous solution.
		//next solution becomes current solution

		auto resTemp = mPrevSol;
		mPrevSol = mCurrSol;
		mCurrSol = mNextSol;
		mNextSol = resTemp;

		auto srvTemp = mPrevSolSrv;
		mPrevSolSrv = mCurrSolSrv;
		mCurrSolSrv = mNextSolSrv;
		mNextSolSrv = srvTemp;

		auto uavTemp = mPrevSolUav;
		mPrevSolUav = mCurrSolUav;
		mCurrSolUav = mNextSolUav;
		mNextSolUav = uavTemp;

		t = 0.0f; //reset time

		//current solution need to be able to read by vertex shader, so change it state to GENERIC_READ
		auto currSolTransition = CD3DX12_RESOURCE_BARRIER::Transition(mCurrSol.Get(),
		                                                              D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		                                                              D3D12_RESOURCE_STATE_GENERIC_READ);
		cmdList->ResourceBarrier(1, &currSolTransition);
		D3D12_RESOURCE_BARRIER next_read2ua = CD3DX12_RESOURCE_BARRIER::Transition(mNextSol.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		cmdList->ResourceBarrier(1, &next_read2ua);
	}
}

void CEGpuWaves::Disturb(ID3D12GraphicsCommandList* cmdList,
                         ID3D12RootSignature* rootSignature,
                         ID3D12PipelineState* pso,
                         UINT i,
                         UINT j,
                         float magnitude) {
	cmdList->SetPipelineState(pso);
	cmdList->SetComputeRootSignature(rootSignature);

	//Set disturb constants
	UINT distrubIndex[2] = {j, i};
	cmdList->SetComputeRoot32BitConstants(0, 1, &magnitude, 3);
	cmdList->SetComputeRoot32BitConstants(0, 2, distrubIndex, 4);

	/*
	 * Current solution is in GENERIC_READ state so it can be read by vertex shader
	 * Change it to UNORDERED_ACCESS for compute shader. Note that UAV can still be read in a compute shader
	 */
	auto currTrReadUA = CD3DX12_RESOURCE_BARRIER::Transition(mCurrSol.Get(), D3D12_RESOURCE_STATE_GENERIC_READ,
	                                                         D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cmdList->ResourceBarrier(1, &currTrReadUA);

	//One thread group kicks off one thread, which displaces the height of one vertex and its neighbors
	cmdList->Dispatch(1, 1, 1);

	D3D12_RESOURCE_BARRIER ua2read = CD3DX12_RESOURCE_BARRIER::Transition(mCurrSol.Get(),
	                                                                      D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
	                                                                      D3D12_RESOURCE_STATE_GENERIC_READ);
	cmdList->ResourceBarrier(1, &ua2read);
}

int CEGpuWaves::RowCount() const {
	return m_numCols;
}

int CEGpuWaves::ColumnCount() const {
	return m_numCols;
}

int CEGpuWaves::VertexCount() const {
	return m_vertexCount;
}

int CEGpuWaves::TriangleCount() const {
	return m_triangleCount;
}

float CEGpuWaves::Width() const {
	return m_numCols * m_spatialStep;
}

float CEGpuWaves::Depth() const {
	return m_numRows * m_spatialStep;
}

float CEGpuWaves::SpatialStep() const {
	return m_spatialStep;
}

void CEGpuWaves::Update(float dt) {
}

void CEGpuWaves::Disturb(int i, int j, float magnitude) {
}
