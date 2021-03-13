#include "CEDX12PickingPlayground.h"

#include <fstream>


#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12PickingPlayground::CEDX12PickingPlayground(): CEDX12Playground() {
}

void CEDX12PickingPlayground::Create() {
	CEDX12Playground::Create();


	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	m_camera.LookAt(
		XMFLOAT3(5.0f, 4.0f, -15.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f));
	LoadTextures();
	// BuildRootSignature
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 0);

		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		// Perfomance TIP: Order from most frequent to least frequent.
		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstantBufferView(1);
		slotRootParameter[2].InitAsShaderResourceView(0, 1);
		slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

		auto staticSamplers = m_dx12manager->GetStaticSamplers();

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		                                        (UINT)staticSamplers.size(), staticSamplers.data(),
		                                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		m_rootSignature = m_dx12manager->CreateRootSignature(&rootSigDesc);
	}
	BuildDescriptorHeaps();
	// BuildShaders
	{
		m_shadersMap["standardVS"] = m_dx12manager->CompileShaders("CEPickingVertexShader.hlsl",
		                                                           nullptr,
		                                                           "VS",
		                                                           // "vs_6_3"
		                                                           "vs_5_1"
		);
		m_shadersMap["opaquePS"] = m_dx12manager->CompileShaders("CEPickingVertexShader.hlsl",
		                                                         nullptr,
		                                                         "PS",
		                                                         // "ps_6_3"
		                                                         "ps_5_1"
		);
	}
	BuildInputLayout();
	BuildShapesGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs(m_rootSignature);

	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());
	m_dx12manager->ExecuteCommandLists();

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12PickingPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
	m_camera.UpdateViewMatrix();

	//Cycle through te circular frame resource array
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	auto fence = m_dx12manager->GetD3D12Fence();
	//Has GPU finished processing commands of current frame resource
	//If not, wait until GPU has completed commands up to this fence point
	if (mCurrFrameResource->Fence != 0 && fence->GetCompletedValue() < mCurrFrameResource->
		Fence) {
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(fence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	AnimateMaterials(gt);
	UpdateObjectCBs(gt);
	UpdateMaterialCBs(gt);
	UpdateMainPassCB(gt);
}

void CEDX12PickingPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);

	auto mScreenViewport = m_dx12manager->GetViewPort();
	auto mScissorRect = m_dx12manager->GetScissorRect();

	auto cmdListAlloc = mCurrFrameResource->commandAllocator;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &mScreenViewport);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	auto trPRT = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trPRT);

	// Clear the back buffer and depth buffer.
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(m_dx12manager->CurrentBackBufferView(),
	                                                            Colors::LightSteelBlue, 0, nullptr);
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(m_dx12manager->DepthStencilView(),
	                                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
	                                                            0, 0, nullptr);

	// Specify the buffers we are going to render to.
	auto rtCBBV = m_dx12manager->CurrentBackBufferView();
	auto rtDsv = m_dx12manager->DepthStencilView();
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &rtCBBV, true,
	                                                         &rtDsv);

	ID3D12DescriptorHeap* descriptorHeaps[] = {m_dx12manager->GetSRVDescriptorHeap().Get()};
	m_dx12manager->GetD3D12CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	auto passCB = mCurrFrameResource->PassStructuredCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	// Bind all the materials used in this scene.  For structured buffers, we can bypass the heap and 
	// set as a root descriptor.
	auto matBuffer = mCurrFrameResource->MaterialBuffer->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootShaderResourceView(2, matBuffer->GetGPUVirtualAddress());

	// Bind all the textures used in this scene.
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(
		3, m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	// Bind all the textures used in this scene.  Observe
	// that we only have to specify the first descriptor in the table.  
	// The root signature knows how many descriptors are expected in the table.
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(
		3, m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Opaque]);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["highlight"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Highlight]);

	// Indicate a state transition on the resource usage.
	auto trRTP = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trRTP);

	// Done recording commands.
	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());

	m_dx12manager->ExecuteCommandLists();

	// Swap the back and front buffers
	auto swapChain = m_dx12manager->GetDXGISwapChain();

	ThrowIfFailed(swapChain->Present(0, 0));

	auto currentBackBufferIndex = m_dx12manager->GetCurrentBackBufferIndex();
	m_dx12manager->SetCurrentBackBufferIndex((currentBackBufferIndex + 1) % CEDX12Manager::GetBackBufferCount());

	m_dx12manager->FlushCommandQueue();
}

