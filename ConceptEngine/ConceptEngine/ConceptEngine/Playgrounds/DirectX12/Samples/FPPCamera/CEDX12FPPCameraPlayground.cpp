#include "CEDX12FPPCameraPlayground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12FPPCameraPlayground::CEDX12FPPCameraPlayground() : CEDX12Playground() {
}

void CEDX12FPPCameraPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	//Create GPU based waves
	m_waves = std::make_unique<Resources::CEGpuWaves>(m_dx12manager->GetD3D12Device().Get(),
	                                                  m_dx12manager->GetD3D12CommandList().Get(),
	                                                  256, 256, 1.0f, 0.03f, 4.0f, 0.2f);

	m_camera.SetPosition(0.0f, 2.0f, -15.0f);

	LoadTextures();
	//Root Signature
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 0);

		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstantBufferView(1);
		slotRootParameter[2].InitAsShaderResourceView(0, 1);
		slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_ALL);

		auto staticSamplers = m_dx12manager->GetStaticSamplers();
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(4,
		                                              slotRootParameter,
		                                              (UINT)staticSamplers.size(),
		                                              staticSamplers.data(),
		                                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		m_rootSignature = m_dx12manager->CreateRootSignature(&rootSignatureDesc);
	}
	// BuildWavesRootSignature
	{
		CD3DX12_DESCRIPTOR_RANGE uavTable0;
		uavTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE uavTable1;
		uavTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);

		CD3DX12_DESCRIPTOR_RANGE uavTable2;
		uavTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);

		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		// Perfomance TIP: Order from most frequent to least frequent.
		slotRootParameter[0].InitAsConstants(6, 0);
		slotRootParameter[1].InitAsDescriptorTable(1, &uavTable0);
		slotRootParameter[2].InitAsDescriptorTable(1, &uavTable1);
		slotRootParameter[3].InitAsDescriptorTable(1, &uavTable2);

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		                                        0, nullptr,
		                                        D3D12_ROOT_SIGNATURE_FLAG_NONE);
		m_wavesRootSignature = m_dx12manager->CreateRootSignature(&rootSigDesc);
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
		m_shadersMap["indexingVS"] = m_dx12manager->CompileShaders("CEIndexingVertexShader.hlsl",
		                                                           nullptr,
		                                                           "VS",
		                                                           // "vs_6_3"
		                                                           "vs_5_1"
		);;
		m_shadersMap["indexingPS"] = m_dx12manager->CompileShaders("CEIndexingPixelShader.hlsl",
		                                                           nullptr,
		                                                           "PS",
		                                                           // "ps_6_3"
		                                                           "ps_5_1"
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
	}
	BuildInputLayout();
	//Build InputLayout for tree sprites
	{
		m_spriteInputLayout = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"SIZE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};
	}
	BuildLandGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	// BuildPSOs(m_rootSignature);
	{
		auto d3dDevice = m_dx12manager->GetD3D12Device();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC basePsoDesc;
		ZeroMemory(&basePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		basePsoDesc.InputLayout = {m_inputLayout.data(), (UINT)m_inputLayout.size()};
		basePsoDesc.pRootSignature = m_rootSignature.Get();
		basePsoDesc.VS = {
			reinterpret_cast<BYTE*>(m_shadersMap["indexingVS"]->GetBufferPointer()),
			m_shadersMap["indexingVS"]->GetBufferSize()
		};
		basePsoDesc.PS = {
			reinterpret_cast<BYTE*>(m_shadersMap["indexingPS"]->GetBufferPointer()),
			m_shadersMap["indexingPS"]->GetBufferSize()
		};
		basePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		basePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		basePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		basePsoDesc.SampleMask = UINT_MAX;
		basePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		basePsoDesc.NumRenderTargets = 1;
		basePsoDesc.RTVFormats[0] = m_dx12manager->GetBackBufferFormat();
		basePsoDesc.SampleDesc.Count = m_dx12manager->GetM4XMSAAState() ? 4 : 1;
		basePsoDesc.SampleDesc.Quality = m_dx12manager->GetM4XMSAAState()
			                                 ? (m_dx12manager->GetM4XMSAAQuality() - 1)
			                                 : 0;
		basePsoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();
		ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&basePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));
	}

	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());
	m_dx12manager->ExecuteCommandLists();

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12FPPCameraPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
	// UpdateCamera(gt);
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

	// AnimateMaterials(gt);
	UpdateObjectCBs(gt);
	UpdateMaterialCBs(gt);
	UpdateMainPassCB(gt);
}

