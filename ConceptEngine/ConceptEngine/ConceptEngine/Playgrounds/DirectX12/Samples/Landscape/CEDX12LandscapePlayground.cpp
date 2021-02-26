#include "CEDX12LandscapePlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"
#include "../../../../../ConceptEngineFramework/Tools/CEUtils.h"

using namespace ConceptEngine::Playgrounds::DirectX12;
using namespace ConceptEngineFramework::Game;

CEDX12LandscapePlayground::CEDX12LandscapePlayground() {
	mTheta = 1.5f * XM_PI;
	mPhi = XM_PIDIV2 - 0.1f;
	mRadius = 50.0f;
}

void CEDX12LandscapePlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	//Create basic waves
	m_waves = std::make_unique<Resources::CEWaves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	//Create Root Signature

	//Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	//Create root ConstantBufferView
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	//root signature is an array of root parameters
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(2,
	                                              slotRootParameter,
	                                              0,
	                                              nullptr,
	                                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//Create root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	m_rootSignature = m_dx12manager->CreateRootSignature(&rootSignatureDesc);

	//Create Shaders and input layout
	BuildShadersAndInputLayout();

	//Create Landscape geometry
	BuildLandscapeGeometry();

	//Create geometry buffers
	BuildGeometryBuffers();

	//Create Render items
	BuildRenderItems();
	BuildRenderItems();

	//Create Frame resources
	BuildFrameResources();

	//Create Pipeline State Objects
	BuildPSOs();

	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	auto commandList = m_dx12manager->GetD3D12CommandList();
	// Execute the initialization commands.
	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* cmdsLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	m_dx12manager->FlushCommandQueue();
}

void CEDX12LandscapePlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
	UpdateCamera(gt);

	//Cycle through circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	//Has GPU finished processing commands of current frame resource
	//If not, wait until GPU has completed commands up to this fence point
	if (mCurrFrameResource->Fence != 0 && m_dx12manager->GetD3D12Fence()->GetCompletedValue() < mCurrFrameResource->
		Fence) {
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateObjectCBs(gt);
	UpdateMainPassCB(gt);
	UpdateWaves(gt);
}

void CEDX12LandscapePlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12LandscapePlayground::Resize() {
	CEDX12Playground::Resize();
}

void CEDX12LandscapePlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12LandscapePlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
	ReleaseCapture();
}

void CEDX12LandscapePlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
	static const float Pi = 3.1415926535f;
	if (key == KeyCode::LButton) {

	}
	else if (key == KeyCode::RButton) {

	}
}

void CEDX12LandscapePlayground::OnKeyUp(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyUp(key, keyChar);
}

void CEDX12LandscapePlayground::OnKeyDown(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyDown(key, keyChar);
	spdlog::info("KEYBOARD DOWN KEY: {}", keyChar);
	if (key == KeyCode::D1) {
		mIsWireframe = true;
	}
	else {
		mIsWireframe = false;
	}
}

void CEDX12LandscapePlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12LandscapePlayground::UpdateCamera(const CETimer& gt) {
	//Convert spherical to cartesian coordinates
	mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	mEyePos.y = mRadius * sinf(mPhi) * sinf(mTheta);
	mEyePos.z = mRadius * cosf(mPhi);

	//Build view matrix
	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void CEDX12LandscapePlayground::UpdateObjectCBs(const CETimer& gt) {
	auto currentObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& e : mAllRitems) {
		Resources::LandscapeRenderItem* p = static_cast<Resources::LandscapeRenderItem*>(e.get());
		//Only update cbuffer data if constants have changed
		//this need to be tracked per frame resource
		if (p->NumFramesDirty > 0) {
			XMMATRIX world = XMLoadFloat4x4(&p->World);

			Resources::CEObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.WorldViewProjection, XMMatrixTranspose(world));

			currentObjectCB->CopyData(p->ObjCBIndex, objConstants);

			//Next FrameResource need to be updated too.
			p->NumFramesDirty--;

			spdlog::info("Update Object CB: Pointer->{}, Derived->{}",
			             static_cast<Resources::LandscapeRenderItem*>(e.get())->NumFramesDirty, p->NumFramesDirty);
		}
	}
}