void CEDX12PickingPlayground::Resize() {
	CEDX12Playground::Resize();

	m_camera.SetLens(0.25f * Resources::Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);

	BoundingFrustum::CreateFromMatrix(mCamFrustum, m_camera.GetProj());
}

void CEDX12PickingPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	if (key == KeyCode::LButton) {
		mLastMousePos.x = x;
		mLastMousePos.y = y;
		SetCapture(m_dx12manager->GetWindowHandle());
	}
	else if (key == KeyCode::RButton) {
		Pick(x, y);
	}
}

void CEDX12PickingPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
	ReleaseCapture();
}

void CEDX12PickingPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
	if (key == KeyCode::LButton) {
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		m_camera.Pitch(dy);
		m_camera.RotateY(dx);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CEDX12PickingPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12PickingPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyDown(key, keyChar, gt);
	const float dt = gt.DeltaTime();
	switch (key) {
	case KeyCode::A:
		m_camera.Strafe(-40.0f * dt);
		break;
	case KeyCode::D:
		m_camera.Strafe(40.0f * dt);
		break;
	case KeyCode::W:
		m_camera.Walk(40.0f * dt);
		break;
	case KeyCode::S:
		m_camera.Walk(-40.0f * dt);
		break;
	case KeyCode::F:
		mFrustumCullingEnabled = !mFrustumCullingEnabled;
		break;
	}
}

void CEDX12PickingPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12PickingPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);

	auto currObjectCB = mCurrFrameResource->ObjectStructuredCB.get();

	for (auto& ri : mAllRitems) {
		Resources::LitShapesRenderItem* e = static_cast<Resources::LitShapesRenderItem*>(ri.get());
		/*
		 * Only update cbuffer data if constant have changed.
		 * This needs to be tracked per frame resource
		 */
		if (e->NumFramesDirty > 0) {
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			Resources::StructuredObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.WorldViewProjection, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
			objConstants.MaterialIndex = e->Mat->MatCBIndex;

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			//Next FrameResource need to be updated too
			e->NumFramesDirty--;
		}
	}
}

void CEDX12PickingPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);
	XMMATRIX view = m_camera.GetView();
	XMMATRIX proj = m_camera.GetProj();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	auto detView = XMMatrixDeterminant(view);
	XMMATRIX invView = XMMatrixInverse(&detView, view);

	auto detProj = XMMatrixDeterminant(proj);
	XMMATRIX invProj = XMMatrixInverse(&detProj, proj);

	auto detViewProj = XMMatrixDeterminant(viewProj);
	XMMATRIX invViewProj = XMMatrixInverse(&detViewProj, viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = m_camera.GetPosition3f();
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)m_dx12manager->GetWindowWidth(),
	                                        (float)m_dx12manager->GetWindowHeight());
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / m_dx12manager->GetWindowWidth(),
	                                           1.0f / m_dx12manager->GetWindowHeight());
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.AmbientLight = {0.25f, 0.25f, 0.35f, 1.0f};
	mMainPassCB.Lights[0].Direction = {0.57735f, -0.57735f, 0.57735f};
	mMainPassCB.Lights[0].Strength = {0.8f, 0.8f, 0.8f};
	mMainPassCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
	mMainPassCB.Lights[1].Strength = {0.4f, 0.4f, 0.4f};
	mMainPassCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};
	mMainPassCB.Lights[2].Strength = {0.2f, 0.2f, 0.2f};

	auto currPassCB = mCurrFrameResource->PassStructuredCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CEDX12PickingPlayground::UpdateMaterialCBs(const CETimer& gt) {
	CEDX12Playground::UpdateMaterialCBs(gt);
	auto currMaterialBuffer = mCurrFrameResource->MaterialBuffer.get();
	for (auto& e : mMaterials) {
		/*
		 * Only update cbuffer data if constants have changed. If cbuffer data changes,
		 * it needs to be updated for each FrameResource
		 */
		Resources::Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0) {
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			Resources::MaterialData matData;
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

			currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			//Next FrameResource need to be updated;
			mat->NumFramesDirty--;
		}
	}
}