void CEDX12FPPCameraPlayground::Render(const CETimer& gt) {
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
	                                                          mPSOs["opaque"].
	                                                          Get()));

	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &mScreenViewport);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	auto cbb = m_dx12manager->CurrentBackBuffer();
	auto trSPRT = CD3DX12_RESOURCE_BARRIER::Transition(cbb,
	                                                   D3D12_RESOURCE_STATE_PRESENT,
	                                                   D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trSPRT);

	// Clear the back buffer and depth buffer.
	auto currBbV = m_dx12manager->CurrentBackBufferView();
	auto depthSV = m_dx12manager->DepthStencilView();
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(currBbV, Colors::LightSteelBlue, 0,
	                                                            nullptr);
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(depthSV,
	                                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
	                                                            0, 0, nullptr);

	// Specify the buffers we are going to render to.
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &currBbV, true, &depthSV);

	ID3D12DescriptorHeap* descriptorHeaps[] = {m_dx12manager->GetSRVDescriptorHeap().Get()};
	m_dx12manager->GetD3D12CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	// Bind all the materials used in this scene.  For structured buffers, we can bypass the heap and 
	// set as a root descriptor.
	auto matBuffer = mCurrFrameResource->MaterialCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootShaderResourceView(2, matBuffer->GetGPUVirtualAddress());

	// Bind all the textures used in this scene.  Observe
	// that we only have to specify the first descriptor in the table.  
	// The root signature knows how many descriptors are expected in the table.
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(
		3, m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mOpaqueRitems);

	auto cbbv = m_dx12manager->CurrentBackBuffer();
	auto trRTSP = CD3DX12_RESOURCE_BARRIER::Transition(cbbv,
	                                                   D3D12_RESOURCE_STATE_RENDER_TARGET,
	                                                   D3D12_RESOURCE_STATE_PRESENT);
	// Indicate a state transition on the resource usage.
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trRTSP);
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

void CEDX12FPPCameraPlayground::Resize() {
	CEDX12Playground::Resize();
	static const float Pi = 3.1415926535f;
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	// XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
	// XMStoreFloat4x4(&mProj, P);
	m_camera.SetLens(0.25f * Resources::Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
}

void CEDX12FPPCameraPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12FPPCameraPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);

	ReleaseCapture();
}

void CEDX12FPPCameraPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
	static const float Pi = 3.1415926535f;

	if (key == KeyCode::LButton) {
		//Make each pixel correspond to quarter of degree
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		m_camera.Pitch(dy);
		m_camera.RotateY(dx);
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

void CEDX12FPPCameraPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12FPPCameraPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
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
	case KeyCode::W:
		m_camera.Walk(100.0f * dt);
		break;
	case KeyCode::A:
		m_camera.Strafe(-100.0f * dt);
		break;
	case KeyCode::S:
		m_camera.Walk(-100.0f * dt);
		break;
	case KeyCode::D:
		m_camera.Strafe(100.0f * dt);
		break;
	}

	auto camPos = m_camera.GetPosition3f();
	spdlog::info("Camera Position -> X: {} Y: {} Z: {}", camPos.x, camPos.y, camPos.z);

	m_camera.UpdateViewMatrix();
	mSunPhi = m_dx12manager->Clamp(mSunPhi, 0.1f, XM_PIDIV2);
}