void CEDX12LandscapePlayground::UpdateMainPassCB(const CETimer& gt) {
	auto width = m_dx12manager->GetWindowWidth();
	auto height = m_dx12manager->GetWindowHeight();

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	auto viewDeterminant = XMMatrixDeterminant(view);
	auto projDeterminant = XMMatrixDeterminant(proj);
	auto viewProjDeterminant = XMMatrixDeterminant(viewProj);

	XMMATRIX invView = XMMatrixInverse(&viewDeterminant, view);
	XMMATRIX invProj = XMMatrixInverse(&projDeterminant, proj);
	XMMATRIX invViewProj = XMMatrixInverse(&viewProjDeterminant, viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));

	mMainPassCB.EyePosW = mEyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)width, (float)height);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / width, 1.0f / height);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CEDX12LandscapePlayground::UpdateWaves(const CETimer& gt) {
	//every quarter second, generate a random wave
	static float t_base = 0.0f;
	if ((gt.TotalTime() - t_base) >= 0.25f) {
		t_base += 0.25f;

		int i = m_dx12manager->Rand(4, m_waves->RowCount() - 5);
		int j = m_dx12manager->Rand(4, m_waves->ColumnCount() - 5);

		float r = m_dx12manager->RandF(0.2f, 0.5f);

		m_waves->Disturb(i, j, r);
	}

	//Update wave simulation
	m_waves->Update(gt.DeltaTime());

	//Update wave vertex buffer with new solution
	auto currWavesVB = mCurrFrameResource->WavesVB.get();
	for (int i = 0; i < m_waves->VertexCount(); ++i) {
		Resources::CEVertex v;

		v.Pos = m_waves->Position(i);
		v.Color = XMFLOAT4(Colors::Blue);

		currWavesVB->CopyData(i, v);
	}

	//set dynamic VB of wave RenderItem to current frame VB;
	static_cast<Resources::LandscapeRenderItem*>(mWavesRitem)->Geo->VertexBufferGPU = currWavesVB->Resource();
}

