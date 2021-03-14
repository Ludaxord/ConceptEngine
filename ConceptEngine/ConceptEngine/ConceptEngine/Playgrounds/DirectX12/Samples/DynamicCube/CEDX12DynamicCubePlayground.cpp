#include "CEDX12DynamicCubePlayground.h"

#include <fstream>


#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12DynamicCubePlayground::CEDX12DynamicCubePlayground() : CEDX12Playground() {
}

void CEDX12DynamicCubePlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	m_camera.SetPosition(0.0f, 2.0f, -15.0f);

	BuildCubeFaceCamera(0.0f, 2.0f, 0.0f);

	m_dynamicCubeMap = std::make_unique<CECubeRenderTarget>(m_dx12manager->GetD3D12Device().Get(), Resources::CubeMapSize, Resources::CubeMapSize, DXGI_FORMAT_R8G8B8A8_UNORM);

	LoadTextures();
	// BuildRootSignature
	{
		CD3DX12_DESCRIPTOR_RANGE texTable0;
		texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

		CD3DX12_DESCRIPTOR_RANGE texTable1;
		texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1, 0);

		//Root parameter can be a table, root descriptor or root constants
		CD3DX12_ROOT_PARAMETER slotRootParameter[5];

		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstantBufferView(1);
		slotRootParameter[2].InitAsShaderResourceView(0, 1);
		slotRootParameter[3].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

		auto staticSamplers = m_dx12manager->GetStaticSamplers();

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(5, slotRootParameter,
		                                        (UINT)staticSamplers.size(), staticSamplers.data(),
		                                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		m_rootSignature = m_dx12manager->CreateRootSignature(&rootSigDesc);
	}
	BuildDescriptorHeaps();
	// BuildShaders
	{
		m_shadersMap["standardVS"] = m_dx12manager->CompileShaders("CEBaseVertexShader.hlsl",
		                                                           nullptr,
		                                                           "VS",
		                                                           // "vs_6_3"
		                                                           "vs_5_1"
		);
		m_shadersMap["opaquePS"] = m_dx12manager->CompileShaders("CEBasePixelShader.hlsl",
		                                                         nullptr,
		                                                         "PS",
		                                                         // "ps_6_3"
		                                                         "ps_5_1"
		);
		m_shadersMap["skyVS"] = m_dx12manager->CompileShaders("CECubeMapVertexShader.hlsl",
		                                                      nullptr,
		                                                      "VS",
		                                                      // "vs_6_3"
		                                                      "vs_5_1"
		);
		m_shadersMap["skyPS"] = m_dx12manager->CompileShaders("CECubeMapPixelShader.hlsl",
		                                                      nullptr,
		                                                      "PS",
		                                                      // "ps_6_3"
		                                                      "ps_5_1"
		);
	}
	BuildInputLayout();
	BuildShapesGeometry();
	BuildModelGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs(m_rootSignature);

	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());
	m_dx12manager->ExecuteCommandLists();

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12DynamicCubePlayground::Update(const CETimer& gt) {
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

void CEDX12DynamicCubePlayground::Render(const CETimer& gt) {
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

	/*
	 * Bind sky cube map. For our demos, we just use one "world" cubemap representing environment from far away, so all objects will use same cube map and we only need to set it once per-frame
	 * If we wanted to use "local" cube maps, we would have to change them per-object, or dynamically index into an array of cube maps
	 */
	CD3DX12_GPU_DESCRIPTOR_HANDLE skyTexDescriptor(
		m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	skyTexDescriptor.Offset(m_skyTexHeapIndex,
	                        m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(3, skyTexDescriptor);

	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Opaque]);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["sky"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Sky]);

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

void CEDX12DynamicCubePlayground::Resize() {
	CEDX12Playground::Resize();
	m_camera.SetLens(0.25f * Resources::Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
}

void CEDX12DynamicCubePlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12DynamicCubePlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
	ReleaseCapture();
}

void CEDX12DynamicCubePlayground::OnMouseMove(KeyCode key, int x, int y) {
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

void CEDX12DynamicCubePlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12DynamicCubePlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyDown(key, keyChar, gt);
	const float dt = gt.DeltaTime();
	switch (key) {
	case KeyCode::A:
		m_camera.Strafe(-50.0f * dt);
		break;
	case KeyCode::D:
		m_camera.Strafe(50.0f * dt);
		break;
	case KeyCode::W:
		m_camera.Walk(50.0f * dt);
		break;
	case KeyCode::S:
		m_camera.Walk(-50.0f * dt);
		break;
	}
}

void CEDX12DynamicCubePlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12DynamicCubePlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);
	auto currObjectCB = mCurrFrameResource->ObjectStructuredCB.get();
	for (auto& ri : mAllRitems) {
		Resources::LitShapesRenderItem* e = static_cast<Resources::LitShapesRenderItem*>(ri.get());
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0) {
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			Resources::StructuredObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.WorldViewProjection, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
			objConstants.MaterialIndex = e->Mat->MatCBIndex;

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void CEDX12DynamicCubePlayground::UpdateMainPassCB(const CETimer& gt) {
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
	mMainPassCB.Lights[0].Strength = {0.6f, 0.6f, 0.6f};
	mMainPassCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
	mMainPassCB.Lights[1].Strength = {0.3f, 0.3f, 0.3f};
	mMainPassCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};
	mMainPassCB.Lights[2].Strength = {0.15f, 0.15f, 0.15f};

	auto currPassCB = mCurrFrameResource->PassStructuredCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CEDX12DynamicCubePlayground::UpdateMaterialCBs(const CETimer& gt) {
	CEDX12Playground::UpdateMaterialCBs(gt);
	auto currMaterialBuffer = mCurrFrameResource->MaterialBuffer.get();
	for (auto& e : mMaterials) {
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
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

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}
}

void CEDX12DynamicCubePlayground::BuildPSOs(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature) {
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

	//PSO for sky
	D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = opaquePsoDesc;

	//Camera is inside sky sphere so just turn off cuilling
	skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	/*
	 * Make sure depth function is LESS_EQUAL and not just LESS. Otherwise, normalized depth values at z = 1 (NDC) will fail depth test if depth buffer was cleared to 1.
	 */
	skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	skyPsoDesc.pRootSignature = m_rootSignature.Get();
	skyPsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shadersMap["skyVS"]->GetBufferPointer()),
		m_shadersMap["skyVS"]->GetBufferSize()
	};
	skyPsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_shadersMap["skyPS"]->GetBufferPointer()),
		m_shadersMap["skyPS"]->GetBufferSize()
	};
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&mPSOs["sky"])));
}