void CEDX12FPPCameraPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12FPPCameraPlayground::UpdateCamera(const CETimer& gt) {
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

void CEDX12FPPCameraPlayground::AnimateMaterials(const CETimer& gt) {
	CEDX12Playground::AnimateMaterials(gt);
	// Scroll the water material texture coordinates.
	auto waterMat = mMaterials["water"].get();

	float& tu = waterMat->MatTransform(3, 0);
	float& tv = waterMat->MatTransform(3, 1);

	tu += 0.1f * gt.DeltaTime();
	tv += 0.02f * gt.DeltaTime();

	if (tu >= 1.0f)
		tu -= 1.0f;

	if (tv >= 1.0f)
		tv -= 1.0f;

	waterMat->MatTransform(3, 0) = tu;
	waterMat->MatTransform(3, 1) = tv;

	// Material has changed, so need to update cbuffer.
	waterMat->NumFramesDirty = gNumFrameResources;
}

void CEDX12FPPCameraPlayground::UpdateObjectCBs(const CETimer& gt) {
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
			// objConstants.DisplacementMapTexelSize = e->DisplacementMapTexelSize;
			// objConstants.GridSpatialStep = e->GridSpatialStep;
			objConstants.MaterialIndex = e->Mat->MatCBIndex;

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void CEDX12FPPCameraPlayground::UpdateMaterialCBs(const CETimer& gt) {
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
			matConstants.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			//Next FrameResource need to be updated too
			mat->NumFramesDirty--;
		}
	}
}

void CEDX12FPPCameraPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);

	auto width = m_dx12manager->GetWindowWidth();
	auto height = m_dx12manager->GetWindowHeight();

	// XMMATRIX view = XMLoadFloat4x4(&mView);
	// XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX view = m_camera.GetView();
	XMMATRIX proj = m_camera.GetProj();

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

	mMainPassCB.EyePosW = m_camera.GetPosition3f();
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

void CEDX12FPPCameraPlayground::LoadTextures() {
	auto grassTex = m_dx12manager->LoadTextureFromFile("grass.dds", "grassTex");
	auto waterTex = m_dx12manager->LoadTextureFromFile("water1.dds", "waterTex");
	auto fenceTex = m_dx12manager->LoadTextureFromFile("WireFence.dds", "fenceTex");
	auto treeArrayTex = m_dx12manager->LoadTextureFromFile("treeArray2.dds", "treeArrayTex");
	auto bricksTex = m_dx12manager->LoadTextureFromFile("bricks.dds", "bricksTex");
	auto stoneTex = m_dx12manager->LoadTextureFromFile("stone.dds", "stoneTex");
	auto tileTex = m_dx12manager->LoadTextureFromFile("tile.dds", "tileTex");
	auto crateTex = m_dx12manager->LoadTextureFromFile("WoodCrate01.dds", "crateTex");

	mTextures[grassTex->Name] = std::move(grassTex);
	mTextures[waterTex->Name] = std::move(waterTex);
	mTextures[fenceTex->Name] = std::move(fenceTex);
	mTextures[treeArrayTex->Name] = std::move(treeArrayTex);
	mTextures[bricksTex->Name] = std::move(bricksTex);
	mTextures[stoneTex->Name] = std::move(stoneTex);
	mTextures[tileTex->Name] = std::move(tileTex);
	mTextures[crateTex->Name] = std::move(crateTex);
}