//TODO: Implement Shader Model 6 usage with DirectXShaderCompiler
//TODO: References: https://asawicki.info/news_1719_two_shader_compilers_of_direct3d_12
//TODO: References: https://github.com/Microsoft/DirectXShaderCompiler
void CEDX12LandscapePlayground::BuildShadersAndInputLayout() {
	m_shadersMap["VS"] = m_dx12manager->CompileShaders("CELandscapeVertexShader.hlsl",
	                                                   nullptr,
	                                                   "VS",
	                                                   // "vs_6_3"
	                                                   "vs_5_0"
	);
	m_shadersMap["PS"] = m_dx12manager->CompileShaders("CELandscapePixelShader.hlsl",
	                                                   nullptr,
	                                                   "PS",
	                                                   // "ps_6_3"
	                                                   "ps_5_0"
	);

	m_inputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void CEDX12LandscapePlayground::BuildLandscapeGeometry() {
	GeometryGenerator geometry;
	GeometryGenerator::MeshData grid = geometry.CreateGrid(160.0f, 160.0f, 50, 50);

	/*
	 * Extract vertex elements we are interested and apply height function to each vertex
	 * In addition, color vertices based on their height so we have sandy looking beaches, grassy low hills, and snow mountain peaks
	 */

	std::vector<Resources::CEVertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i) {
		auto& p = grid.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Pos.y = GetHillsHeight(p.x, p.z);

		//Color vertex based on its height
		if (vertices[i].Pos.y < -10.0f) {
			//Sandy beach color
			vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (vertices[i].Pos.y < 5.0f) {
			//Light yellow green
			vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (vertices[i].Pos.y < 12.0f) {
			//Dark yellow-green
			vertices[i].Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (vertices[i].Pos.y < 20.0f) {
			//Dark brown
			vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else {
			//White snow
			vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CEVertex);

	std::vector<std::uint16_t> indices = grid.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "landscapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CEVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry subMesh;
	subMesh.IndexCount = (UINT)indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = subMesh;

	m_geometries["landscapeGeo"] = std::move(geo);
}

void CEDX12LandscapePlayground::BuildGeometryBuffers() {
	std::vector<std::uint16_t> indices(3 * m_waves->TriangleCount()); //3 indices per face
	assert(m_waves->VertexCount() < 0x0000ffff);

	//Iterate over each quad
	int m = m_waves->RowCount();
	int n = m_waves->ColumnCount();
	int k = 0;
	for (int i = 0; i < m - 1; ++i) {
		for (int j = 0; j < n - 1; ++j) {
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; //next quad;
		}
	}

	UINT vbByteSize = m_waves->VertexCount() * sizeof(Resources::CEVertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "waterGeo";

	//Set dynamically
	geo->VertexBufferCPU = nullptr;
	geo->VertexBufferGPU = nullptr;

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CEVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry subMesh;
	subMesh.IndexCount = (UINT)indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = subMesh;
	m_geometries["waterGeo"] = std::move(geo);
}

//TODO: Add RayTraced StateObject
void CEDX12LandscapePlayground::BuildPSOs() {
	auto d3dDevice = m_dx12manager->GetD3D12Device();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	/*
	 * PSO for opaque objects
	 */
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = {m_inputLayout.data(), (UINT)m_inputLayout.size()};
	opaquePsoDesc.pRootSignature = m_rootSignature.Get();
	opaquePsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shadersMap["VS"]->GetBufferPointer()),
		m_shadersMap["VS"]->GetBufferSize()
	};
	opaquePsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_shadersMap["PS"]->GetBufferPointer()),
		m_shadersMap["PS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = m_dx12manager->GetBackBufferFormat();
	opaquePsoDesc.SampleDesc.Count = m_dx12manager->GetM4XMSAAState() ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m_dx12manager->GetM4XMSAAState() ? (m_dx12manager->GetM4XMSAAQuality() - 1) : 0;
	opaquePsoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));

	/*
	 * PSO for WireFrame Objects
	 */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireFramePsoDesc = opaquePsoDesc;
	opaqueWireFramePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(
		d3dDevice->CreateGraphicsPipelineState(
			&opaqueWireFramePsoDesc,
			IID_PPV_ARGS(&mPSOs["opaque_wireframe"])
		)
	);
}

void CEDX12LandscapePlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(
			std::make_unique<Resources::CEFrameResource>(
				m_dx12manager->GetD3D12Device().Get(),
				1,
				(UINT)mAllRitems.size(),
				m_waves->VertexCount()
			)
		);
	}
}

void CEDX12LandscapePlayground::BuildRenderItems() {
	auto wavesRitem = std::make_unique<Resources::LandscapeRenderItem>();
	wavesRitem->World = Resources::MatrixIdentity4X4();
	wavesRitem->ObjCBIndex = 0;
	wavesRitem->Geo = m_geometries["waterGeo"].get();
	wavesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
	wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mWavesRitem = wavesRitem.get();

	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(wavesRitem.get());

	auto gridRitem = std::make_unique<Resources::LandscapeRenderItem>();
	gridRitem->World = Resources::MatrixIdentity4X4();
	gridRitem->ObjCBIndex = 1;
	gridRitem->Geo = m_geometries["landscapeGeo"].get();
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(gridRitem.get());

	mAllRitems.push_back(std::move(wavesRitem));
	mAllRitems.push_back(std::move(gridRitem));
}

//TODO: Move to CEDX12Manager
void CEDX12LandscapePlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                                                std::vector<Resources::RenderItem*>& ritems) const {
	UINT objCBByteSize = (sizeof(Resources::CEObjectConstants) + 255) & ~255;

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();

	//For each render item...
	for (size_t i = 0; i < ritems.size(); ++i) {
		auto ri = static_cast<Resources::LandscapeRenderItem*>(ritems[i]);

		auto riVertexBufferView = ri->Geo->VertexBufferView();
		auto riIndexBufferView = ri->Geo->IndexBufferView();

		cmdList->IASetVertexBuffers(0, 1, &riVertexBufferView);
		cmdList->IASetIndexBuffer(&riIndexBufferView);
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress();
		objCBAddress += ri->ObjCBIndex * objCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

float CEDX12LandscapePlayground::GetHillsHeight(float x, float z) const {
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 CEDX12LandscapePlayground::GetHillsNormal(float x, float z) const {
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z)
	);

	XMVECTOR uintNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, uintNormal);

	return n;
}