void CEDX12DynamicCubePlayground::LoadTextures() {
	std::unordered_map<std::string, std::string> textures = {
		{"bricksDiffuseMap", "bricks2.dds"},
		{"tileDiffuseMap", "tile.dds"},
		{"defaultDiffuseMap", "white1x1.dds"},
		{"skyCubeMap", "grasscube1024.dds"}
	};

	for (auto texPair : textures) {
		auto tex = m_dx12manager->LoadTextureFromFile(texPair.second, texPair.first);
		mTextures[tex->Name] = std::move(tex);
	}
}

void CEDX12DynamicCubePlayground::BuildDescriptorHeaps() {
	m_dx12manager->CreateSRVDescriptorHeap(5);

	auto srvSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		m_dx12manager->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	auto d3dDevice = m_dx12manager->GetD3D12Device();

	auto bricksTex = mTextures["bricksDiffuseMap"]->Resource;
	auto tileTex = mTextures["tileDiffuseMap"]->Resource;
	auto whiteTex = mTextures["defaultDiffuseMap"]->Resource;
	auto skyTex = mTextures["skyCubeMap"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = bricksTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = bricksTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	d3dDevice->CreateShaderResourceView(bricksTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvSize);

	srvDesc.Format = tileTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = tileTex->GetDesc().MipLevels;
	d3dDevice->CreateShaderResourceView(tileTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvSize);

	srvDesc.Format = whiteTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = whiteTex->GetDesc().MipLevels;
	d3dDevice->CreateShaderResourceView(whiteTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvSize);

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.Format = skyTex->GetDesc().Format;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.MipLevels = skyTex->GetDesc().MipLevels;
	d3dDevice->CreateShaderResourceView(skyTex.Get(), &srvDesc, hDescriptor);

	m_skyTexHeapIndex = 3;
}


void CEDX12DynamicCubePlayground::BuildModelGeometry() {
	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream modelsPathStream;
	modelsPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Models\\" <<
		"skull.txt";
	auto modelPath = modelsPathStream.str();
	spdlog::info("Loading Model from TXT: {}", modelPath);

	std::ifstream fin(modelPath);

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

		vertices[i].TexCoord = {0.0f, 0.0f};

		XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);

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
	geo->Name = "skullGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize,
	                                                          geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize,
	                                                         geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalTextureVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	submesh.Bounds = bounds;

	geo->DrawArgs["skull"] = submesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12DynamicCubePlayground::BuildShapesGeometry() {
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

	Resources::SubMeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	Resources::SubMeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	Resources::SubMeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	Resources::SubMeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	std::vector<Resources::CENormalTextureVertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].TexCoord = box.Vertices[i].TexC;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].TexCoord = grid.Vertices[i].TexC;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].TexCoord = sphere.Vertices[i].TexC;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].TexCoord = cylinder.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalTextureVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalTextureVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12DynamicCubePlayground::BuildFrameResources() {
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

void CEDX12DynamicCubePlayground::BuildMaterials() {

	auto bricks0 = std::make_unique<Resources::Material>();
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 0;
	bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks0->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	bricks0->Roughness = 0.3f;

	auto tile0 = std::make_unique<Resources::Material>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 1;
	tile0->DiffuseSrvHeapIndex = 1;
	tile0->DiffuseAlbedo = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	tile0->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	tile0->Roughness = 0.1f;

	auto mirror0 = std::make_unique<Resources::Material>();
	mirror0->Name = "mirror0";
	mirror0->MatCBIndex = 2;
	mirror0->DiffuseSrvHeapIndex = 2;
	mirror0->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.1f, 1.0f);
	mirror0->FresnelR0 = XMFLOAT3(0.98f, 0.97f, 0.95f);
	mirror0->Roughness = 0.1f;

	auto skullMat = std::make_unique<Resources::Material>();
	skullMat->Name = "skullMat";
	skullMat->MatCBIndex = 3;
	skullMat->DiffuseSrvHeapIndex = 2;
	skullMat->DiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	skullMat->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	skullMat->Roughness = 0.2f;

	auto sky = std::make_unique<Resources::Material>();
	sky->Name = "sky";
	sky->MatCBIndex = 4;
	sky->DiffuseSrvHeapIndex = 3;
	sky->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sky->Roughness = 1.0f;

	mMaterials["bricks0"] = std::move(bricks0);
	mMaterials["tile0"] = std::move(tile0);
	mMaterials["mirror0"] = std::move(mirror0);
	mMaterials["skullMat"] = std::move(skullMat);
	mMaterials["sky"] = std::move(sky);
}

