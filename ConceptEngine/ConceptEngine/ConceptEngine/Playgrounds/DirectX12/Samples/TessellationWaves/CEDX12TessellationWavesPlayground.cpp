#include "CEDX12TessellationWavesPlayground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12TessellationWavesPlayground::CEDX12TessellationWavesPlayground() : CEDX12Playground() {
}

void CEDX12TessellationWavesPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	LoadTextures();
	//Root Signature
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE displacementMapTable;
		displacementMapTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[1].InitAsConstantBufferView(0);
		slotRootParameter[2].InitAsConstantBufferView(1);
		slotRootParameter[3].InitAsConstantBufferView(2);

		auto staticSamplers = m_dx12manager->GetStaticSamplers();
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(4,
		                                              slotRootParameter,
		                                              (UINT)staticSamplers.size(),
		                                              staticSamplers.data(),
		                                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		m_rootSignature = m_dx12manager->CreateRootSignature(&rootSignatureDesc);
	}
	BuildDescriptorHeaps();
	//Build Shaders
	{
		const D3D_SHADER_MACRO defines[] = {
			"FOG", "1",
			NULL, NULL
		};

		const D3D_SHADER_MACRO alphaDefines[] = {
			"FOG", "1",
			"ALPHA", "1",
			NULL, NULL
		};

		const D3D_SHADER_MACRO waveDefines[] = {
			"DISPLACEMENT_MAP", "1",
			NULL, NULL
		};

		m_shadersMap["standardVS"] = m_dx12manager->CompileShaders("CESobelFogVertexShader.hlsl",
		                                                           nullptr,
		                                                           "VS",
		                                                           // "vs_6_3"
		                                                           "vs_5_1"
		);
		m_shadersMap["wavesVS"] = m_dx12manager->CompileShaders("CESobelFogVertexShader.hlsl",
		                                                        waveDefines,
		                                                        "VS",
		                                                        // "vs_6_3"
		                                                        "vs_5_1"
		);
		m_shadersMap["opaquePS"] = m_dx12manager->CompileShaders("CESobelFogPixelShader.hlsl",
		                                                         defines,
		                                                         "PS",
		                                                         // "ps_6_3"
		                                                         "ps_5_1"
		);
		m_shadersMap["alphaPS"] = m_dx12manager->CompileShaders("CESobelFogPixelShader.hlsl",
		                                                        alphaDefines,
		                                                        "PS",
		                                                        // "ps_6_3"
		                                                        "ps_5_1"
		);
		m_shadersMap["treeSpriteVS"] = m_dx12manager->CompileShaders("CESpritesVertexShader.hlsl",
		                                                             nullptr,
		                                                             "VS",
		                                                             // "vs_6_3"
		                                                             "vs_5_1"
		);
		m_shadersMap["treeSpriteGS"] = m_dx12manager->CompileShaders("CESpritesGeometryShader.hlsl",
		                                                             nullptr,
		                                                             "GS",
		                                                             // "gs_6_3"
		                                                             "gs_5_1"
		);
		m_shadersMap["treeSpritePS"] = m_dx12manager->CompileShaders("CESpritesPixelShader.hlsl",
		                                                             alphaDefines,
		                                                             "PS",
		                                                             // "ps_6_3"
		                                                             "ps_5_1"
		);
		m_shadersMap["wavesUpdateCS"] = m_dx12manager->CompileShaders("CEWavesSimulationComputeShader.hlsl",
		                                                              nullptr,
		                                                              "UpdateWavesCS",
		                                                              // "cs_6_3"
		                                                              "cs_5_1"
		);
		m_shadersMap["wavesDisturbCS"] = m_dx12manager->CompileShaders("CEWavesSimulationComputeShader.hlsl",
		                                                               nullptr,
		                                                               "DisturbWavesCS",
		                                                               // "cs_6_3"
		                                                               "cs_5_1"
		);
		m_shadersMap["compositeVS"] = m_dx12manager->CompileShaders("CECompositeVertexShader.hlsl",
		                                                            nullptr,
		                                                            "VS",
		                                                            // "vs_6_3"
		                                                            "vs_5_1"
		);
		m_shadersMap["compositePS"] = m_dx12manager->CompileShaders("CECompositePixelShader.hlsl",
		                                                            nullptr,
		                                                            "PS",
		                                                            // "ps_6_3"
		                                                            "ps_5_1"
		);
		m_shadersMap["sobelCS"] = m_dx12manager->CompileShaders("CESobelComputeShader.hlsl",
		                                                        nullptr,
		                                                        "SobelCS",
		                                                        // "cs_6_3"
		                                                        "cs_5_1"
		);
		m_shadersMap["tessVS"] = m_dx12manager->CompileShaders("CETessellationVertexShader.hlsl",
		                                                       nullptr,
		                                                       "VS",
		                                                       // "vs_6_3"
		                                                       "vs_5_1"
		);
		m_shadersMap["tessHS"] = m_dx12manager->CompileShaders("CETessellationHullShader.hlsl",
		                                                       nullptr,
		                                                       "HS",
		                                                       // "hs_6_3"
		                                                       "hs_5_1"
		);
		m_shadersMap["tessDS"] = m_dx12manager->CompileShaders("CETessellationDomainShader.hlsl",
		                                                       nullptr,
		                                                       "DS",
		                                                       // "ds_6_3"
		                                                       "ds_5_1"
		);
		m_shadersMap["tessPS"] = m_dx12manager->CompileShaders("CETessellationPixelShader.hlsl",
		                                                       nullptr,
		                                                       "PS",
		                                                       // "ps_6_3"
		                                                       "ps_5_1"
		);
	}
	BuildInputLayout();
	{
		m_tessInputLayout = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};
	}
	BuildLandGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs(m_rootSignature);
	{
		auto d3dDevice = m_dx12manager->GetD3D12Device();
		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

		ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		opaquePsoDesc.InputLayout = { m_tessInputLayout.data(), (UINT)m_tessInputLayout.size()};
		opaquePsoDesc.pRootSignature = m_rootSignature.Get();
		opaquePsoDesc.VS =
		{
			reinterpret_cast<BYTE*>(m_shadersMap["tessVS"]->GetBufferPointer()),
			m_shadersMap["tessVS"]->GetBufferSize()
		};
		opaquePsoDesc.HS =
		{
			reinterpret_cast<BYTE*>(m_shadersMap["tessHS"]->GetBufferPointer()),
			m_shadersMap["tessHS"]->GetBufferSize()
		};
		opaquePsoDesc.DS =
		{
			reinterpret_cast<BYTE*>(m_shadersMap["tessDS"]->GetBufferPointer()),
			m_shadersMap["tessDS"]->GetBufferSize()
		};
		opaquePsoDesc.PS =
		{
			reinterpret_cast<BYTE*>(m_shadersMap["tessPS"]->GetBufferPointer()),
			m_shadersMap["tessPS"]->GetBufferSize()
		};
		opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		opaquePsoDesc.SampleMask = UINT_MAX;
		opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		opaquePsoDesc.NumRenderTargets = 1;
		opaquePsoDesc.RTVFormats[0] = m_dx12manager->GetBackBufferFormat();
		opaquePsoDesc.SampleDesc.Count = m_dx12manager->GetM4XMSAAState() ? 4 : 1;
		opaquePsoDesc.SampleDesc.Quality = m_dx12manager->GetM4XMSAAState() ? (m_dx12manager->GetM4XMSAAQuality() - 1) : 0;
		opaquePsoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();
		ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));
	}

	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());
	m_dx12manager->ExecuteCommandLists();

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12TessellationWavesPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
	UpdateCamera(gt);

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