void CEDX12FPPCameraPlayground::BuildDescriptorHeaps() {
	//Offscreen RTV goes after swap chain descriptors

	auto d3dDevice = m_dx12manager->GetD3D12Device();
	auto srvUavSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	UINT srvCount = 4;

	int waveSrvOffset = srvCount;

	/*
	 * Create SRV heap
	 */
	m_dx12manager->
		CreateSRVDescriptorHeap(srvCount + m_waves->DescriptorCount());

	//Fill out heap with actual descriptors
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		m_dx12manager->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	auto grassTex = mTextures["grassTex"]->Resource;
	auto waterTex = mTextures["waterTex"]->Resource;
	auto fenceTex = mTextures["fenceTex"]->Resource;
	auto treeArrayTex = mTextures["treeArrayTex"]->Resource;

	auto bricksTex = mTextures["bricksTex"]->Resource;
	auto stoneTex = mTextures["stoneTex"]->Resource;
	auto tileTex = mTextures["tileTex"]->Resource;
	auto crateTex = mTextures["crateTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = bricksTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = bricksTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	d3dDevice->CreateShaderResourceView(bricksTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvUavSize);

	srvDesc.Format = stoneTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = stoneTex->GetDesc().MipLevels;
	d3dDevice->CreateShaderResourceView(stoneTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvUavSize);

	srvDesc.Format = tileTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = tileTex->GetDesc().MipLevels;
	d3dDevice->CreateShaderResourceView(tileTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvUavSize);

	srvDesc.Format = crateTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = crateTex->GetDesc().MipLevels;
	d3dDevice->CreateShaderResourceView(crateTex.Get(), &srvDesc, hDescriptor);

}

void CEDX12FPPCameraPlayground::BuildLandGeometry() {
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

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize,
	                                                          geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize,
	                                                         geo->IndexBufferUploader);

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

void CEDX12FPPCameraPlayground::BuildTreeSpritesGeometry() {
	struct TreeSpriteVertex {
		XMFLOAT3 Pos;
		XMFLOAT2 Size;
	};

	static const int treeCount = 16;
	std::array<TreeSpriteVertex, treeCount> vertices;
	std::array<std::uint16_t, treeCount> indices;
	for (UINT i = 0; i < treeCount; ++i) {
		float x = m_dx12manager->RandF(-45.0f, 45.0f);
		float z = m_dx12manager->RandF(-45.0f, 45.0f);
		float y = GetHillsHeight(x, z);

		y += 8.0f;

		vertices[i].Pos = XMFLOAT3(x, y, z);
		vertices[i].Size = XMFLOAT2(20.0f, 20.0f);
		indices[i] = i;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(TreeSpriteVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "treeSpritesGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->VertexBufferGPU->SetName(L"Landscape TreeSprites Vertex Resource");

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);
	geo->IndexBufferGPU->SetName(L"Landscape TreeSprites Index Resource");

	geo->VertexByteStride = sizeof(TreeSpriteVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["points"] = submesh;

	mGeometries["treeSpritesGeo"] = std::move(geo);
}

void CEDX12FPPCameraPlayground::BuildWavesGeometryBuffers() {

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, m_waves->RowCount(), m_waves->ColumnCount());

	std::vector<Resources::CENormalTextureVertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i) {
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].TexCoord = grid.Vertices[i].TexC;
	}

	std::vector<std::uint32_t> indices = grid.Indices32;

	UINT vbByteSize = m_waves->VertexCount() * sizeof(Resources::CENormalTextureVertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "waterGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->VertexBufferGPU->SetName(L"Landscape Waves Vertex Resource");

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);
	geo->IndexBufferGPU->SetName(L"Landscape Waves Index Resource");

	geo->VertexByteStride = sizeof(Resources::CENormalTextureVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = submesh;

	mGeometries["waterGeo"] = std::move(geo);
}

void CEDX12FPPCameraPlayground::BuildBoxGeometry() {
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(8.0f, 8.0f, 8.0f, 3);

	std::vector<Resources::CENormalTextureVertex> vertices(box.Vertices.size());
	for (size_t i = 0; i < box.Vertices.size(); ++i) {
		auto& p = box.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].TexCoord = box.Vertices[i].TexC;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalTextureVertex);

	std::vector<std::uint16_t> indices = box.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->VertexBufferGPU->SetName(L"Landscape Box Vertex Resource");

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);
	geo->IndexBufferGPU->SetName(L"Landscape Box Index Resource");

	geo->VertexByteStride = sizeof(Resources::CENormalTextureVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["box"] = submesh;

	mGeometries["boxGeo"] = std::move(geo);
}