void CEDX12DynamicCubePlayground::BuildRenderItems() {

	auto skyRitem = std::make_unique<Resources::LitShapesRenderItem>();
	XMStoreFloat4x4(&skyRitem->World, XMMatrixScaling(5000.0f, 5000.0f, 5000.0f));
	skyRitem->TexTransform = Resources::MatrixIdentity4X4();
	skyRitem->ObjCBIndex = 0;
	skyRitem->Mat = mMaterials["sky"].get();
	skyRitem->Geo = mGeometries["shapeGeo"].get();
	skyRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	skyRitem->IndexCount = skyRitem->Geo->DrawArgs["sphere"].IndexCount;
	skyRitem->StartIndexLocation = skyRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	skyRitem->BaseVertexLocation = skyRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Sky].push_back(skyRitem.get());
	mAllRitems.push_back(std::move(skyRitem));

	auto boxRitem = std::make_unique<Resources::LitShapesRenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 1.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	boxRitem->ObjCBIndex = 1;
	boxRitem->Mat = mMaterials["bricks0"].get();
	boxRitem->Geo = mGeometries["shapeGeo"].get();
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(boxRitem.get());
	mAllRitems.push_back(std::move(boxRitem));

	auto skullRitem = std::make_unique<Resources::LitShapesRenderItem>();
	XMStoreFloat4x4(&skullRitem->World, XMMatrixScaling(0.4f, 0.4f, 0.4f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	skullRitem->TexTransform = Resources::MatrixIdentity4X4();
	skullRitem->ObjCBIndex = 2;
	skullRitem->Mat = mMaterials["skullMat"].get();
	skullRitem->Geo = mGeometries["skullGeo"].get();
	skullRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	skullRitem->IndexCount = skullRitem->Geo->DrawArgs["skull"].IndexCount;
	skullRitem->StartIndexLocation = skullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
	skullRitem->BaseVertexLocation = skullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(skullRitem.get());
	mAllRitems.push_back(std::move(skullRitem));

	auto gridRitem = std::make_unique<Resources::LitShapesRenderItem>();
	gridRitem->World = Resources::MatrixIdentity4X4();
	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	gridRitem->ObjCBIndex = 3;
	gridRitem->Mat = mMaterials["tile0"].get();
	gridRitem->Geo = mGeometries["shapeGeo"].get();
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(gridRitem.get());
	mAllRitems.push_back(std::move(gridRitem));

	XMMATRIX brickTexTransform = XMMatrixScaling(1.5f, 2.0f, 1.0f);
	UINT objCBIndex = 4;
	for (int i = 0; i < 5; ++i) {
		auto leftCylRitem = std::make_unique<Resources::LitShapesRenderItem>();
		auto rightCylRitem = std::make_unique<Resources::LitShapesRenderItem>();
		auto leftSphereRitem = std::make_unique<Resources::LitShapesRenderItem>();
		auto rightSphereRitem = std::make_unique<Resources::LitShapesRenderItem>();

		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
		XMStoreFloat4x4(&leftCylRitem->TexTransform, brickTexTransform);
		leftCylRitem->ObjCBIndex = objCBIndex++;
		leftCylRitem->Mat = mMaterials["bricks0"].get();
		leftCylRitem->Geo = mGeometries["shapeGeo"].get();
		leftCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
		XMStoreFloat4x4(&rightCylRitem->TexTransform, brickTexTransform);
		rightCylRitem->ObjCBIndex = objCBIndex++;
		rightCylRitem->Mat = mMaterials["bricks0"].get();
		rightCylRitem->Geo = mGeometries["shapeGeo"].get();
		rightCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		leftSphereRitem->TexTransform = Resources::MatrixIdentity4X4();
		leftSphereRitem->ObjCBIndex = objCBIndex++;
		leftSphereRitem->Mat = mMaterials["mirror0"].get();
		leftSphereRitem->Geo = mGeometries["shapeGeo"].get();
		leftSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->TexTransform = Resources::MatrixIdentity4X4();
		rightSphereRitem->ObjCBIndex = objCBIndex++;
		rightSphereRitem->Mat = mMaterials["mirror0"].get();
		rightSphereRitem->Geo = mGeometries["shapeGeo"].get();
		rightSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(leftCylRitem.get());
		mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(rightCylRitem.get());
		mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(leftSphereRitem.get());
		mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(rightSphereRitem.get());

		mAllRitems.push_back(std::move(leftCylRitem));
		mAllRitems.push_back(std::move(rightCylRitem));
		mAllRitems.push_back(std::move(leftSphereRitem));
		mAllRitems.push_back(std::move(rightSphereRitem));
	}
}

void CEDX12DynamicCubePlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                                                  std::vector<Resources::RenderItem*>& ritems) const {
	UINT objCBByteSize = (sizeof(Resources::StructuredObjectConstants) + 255) & ~255;

	auto objectCB = mCurrFrameResource->ObjectStructuredCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i) {
		auto ri = static_cast<Resources::LitShapesRenderItem*>(ritems[i]);

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

void CEDX12DynamicCubePlayground::BuildCubeFaceCamera(float x, float y, float z) {
	//Generate cube map about given position
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	//Look along each coordinate axis.
	XMFLOAT3 targets[6] = {
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f) // -Z
	};

	/*
	 * Use world up vector (0, 1, 0) for all directions except +Y/-Y. In these cases, we are looking down +Y or -Y, so we need a different "up" vector
	 */
	XMFLOAT3 ups[6] = {
		XMFLOAT3(0.0f, 1.0f, 0.0f), // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f), // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f), // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f) // -Z
	};

	for (int i = 0; i < 6; ++i) {
		mCubeMapCamera[i].LookAt(center, targets[i], ups[i]);
		mCubeMapCamera[i].SetLens(0.5f * XM_PI, 1.0f, 0.1f, 1000.0f);
		mCubeMapCamera[i].UpdateViewMatrix();
	}
}