void CEDX12TessellationWavesPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);

	auto mScreenViewport = m_dx12manager->GetViewPort();
	auto mScissorRect = m_dx12manager->GetScissorRect();

	auto cmdListAlloc = mCurrFrameResource->commandAllocator;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Reset(cmdListAlloc.Get(),
	                                                          mPSOs[(mIsWireframe ? "opaque_wireframe" : "opaque")].
	                                                          Get()));

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["opaque"].Get());

	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &mScreenViewport);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &mScissorRect);

	//Change offscreen texture to be used as a render target output
	auto trOffScrGRRT = CD3DX12_RESOURCE_BARRIER::Transition(m_dx12manager->CurrentBackBuffer(),
	                                                         D3D12_RESOURCE_STATE_PRESENT,
	                                                         D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trOffScrGRRT);

	//Clear back buffer and depth buffer
	auto dsv = m_dx12manager->DepthStencilView();
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(m_dx12manager->CurrentBackBufferView(),
	                                                            (float*)&mMainPassCB.FogColor,
	                                                            0,
	                                                            nullptr);
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(dsv,
	                                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	                                                            1.0f,
	                                                            0,
	                                                            0,
	                                                            nullptr);

	auto cbbv = m_dx12manager->CurrentBackBufferView();
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &cbbv, true, &dsv);

	//TODO: Add function as parameter to method to make creation of scene more dynamic
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_dx12manager->GetSRVDescriptorHeap().Get() };
	m_dx12manager->GetD3D12CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Opaque]);


	// Indicate a state transition on the resource usage.
	auto trCbb2 = CD3DX12_RESOURCE_BARRIER::Transition(m_dx12manager->CurrentBackBuffer(),
	                                                   D3D12_RESOURCE_STATE_RENDER_TARGET,
	                                                   D3D12_RESOURCE_STATE_PRESENT);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trCbb2);

	// Done recording commands.
	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());

	m_dx12manager->ExecuteCommandLists();

	// Swap the back and front buffers
	auto swapChain = m_dx12manager->GetDXGISwapChain();
	UINT presentFlags = (m_dx12manager->GetTearingSupport() && !m_dx12manager->IsFullScreen())
		                    ? DXGI_PRESENT_ALLOW_TEARING
		                    : 0;
	ThrowIfFailed(swapChain->Present(0, presentFlags));

	auto currentBackBufferIndex = m_dx12manager->GetCurrentBackBufferIndex();
	m_dx12manager->SetCurrentBackBufferIndex((currentBackBufferIndex + 1) % CEDX12Manager::GetBackBufferCount());

	m_dx12manager->FlushCommandQueue();
}