void CEDX12FPPCameraPlayground::BuildMaterials() {

	auto bricks0 = std::make_unique<Resources::Material>();
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 0;
	bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks0->Roughness = 0.1f;

	auto stone0 = std::make_unique<Resources::Material>();
	stone0->Name = "stone0";
	stone0->MatCBIndex = 1;
	stone0->DiffuseSrvHeapIndex = 1;
	stone0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	stone0->Roughness = 0.3f;

	auto tile0 = std::make_unique<Resources::Material>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 2;
	tile0->DiffuseSrvHeapIndex = 2;
	tile0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile0->Roughness = 0.3f;

	auto crate0 = std::make_unique<Resources::Material>();
	crate0->Name = "crate0";
	crate0->MatCBIndex = 3;
	crate0->DiffuseSrvHeapIndex = 3;
	crate0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	crate0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	crate0->Roughness = 0.2f;

	mMaterials["bricks0"] = std::move(bricks0);
	mMaterials["stone0"] = std::move(stone0);
	mMaterials["tile0"] = std::move(tile0);
	mMaterials["crate0"] = std::move(crate0);
}

void CEDX12FPPCameraPlayground::BuildRenderItems() {
	auto boxRitem = std::make_unique<Resources::LitShapesRenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	boxRitem->ObjCBIndex = 0;
	boxRitem->Mat = mMaterials["crate0"].get();
	boxRitem->Geo = mGeometries["shapeGeo"].get();
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	mAllRitems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<Resources::LitShapesRenderItem>();
	gridRitem->World = Resources::MatrixIdentity4X4();
	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	gridRitem->ObjCBIndex = 1;
	gridRitem->Mat = mMaterials["tile0"].get();
	gridRitem->Geo = mGeometries["shapeGeo"].get();
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	mAllRitems.push_back(std::move(gridRitem));

	XMMATRIX brickTexTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	UINT objCBIndex = 2;
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
		leftSphereRitem->Mat = mMaterials["stone0"].get();
		leftSphereRitem->Geo = mGeometries["shapeGeo"].get();
		leftSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->TexTransform = Resources::MatrixIdentity4X4();
		rightSphereRitem->ObjCBIndex = objCBIndex++;
		rightSphereRitem->Mat = mMaterials["stone0"].get();
		rightSphereRitem->Geo = mGeometries["shapeGeo"].get();
		rightSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		mAllRitems.push_back(std::move(leftCylRitem));
		mAllRitems.push_back(std::move(rightCylRitem));
		mAllRitems.push_back(std::move(leftSphereRitem));
		mAllRitems.push_back(std::move(rightSphereRitem));
	}

	// All the render items are opaque.
	for (auto& e : mAllRitems)
		mOpaqueRitems.push_back(e.get());
}

void CEDX12FPPCameraPlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<Resources::CEFrameResource>(m_dx12manager->GetD3D12Device().Get(),
		                                                                       1,
		                                                                       (UINT)mAllRitems.size(),
		                                                                       (UINT)mMaterials.size(),
		                                                                       0,
		                                                                       Resources::WavesNormalTextureVertex));
	}
}

void CEDX12FPPCameraPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
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

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

float CEDX12FPPCameraPlayground::GetHillsHeight(float x, float z) const {
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 CEDX12FPPCameraPlayground::GetHillsNormal(float x, float z) const {
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void CEDX12FPPCameraPlayground::DrawFullscreenQuad(ID3D12GraphicsCommandList* cmdList) {
	//Null-out IA stage since we build vertex off SV_VertexID in shader
	cmdList->IASetVertexBuffers(0, 1, nullptr);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->DrawInstanced(6, 1, 0, 0);
}

void CEDX12FPPCameraPlayground::UpdateWavesGPU(const CETimer& gt) {
	//every quarter second, generate random wave
	static float t_base = 0.0f;
	if ((gt.TotalTime() - t_base) >= 0.25f) {
		t_base += 0.25f;

		int i = m_dx12manager->Rand(4, m_waves->RowCount() - 5);
		int j = m_dx12manager->Rand(4, m_waves->ColumnCount() - 5);

		float r = m_dx12manager->RandF(1.0f, 2.0f);

		m_waves->Disturb(m_dx12manager->GetD3D12CommandList().Get(),
		                 m_wavesRootSignature.Get(),
		                 mPSOs["wavesDisturb"].Get(),
		                 i,
		                 j,
		                 r);
	}

	//Update wave simulation.
	m_waves->Update(gt,
	                m_dx12manager->GetD3D12CommandList().Get(),
	                m_wavesRootSignature.Get(),
	                mPSOs["wavesUpdate"].Get());
}