void CEDX12PickingPlayground::BuildPSOs(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature) {
	// CEDX12Playground::BuildPSOs(rootSignature);
	auto d3dDevice = m_dx12manager->GetD3D12Device();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	/*
	 * PSO for opaque objects
	 */
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = {m_inputLayout.data(), (UINT)m_inputLayout.size()};
	opaquePsoDesc.pRootSignature = rootSignature.Get();
	opaquePsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shadersMap["standardVS"]->GetBufferPointer()),
		m_shadersMap["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_shadersMap["opaquePS"]->GetBufferPointer()),
		m_shadersMap["opaquePS"]->GetBufferSize()
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

	// PSO for highlight objects
	D3D12_GRAPHICS_PIPELINE_STATE_DESC highlightPsoDesc = opaquePsoDesc;

	/*
	 * Change depth test from < to <= so that if we draw same triangle twice itr will still pass the depth test. This is needed because we redraw picked triangle with a different material to highlight it.
	 * If we do not use <= the triangle will fail the depth test, second time we try and draw it
	 */
	highlightPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//Standard transparency blending, like with water PSOs
	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	transparencyBlendDesc.BlendEnable = true;
	transparencyBlendDesc.LogicOpEnable = false;
	transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	highlightPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&highlightPsoDesc, IID_PPV_ARGS(&mPSOs["highlight"])));

}

void CEDX12PickingPlayground::LoadTextures() {
	auto defaultDiffuseTex = m_dx12manager->LoadTextureFromFile("white1x1.dds", "defaultDiffuseTex");
	mTextures[defaultDiffuseTex->Name] = std::move(defaultDiffuseTex);
}

void CEDX12PickingPlayground::BuildDescriptorHeaps() {
	m_dx12manager->CreateSRVDescriptorHeap(1);

	auto srvSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		m_dx12manager->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	auto d3dDevice = m_dx12manager->GetD3D12Device();

	auto defaultDiffuseTex = mTextures["defaultDiffuseTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = defaultDiffuseTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = defaultDiffuseTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	d3dDevice->CreateShaderResourceView(defaultDiffuseTex.Get(), &srvDesc, hDescriptor);
}

void CEDX12PickingPlayground::BuildShapesGeometry() {
	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream modelsPathStream;
	modelsPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Models\\" <<
		"car.txt";
	auto modelPath = modelsPathStream.str();
	std::ifstream fin(modelPath);

	if (!fin) {
		MessageBox(0, L"Models/car.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	XMFLOAT3 vMinf3(+Resources::Infinity, +Resources::Infinity, +Resources::Infinity);
	XMFLOAT3 vMaxf3(-Resources::Infinity, -Resources::Infinity, -Resources::Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	std::vector<Resources::CENormalTextureVertex> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i) {
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

		XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);

		vertices[i].TexCoord = {0.0f, 0.0f};

		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	BoundingBox bounds;
	XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	std::vector<std::int32_t> indices(3 * tcount);
	for (UINT i = 0; i < tcount; ++i) {
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalTextureVertex);

	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::int32_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "carGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalTextureVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	submesh.Bounds = bounds;

	geo->DrawArgs["car"] = submesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12PickingPlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<Resources::CEFrameResource>(m_dx12manager->GetD3D12Device().Get(),
		                                                                       1,
		                                                                       (UINT)mAllRitems.size(),
		                                                                       (UINT)mMaterials.size(),
		                                                                       0,
		                                                                       Resources::WavesNormalTextureVertex,
		                                                                       false));
	}
}

void CEDX12PickingPlayground::BuildMaterials() {
	auto gray0 = std::make_unique<Resources::Material>();
	gray0->Name = "gray0";
	gray0->MatCBIndex = 0;
	gray0->DiffuseSrvHeapIndex = 0;
	gray0->DiffuseAlbedo = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	gray0->FresnelR0 = XMFLOAT3(0.04f, 0.04f, 0.04f);
	gray0->Roughness = 0.0f;

	auto highlight0 = std::make_unique<Resources::Material>();
	highlight0->Name = "highlight0";
	highlight0->MatCBIndex = 1;
	highlight0->DiffuseSrvHeapIndex = 0;
	highlight0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.6f);
	highlight0->FresnelR0 = XMFLOAT3(0.06f, 0.06f, 0.06f);
	highlight0->Roughness = 0.0f;


	mMaterials["gray0"] = std::move(gray0);
	mMaterials["highlight0"] = std::move(highlight0);
}