void CEDX12TessellationWavesPlayground::Resize() {
	CEDX12Playground::Resize();
	static const float Pi = 3.1415926535f;
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void CEDX12TessellationWavesPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12TessellationWavesPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);

	ReleaseCapture();
}

void CEDX12TessellationWavesPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
	static const float Pi = 3.1415926535f;
	if (key == KeyCode::LButton) {
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = m_dx12manager->Clamp(mPhi, 0.1f, Pi - 0.1f);
	}
	else if (key == KeyCode::RButton) {
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.2f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f * static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = m_dx12manager->Clamp(mRadius, 5.0f, 150.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CEDX12TessellationWavesPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12TessellationWavesPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyDown(key, keyChar, gt);
	const float dt = gt.DeltaTime();
	switch (key) {
	case KeyCode::Left:
		mSunTheta -= 4.0f * dt;
		break;
	case KeyCode::Right:
		mSunTheta += 4.0f * dt;
		break;
	case KeyCode::Up:
		mSunPhi -= 4.0f * dt;
		break;
	case KeyCode::Down:
		mSunPhi += 4.0f * dt;
		break;
	}

	mSunPhi = m_dx12manager->Clamp(mSunPhi, 0.1f, XM_PIDIV2);
}

void CEDX12TessellationWavesPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12TessellationWavesPlayground::UpdateCamera(const CETimer& gt) {
	CEDX12Playground::UpdateCamera(gt);
	mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
	mEyePos.y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void CEDX12TessellationWavesPlayground::AnimateMaterials(const CETimer& gt) {
	CEDX12Playground::AnimateMaterials(gt);

}

void CEDX12TessellationWavesPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);

	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& ri : mAllRitems) {
		Resources::LitShapesRenderItem* e = static_cast<Resources::LitShapesRenderItem*>(ri.get());
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0) {
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			Resources::ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.WorldViewProjection, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
			objConstants.DisplacementMapTexelSize = e->DisplacementMapTexelSize;
			objConstants.GridSpatialStep = e->GridSpatialStep;

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void CEDX12TessellationWavesPlayground::UpdateMaterialCBs(const CETimer& gt) {
	CEDX12Playground::UpdateMaterialCBs(gt);
	auto currMaterialCB = mCurrFrameResource->MaterialCB.get();

	for (auto& e : mMaterials) {
		//Only update cbuffer data if constants have changed. If cbuffer data changes, it needs to be updated for each FrameResource
		Resources::Material* mat = e.second.get();

		if (mat->NumFramesDirty > 0) {
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			Resources::MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			//Next FrameResource need to be updated too
			mat->NumFramesDirty--;
		}
	}
}

void CEDX12TessellationWavesPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);

	auto width = m_dx12manager->GetWindowWidth();
	auto height = m_dx12manager->GetWindowHeight();

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	auto determinantView = XMMatrixDeterminant(view);
	XMMATRIX invView = XMMatrixInverse(&determinantView, view);

	auto determinantProj = XMMatrixDeterminant(proj);
	XMMATRIX invProj = XMMatrixInverse(&determinantProj, proj);

	auto determinantViewProj = XMMatrixDeterminant(viewProj);
	XMMATRIX invViewProj = XMMatrixInverse(&determinantViewProj, viewProj);

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
	mMainPassCB.AmbientLight = {0.25f, 0.25f, 0.35f, 1.0f};

	XMVECTOR lightDirection = -m_dx12manager->SphericalToCartesian(1.0f, mSunTheta, mSunPhi);

	XMStoreFloat3(&mMainPassCB.Lights[0].Direction, lightDirection);
	mMainPassCB.Lights[0].Strength = {0.6f, 0.6f, 0.6f};
	mMainPassCB.Lights[1].Strength = {0.3f, 0.3f, 0.3f};
	mMainPassCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
	mMainPassCB.Lights[2].Strength = {0.15f, 0.15f, 0.15f};
	mMainPassCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CEDX12TessellationWavesPlayground::LoadTextures() {
	auto grassTex = m_dx12manager->LoadTextureFromFile("grass.dds", "grassTex");
	auto waterTex = m_dx12manager->LoadTextureFromFile("water1.dds", "waterTex");
	auto fenceTex = m_dx12manager->LoadTextureFromFile("WireFence.dds", "fenceTex");
	auto treeArrayTex = m_dx12manager->LoadTextureFromFile("treeArray2.dds", "treeArrayTex");
	auto checkboardTex = m_dx12manager->LoadTextureFromFile("checkboard.dds", "checkboardTex");
	auto iceTex = m_dx12manager->LoadTextureFromFile("ice.dds", "iceTex");
	auto white1x1Tex = m_dx12manager->LoadTextureFromFile("white1x1.dds", "white1x1Tex");
	auto bricksTex = m_dx12manager->LoadTextureFromFile("bricks.dds", "bricksTex");

	mTextures[grassTex->Name] = std::move(grassTex);
	mTextures[waterTex->Name] = std::move(waterTex);
	mTextures[fenceTex->Name] = std::move(fenceTex);
	mTextures[treeArrayTex->Name] = std::move(treeArrayTex);
	mTextures[checkboardTex->Name] = std::move(checkboardTex);
	mTextures[iceTex->Name] = std::move(iceTex);
	mTextures[white1x1Tex->Name] = std::move(white1x1Tex);
	mTextures[bricksTex->Name] = std::move(bricksTex);
}

void CEDX12TessellationWavesPlayground::BuildDescriptorHeaps() {
	auto d3dDevice = m_dx12manager->GetD3D12Device();
	auto srvUavSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	UINT srvCount = 4;

	/*
	 * Create SRV heap
	 */
	m_dx12manager->
		CreateSRVDescriptorHeap(srvCount);

	//Fill out heap with actual descriptors
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		m_dx12manager->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	auto bricksTex = mTextures["bricksTex"]->Resource;
	auto checkboardTex = mTextures["checkboardTex"]->Resource;
	auto iceTex = mTextures["iceTex"]->Resource;
	auto white1x1Tex = mTextures["white1x1Tex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = bricksTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	d3dDevice->CreateShaderResourceView(bricksTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvUavSize);

	srvDesc.Format = checkboardTex->GetDesc().Format;
	d3dDevice->CreateShaderResourceView(checkboardTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvUavSize);

	srvDesc.Format = iceTex->GetDesc().Format;
	d3dDevice->CreateShaderResourceView(iceTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvUavSize);

	srvDesc.Format = white1x1Tex->GetDesc().Format;
	d3dDevice->CreateShaderResourceView(white1x1Tex.Get(), &srvDesc, hDescriptor);
}

void CEDX12TessellationWavesPlayground::BuildLandGeometry() {

	std::array<XMFLOAT3, 4> vertices =
	{
		XMFLOAT3(-10.0f, 0.0f, +10.0f),
		XMFLOAT3(+10.0f, 0.0f, +10.0f),
		XMFLOAT3(-10.0f, 0.0f, -10.0f),
		XMFLOAT3(+10.0f, 0.0f, -10.0f)
	};

	std::array<std::int16_t, 4> indices = {0, 1, 2, 3};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalTextureVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "quadpatchGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(XMFLOAT3);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry quadSubmesh;
	quadSubmesh.IndexCount = 4;
	quadSubmesh.StartIndexLocation = 0;
	quadSubmesh.BaseVertexLocation = 0;

	geo->DrawArgs["quadpatch"] = quadSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12TessellationWavesPlayground::BuildMaterials() {
	auto grass = std::make_unique<Resources::Material>();
	grass->Name = "grass";
	grass->MatCBIndex = 0;
	grass->DiffuseSrvHeapIndex = 0;
	grass->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	grass->Roughness = 0.125f;

	// This is not a good water material definition, but we do not have all the rendering
	// tools we need (transparency, environment reflection), so we fake it for now.
	auto water = std::make_unique<Resources::Material>();
	water->Name = "water";
	water->MatCBIndex = 1;
	water->DiffuseSrvHeapIndex = 1;
	water->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	water->Roughness = 0.0f;

	auto wirefence = std::make_unique<Resources::Material>();
	wirefence->Name = "wirefence";
	wirefence->MatCBIndex = 2;
	wirefence->DiffuseSrvHeapIndex = 2;
	wirefence->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	wirefence->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	wirefence->Roughness = 0.25f;

	auto treeSprites = std::make_unique<Resources::Material>();
	treeSprites->Name = "treeSprites";
	treeSprites->MatCBIndex = 3;
	treeSprites->DiffuseSrvHeapIndex = 3;
	treeSprites->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	treeSprites->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	treeSprites->Roughness = 0.125f;

	auto whiteMat = std::make_unique<Resources::Material>();
	whiteMat->Name = "quadMat";
	whiteMat->MatCBIndex = 0;
	whiteMat->DiffuseSrvHeapIndex = 3;
	whiteMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	whiteMat->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	whiteMat->Roughness = 0.5f;

	mMaterials["whiteMat"] = std::move(whiteMat);
	mMaterials["grass"] = std::move(grass);
	mMaterials["water"] = std::move(water);
	mMaterials["wirefence"] = std::move(wirefence);
	mMaterials["treeSprites"] = std::move(treeSprites);
}

void CEDX12TessellationWavesPlayground::BuildRenderItems() {

	auto quadPatchRitem = std::make_unique<Resources::LitShapesRenderItem>();
	quadPatchRitem->World = Resources::MatrixIdentity4X4();
	quadPatchRitem->TexTransform = Resources::MatrixIdentity4X4();

	quadPatchRitem->ObjCBIndex = 0;
	quadPatchRitem->Mat = mMaterials["whiteMat"].get();
	quadPatchRitem->Geo = mGeometries["quadpatchGeo"].get();
	quadPatchRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
	quadPatchRitem->IndexCount = quadPatchRitem->Geo->DrawArgs["quadpatch"].IndexCount;
	quadPatchRitem->StartIndexLocation = quadPatchRitem->Geo->DrawArgs["quadpatch"].StartIndexLocation;
	quadPatchRitem->BaseVertexLocation = quadPatchRitem->Geo->DrawArgs["quadpatch"].BaseVertexLocation;
	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(quadPatchRitem.get());

	mAllRitems.push_back(std::move(quadPatchRitem));
}

void CEDX12TessellationWavesPlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<Resources::CEFrameResource>(m_dx12manager->GetD3D12Device().Get(),
		                                                                       2,
		                                                                       (UINT)mAllRitems.size(),
		                                                                       (UINT)mMaterials.size(),
		                                                                       0,
		                                                                       Resources::WavesNormalTextureVertex));
	}
}

void CEDX12TessellationWavesPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                                                        std::vector<Resources::RenderItem*>& ritems) const {
	UINT objCBByteSize = (sizeof(Resources::ObjectConstants) + 255) & ~255;
	UINT matCBByteSize = (sizeof(Resources::MaterialConstants) + 255) & ~255;

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();
	auto matCB = mCurrFrameResource->MaterialCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i) {
		auto ri = static_cast<Resources::LitShapesRenderItem*>(ritems[i]);

		auto vbv = ri->Geo->VertexBufferView();
		auto ibv = ri->Geo->IndexBufferView();
		cmdList->IASetVertexBuffers(0, 1, &vbv);
		cmdList->IASetIndexBuffer(&ibv);
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		auto size = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, size);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

		cmdList->SetGraphicsRootDescriptorTable(0, tex);
		cmdList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

float CEDX12TessellationWavesPlayground::GetHillsHeight(float x, float z) const {
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 CEDX12TessellationWavesPlayground::GetHillsNormal(float x, float z) const {
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}