void CEDX12PickingPlayground::BuildRenderItems() {
	auto carRitem = std::make_unique<Resources::LitShapesRenderItem>();
	XMStoreFloat4x4(&carRitem->World, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	XMStoreFloat4x4(&carRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	carRitem->ObjCBIndex = 0;
	carRitem->Mat = mMaterials["gray0"].get();
	carRitem->Geo = mGeometries["carGeo"].get();
	carRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	carRitem->IndexCount = carRitem->Geo->DrawArgs["car"].IndexCount;
	carRitem->StartIndexLocation = carRitem->Geo->DrawArgs["car"].StartIndexLocation;
	carRitem->BaseVertexLocation = carRitem->Geo->DrawArgs["car"].BaseVertexLocation;
	carRitem->Bounds = carRitem->Geo->DrawArgs["car"].Bounds;
	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(carRitem.get());

	auto pickedRitem = std::make_unique<Resources::LitShapesRenderItem>();
	pickedRitem->World = Resources::MatrixIdentity4X4();
	pickedRitem->TexTransform = Resources::MatrixIdentity4X4();
	pickedRitem->ObjCBIndex = 1;
	pickedRitem->Mat = mMaterials["highlight0"].get();
	pickedRitem->Geo = mGeometries["carGeo"].get();
	pickedRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Picked triangle is not visible until one is picked.
	pickedRitem->Visible = false;

	// DrawCall parameters are filled out when a triangle is picked.
	pickedRitem->IndexCount = 0;
	pickedRitem->StartIndexLocation = 0;
	pickedRitem->BaseVertexLocation = 0;
	mPickedRitem = pickedRitem.get();
	mRitemLayer[(int)Resources::RenderLayer::Highlight].push_back(pickedRitem.get());


	mAllRitems.push_back(std::move(carRitem));
	mAllRitems.push_back(std::move(pickedRitem));
}

void CEDX12PickingPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                                              std::vector<Resources::RenderItem*>& ritems) const {
	UINT objCBByteSize = (sizeof(Resources::StructuredObjectConstants) + 255) & ~255;

	auto objectCB = mCurrFrameResource->ObjectStructuredCB->Resource();

	for (size_t i = 0; i < ritems.size(); ++i) {
		auto ri = static_cast<Resources::LitShapesRenderItem*>(ritems[i]);

		if (ri->Visible == false)
			continue;

		auto vbv = ri->Geo->VertexBufferView();
		auto ibv = ri->Geo->IndexBufferView();

		cmdList->IASetVertexBuffers(0, 1, &vbv);
		cmdList->IASetIndexBuffer(&ibv);
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

void CEDX12PickingPlayground::Pick(int sx, int sy) {
	XMFLOAT4X4 P = m_camera.GetProj4x4f();

	//Compute picking ray in view space
	float vx = (+2.0f * sx / m_dx12manager->GetWindowWidth() - 1.0f) / P(0, 0);
	float vy = (-2.0f * sy / m_dx12manager->GetWindowHeight() + 1.0f) / P(1, 1);

	//Ray definition in view space
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	XMMATRIX V = m_camera.GetView();
	auto detV = XMMatrixDeterminant(V);
	XMMATRIX invView = XMMatrixInverse(&detV, V);

	//Assume nothins is picked to start, so picked render-item is invisible
	static_cast<Resources::LitShapesRenderItem*>(mPickedRitem)->Visible = false;

	//Check if we picked an opaque render item. A real app might keep a separate "picking list" of objects that can be selected
	for (auto e : mRitemLayer[(int)Resources::RenderLayer::Opaque]) {
		auto ri = static_cast<Resources::LitShapesRenderItem*>(e);
		auto geo = ri->Geo;

		//Skip invisible render-items;
		if (ri->Visible == false)
			continue;

		XMMATRIX W = XMLoadFloat4x4(&ri->World);

		auto detWorld = XMMatrixDeterminant(W);
		XMMATRIX invWorld = XMMatrixInverse(&detWorld, W);

		//Transform ray to vi space of Mesh
		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
		rayDir = XMVector3TransformNormal(rayDir, toLocal);

		//Make ray direction unit length for intersection tests
		rayDir = XMVector3Normalize(rayDir);

		/*
		 * If we hit bounding box of mesh, then we might have picked a Mesh triangle
		 * So do the ray/triangle tests
		 *
		 * If we did not hit bounding box then it is impossible that we hit Mesh, so do not waste effort doing ray/triangle tests
		 */
		float tMin = 0.0f;
		if (ri->Bounds.Intersects(rayOrigin, rayDir, tMin)) {
			//NOTE: for demo we know what to cast vertex/index data to. If we were mixing formats, some metadata would be needed to figure out what to cast it to
			auto vertices = (Resources::CENormalTextureVertex*)geo->VertexBufferCPU->GetBufferPointer();
			auto indices = (std::uint32_t*)geo->IndexBufferCPU->GetBufferPointer();

			UINT triCount = ri->IndexCount / 3;

			//Find nearest ray/triangle intersection;
			tMin = Resources::Infinity;
			for (UINT i = 0; i < triCount; ++i) {
				//Indices for this triangle
				UINT i0 = indices[i * 3 + 0];
				UINT i1 = indices[i * 3 + 1];
				UINT i2 = indices[i * 3 + 2];

				//Vertices for this triangle
				XMVECTOR v0 = XMLoadFloat3(&vertices[i0].Pos);
				XMVECTOR v1 = XMLoadFloat3(&vertices[i1].Pos);
				XMVECTOR v2 = XMLoadFloat3(&vertices[i2].Pos);

				//We have to iterate over all triangles in order to find nearest intersection
				float t = 0.0f;
				if (TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, t)) {
					if (t < tMin) {
						//This is new nearest picked triangle
						tMin = t;
						UINT pickedTriangle = i;

						static_cast<Resources::LitShapesRenderItem*>(mPickedRitem)->Visible = true;
						static_cast<Resources::LitShapesRenderItem*>(mPickedRitem)->IndexCount = 3;
						static_cast<Resources::LitShapesRenderItem*>(mPickedRitem)->BaseVertexLocation = 0;

						//Picked render item need same world matrix as object picked
						static_cast<Resources::LitShapesRenderItem*>(mPickedRitem)->World = ri->World;
						static_cast<Resources::LitShapesRenderItem*>(mPickedRitem)->NumFramesDirty = gNumFrameResources;

						//Offset to picked triangle in mesh index buffer
						static_cast<Resources::LitShapesRenderItem*>(mPickedRitem)->StartIndexLocation = 3 *
							pickedTriangle;
					}
				}
			}
		}
	}
}
