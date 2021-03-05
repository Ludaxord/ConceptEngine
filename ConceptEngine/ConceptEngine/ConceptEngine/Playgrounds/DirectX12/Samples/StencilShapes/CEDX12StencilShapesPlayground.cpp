#include "CEDX12StencilShapesPlayground.h"

#include <fstream>


#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12StencilShapesPlayground::CEDX12StencilShapesPlayground(): CEDX12Playground() {
}

void CEDX12StencilShapesPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	LoadTextures();
	//RootSignatures
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		// Perfomance TIP: Order from most frequent to least frequent.
		slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[1].InitAsConstantBufferView(0);
		slotRootParameter[2].InitAsConstantBufferView(1);
		slotRootParameter[3].InitAsConstantBufferView(2);

		auto staticSamplers = m_dx12manager->GetStaticSamplers();

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		                                        (UINT)staticSamplers.size(), staticSamplers.data(),
		                                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		//create root signature with single slot which points to descriptor range consisting of single constant buffer
		m_rootSignature = m_dx12manager->CreateRootSignature(&rootSigDesc);
	}
	BuildDescriptorHeaps();
	//Build shaders with fog
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

		m_shadersMap["standardVS"] = m_dx12manager->CompileShaders("CEStencilVertexShader.hlsl",
		                                                           nullptr,
		                                                           "VS",
		                                                           // "vs_6_3"
		                                                           "vs_5_1"
		);
		m_shadersMap["opaquePS"] = m_dx12manager->CompileShaders("CEStencilPixelShader.hlsl",
		                                                         defines,
		                                                         "PS",
		                                                         // "ps_6_3"
		                                                         "ps_5_1"
		);
		m_shadersMap["alphaPS"] = m_dx12manager->CompileShaders("CEStencilPixelShader.hlsl",
		                                                        alphaDefines,
		                                                        "PS",
		                                                        // "ps_6_3"
		                                                        "ps_5_1"
		);
	}
	BuildInputLayout();
	BuildRoomGeometry();
	BuildSkullGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs(m_rootSignature);
	{
		auto d3dDevice = m_dx12manager->GetD3D12Device();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC basePsoDesc;

		//
		// PSO for opaque objects.
		//
		ZeroMemory(&basePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		basePsoDesc.InputLayout = {m_inputLayout.data(), (UINT)m_inputLayout.size()};
		basePsoDesc.pRootSignature = m_rootSignature.Get();
		basePsoDesc.VS = {
			reinterpret_cast<BYTE*>(m_shadersMap["standardVS"]->GetBufferPointer()),
			m_shadersMap["standardVS"]->GetBufferSize()
		};
		basePsoDesc.PS =
		{
			reinterpret_cast<BYTE*>(m_shadersMap["opaquePS"]->GetBufferPointer()),
			m_shadersMap["opaquePS"]->GetBufferSize()
		};
		basePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		basePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		basePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		basePsoDesc.SampleMask = UINT_MAX;
		basePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		basePsoDesc.NumRenderTargets = 1;
		basePsoDesc.RTVFormats[0] = m_dx12manager->GetBackBufferFormat();
		basePsoDesc.SampleDesc.Count = m_dx12manager->GetM4XMSAAState() ? 4 : 1;
		basePsoDesc.SampleDesc.Quality =
			m_dx12manager->GetM4XMSAAState() ? (m_dx12manager->GetM4XMSAAQuality() - 1) : 0;
		basePsoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();
		ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&basePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));

		//
		// PSO for transparent objects
		//

		D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = basePsoDesc;

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

		transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
		ThrowIfFailed(
			d3dDevice->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&mPSOs["transparent"])));

		//
		// PSO for marking stencil mirrors.
		//

		CD3DX12_BLEND_DESC mirrorBlendState(D3D12_DEFAULT);
		mirrorBlendState.RenderTarget[0].RenderTargetWriteMask = 0;

		D3D12_DEPTH_STENCIL_DESC mirrorDSS;
		mirrorDSS.DepthEnable = true;
		mirrorDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		mirrorDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		mirrorDSS.StencilEnable = true;
		mirrorDSS.StencilReadMask = 0xff;
		mirrorDSS.StencilWriteMask = 0xff;

		mirrorDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		mirrorDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		mirrorDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		mirrorDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		// We are not rendering backfacing polygons, so these settings do not matter.
		mirrorDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		mirrorDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		mirrorDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		mirrorDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC markMirrorsPsoDesc = basePsoDesc;
		markMirrorsPsoDesc.BlendState = mirrorBlendState;
		markMirrorsPsoDesc.DepthStencilState = mirrorDSS;
		ThrowIfFailed(
			d3dDevice->CreateGraphicsPipelineState(&markMirrorsPsoDesc, IID_PPV_ARGS(&mPSOs["markStencilMirrors"])));

		//
		// PSO for stencil reflections.
		//

		D3D12_DEPTH_STENCIL_DESC reflectionsDSS;
		reflectionsDSS.DepthEnable = true;
		reflectionsDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		reflectionsDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		reflectionsDSS.StencilEnable = true;
		reflectionsDSS.StencilReadMask = 0xff;
		reflectionsDSS.StencilWriteMask = 0xff;

		reflectionsDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		reflectionsDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		reflectionsDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		reflectionsDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		// We are not rendering backfacing polygons, so these settings do not matter.
		reflectionsDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		reflectionsDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		reflectionsDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		reflectionsDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC drawReflectionsPsoDesc = basePsoDesc;
		drawReflectionsPsoDesc.DepthStencilState = reflectionsDSS;
		drawReflectionsPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		drawReflectionsPsoDesc.RasterizerState.FrontCounterClockwise = true;
		ThrowIfFailed(
			d3dDevice->CreateGraphicsPipelineState(&drawReflectionsPsoDesc,
			                                       IID_PPV_ARGS(&mPSOs["drawStencilReflections"])));

		//
		// PSO for shadow objects
		//

		// We are going to draw shadows with transparency, so base it off the transparency description.
		D3D12_DEPTH_STENCIL_DESC shadowDSS;
		shadowDSS.DepthEnable = true;
		shadowDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		shadowDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		shadowDSS.StencilEnable = true;
		shadowDSS.StencilReadMask = 0xff;
		shadowDSS.StencilWriteMask = 0xff;

		shadowDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		shadowDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		shadowDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		shadowDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		// We are not rendering backfacing polygons, so these settings do not matter.
		shadowDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		shadowDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		shadowDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		shadowDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC shadowPsoDesc = transparentPsoDesc;
		shadowPsoDesc.DepthStencilState = shadowDSS;
		ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&shadowPsoDesc, IID_PPV_ARGS(&mPSOs["shadow"])));
	}

	//execute command list
	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	auto commandList = m_dx12manager->GetD3D12CommandList();

	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* commandLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12StencilShapesPlayground::Update(const CETimer& gt) {
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
	UpdateReflectedPassCB(gt);
	UpdateSkullPlacement(gt);
	// UpdateWaves(gt);
}

void CEDX12StencilShapesPlayground::Render(const CETimer& gt) {
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

	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &mScreenViewport);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &mScissorRect);

	auto trRt = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Indicate a state transition on the resource usage.
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trRt);

	// Clear the back buffer and depth buffer.
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(m_dx12manager->CurrentBackBufferView(),
	                                                            (float*)&mMainPassCB.FogColor, 0, nullptr);
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(m_dx12manager->DepthStencilView(),
	                                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	                                                            1.0f,
	                                                            0,
	                                                            0, nullptr);

	auto currBackBuffView = m_dx12manager->CurrentBackBufferView();
	auto currDepthStencilView = m_dx12manager->DepthStencilView();

	// Specify the buffers we are going to render to.
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &currBackBuffView, true, &currDepthStencilView);

	ID3D12DescriptorHeap* descriptorHeaps[] = {m_dx12manager->GetSRVDescriptorHeap().Get()};
	m_dx12manager->GetD3D12CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	UINT passCBByteSize = (sizeof(Resources::PassConstants) + 255) & ~255;

	// Draw opaque items--floors, walls, skull.
	auto passCB = mCurrFrameResource->PassCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Opaque]);

	// Mark the visible mirror pixels in the stencil buffer with the value 1
	m_dx12manager->GetD3D12CommandList()->OMSetStencilRef(1);
	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["markStencilMirrors"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Mirrors]);

	// Draw the reflection into the mirror only (only for pixels where the stencil buffer is 1).
	// Note that we must supply a different per-pass constant buffer--one with the lights reflected.
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(
		2, passCB->GetGPUVirtualAddress() + 1 * passCBByteSize);
	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["drawStencilReflections"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Reflected]);

	// Restore main pass constants and stencil ref.
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
	m_dx12manager->GetD3D12CommandList()->OMSetStencilRef(0);

	// Draw mirror with transparency so reflection blends through.
	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["transparent"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Transparent]);

	// Draw shadows
	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["shadow"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Shadow]);
	// Indicate a state transition on the resource usage.
	auto trPr = CD3DX12_RESOURCE_BARRIER::Transition(m_dx12manager->CurrentBackBuffer(),
	                                                 D3D12_RESOURCE_STATE_RENDER_TARGET,
	                                                 D3D12_RESOURCE_STATE_PRESENT);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trPr);

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

void CEDX12StencilShapesPlayground::Resize() {
	CEDX12Playground::Resize();
	static const float Pi = 3.1415926535f;
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void CEDX12StencilShapesPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12StencilShapesPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);

	ReleaseCapture();
}

void CEDX12StencilShapesPlayground::OnMouseMove(KeyCode key, int x, int y) {
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

void CEDX12StencilShapesPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	// CEDX12Playground::OnKeyUp(key, keyChar, gt);
	switch (key) {
	case KeyCode::F1:
		mIsWireframe = false;
		break;
	}
}

void CEDX12StencilShapesPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	// CEDX12Playground::OnKeyDown(key, keyChar, gt);
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
		mSkullTranslation.y += 10.0f * dt;
		break;
	case KeyCode::S:
		mSkullTranslation.y -= 10.0f * dt;
		break;
	case KeyCode::A:
		mSkullTranslation.x -= 10.0f * dt;
		break;
	case KeyCode::D:
		mSkullTranslation.x += 10.0f * dt;
		break;
	case KeyCode::F1:
		mIsWireframe = true;
		break;
	}

	mSunPhi = m_dx12manager->Clamp(mSunPhi, 0.1f, XM_PIDIV2);
	UpdateSkullPlacement(gt);
}

void CEDX12StencilShapesPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12StencilShapesPlayground::UpdateCamera(const CETimer& gt) {
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

void CEDX12StencilShapesPlayground::AnimateMaterials(const CETimer& gt) {
	CEDX12Playground::AnimateMaterials(gt);

}

void CEDX12StencilShapesPlayground::UpdateObjectCBs(const CETimer& gt) {
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

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void CEDX12StencilShapesPlayground::UpdateMaterialCBs(const CETimer& gt) {
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

void CEDX12StencilShapesPlayground::UpdateMainPassCB(const CETimer& gt) {
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

	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	// mMainPassCB.Lights[0].Direction = {0.57735f, -0.57735f, 0.57735f};
	// mMainPassCB.Lights[0].Strength = {0.9f, 0.9f, 0.9f};
	// mMainPassCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
	// mMainPassCB.Lights[1].Strength = {0.5f, 0.5f, 0.5f};
	// mMainPassCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};
	// mMainPassCB.Lights[2].Strength = {0.2f, 0.2f, 0.2f};

	XMVECTOR lightDirection = -m_dx12manager->SphericalToCartesian(1.0f, mSunTheta, mSunPhi);

	mMainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	XMStoreFloat3(&mMainPassCB.Lights[0].Direction, lightDirection);
	mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	mMainPassCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
	mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };
	mMainPassCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};
	
	// Main pass stored in index 2
	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CEDX12StencilShapesPlayground::UpdateReflectedPassCB(const CETimer& gt) {
	mReflectedPassCB = mMainPassCB;

	XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	XMMATRIX R = XMMatrixReflect(mirrorPlane);

	// Reflect the lighting.
	for (int i = 0; i < 3; ++i) {
		XMVECTOR lightDir = XMLoadFloat3(&mMainPassCB.Lights[i].Direction);
		XMVECTOR reflectedLightDir = XMVector3TransformNormal(lightDir, R);
		XMStoreFloat3(&mReflectedPassCB.Lights[i].Direction, reflectedLightDir);
	}

	// Reflected pass stored in index 1
	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(1, mReflectedPassCB);
}

void CEDX12StencilShapesPlayground::UpdateSkullPlacement(const CETimer& gt) {
	static const float Pi = 3.1415926535f;
	// Don't let user move below ground plane.
	mSkullTranslation.y = m_dx12manager->Max(mSkullTranslation.y, 0.0f);

	// Update the new world matrix.
	XMMATRIX skullRotate = XMMatrixRotationY(0.5f * Pi);
	XMMATRIX skullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
	XMMATRIX skullOffset = XMMatrixTranslation(mSkullTranslation.x, mSkullTranslation.y, mSkullTranslation.z);
	XMMATRIX skullWorld = skullRotate * skullScale * skullOffset;
	XMStoreFloat4x4(&static_cast<Resources::LitShapesRenderItem*>(mSkullRitem)->World, skullWorld);

	// Update reflection world matrix.
	XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	XMMATRIX R = XMMatrixReflect(mirrorPlane);
	XMStoreFloat4x4(&static_cast<Resources::LitShapesRenderItem*>(mReflectedSkullRitem)->World, skullWorld * R);

	// Update shadow world matrix.
	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
	XMVECTOR toMainLight = -XMLoadFloat3(&mMainPassCB.Lights[0].Direction);
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowOffsetY = XMMatrixTranslation(0.0f, 0.001f, 0.0f);
	XMStoreFloat4x4(&static_cast<Resources::LitShapesRenderItem*>(mShadowedSkullRitem)->World,
	                skullWorld * S * shadowOffsetY);

	static_cast<Resources::LitShapesRenderItem*>(mSkullRitem)->NumFramesDirty = gNumFrameResources;
	static_cast<Resources::LitShapesRenderItem*>(mReflectedSkullRitem)->NumFramesDirty = gNumFrameResources;
	static_cast<Resources::LitShapesRenderItem*>(mShadowedSkullRitem)->NumFramesDirty = gNumFrameResources;
}

void CEDX12StencilShapesPlayground::LoadTextures() {
	auto bricksTex = m_dx12manager->LoadTextureFromFile("bricks3.dds", "bricksTex");
	auto checkboardTex = m_dx12manager->LoadTextureFromFile("checkboard.dds", "checkboardTex");
	auto iceTex = m_dx12manager->LoadTextureFromFile("ice.dds", "iceTex");
	auto white1x1Tex = m_dx12manager->LoadTextureFromFile("white1x1.dds", "white1x1Tex");

	mTextures[bricksTex->Name] = std::move(bricksTex);
	mTextures[checkboardTex->Name] = std::move(checkboardTex);
	mTextures[iceTex->Name] = std::move(iceTex);
	mTextures[white1x1Tex->Name] = std::move(white1x1Tex);
}

void CEDX12StencilShapesPlayground::BuildDescriptorHeaps() {
	m_dx12manager->CreateSRVDescriptorHeap(4);

	auto d3dDevice = m_dx12manager->GetD3D12Device();
	auto size = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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
	hDescriptor.Offset(1, size);

	srvDesc.Format = checkboardTex->GetDesc().Format;
	d3dDevice->CreateShaderResourceView(checkboardTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, size);

	srvDesc.Format = iceTex->GetDesc().Format;
	d3dDevice->CreateShaderResourceView(iceTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, size);

	srvDesc.Format = white1x1Tex->GetDesc().Format;
	d3dDevice->CreateShaderResourceView(white1x1Tex.Get(), &srvDesc, hDescriptor);
}

void CEDX12StencilShapesPlayground::BuildRoomGeometry() {
	// Create and specify geometry.  For this sample we draw a floor
	// and a wall with a mirror on it.  We put the floor, wall, and
	// mirror geometry in one vertex buffer.
	//
	//   |--------------|
	//   |              |
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/

	std::array<Resources::CENormalTextureVertex, 20> vertices =
	{
		// Floor: Observe we tile texture coordinates.
		Resources::CENormalTextureVertex(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f), // 0 
		Resources::CENormalTextureVertex(-3.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f),
		Resources::CENormalTextureVertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f),
		Resources::CENormalTextureVertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f),

		// Wall: Observe we tile texture coordinates, and that we
		// leave a gap in the middle for the mirror.
		Resources::CENormalTextureVertex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 4
		Resources::CENormalTextureVertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		Resources::CENormalTextureVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f),
		Resources::CENormalTextureVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f),

		Resources::CENormalTextureVertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 8 
		Resources::CENormalTextureVertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		Resources::CENormalTextureVertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f),
		Resources::CENormalTextureVertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f),

		Resources::CENormalTextureVertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 12
		Resources::CENormalTextureVertex(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		Resources::CENormalTextureVertex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f),
		Resources::CENormalTextureVertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f),

		// Mirror
		Resources::CENormalTextureVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 16
		Resources::CENormalTextureVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		Resources::CENormalTextureVertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
		Resources::CENormalTextureVertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f)
	};

	std::array<std::int16_t, 30> indices =
	{
		// Floor
		0, 1, 2,
		0, 2, 3,

		// Walls
		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		// Mirror
		16, 17, 18,
		16, 18, 19
	};

	Resources::SubMeshGeometry floorSubmesh;
	floorSubmesh.IndexCount = 6;
	floorSubmesh.StartIndexLocation = 0;
	floorSubmesh.BaseVertexLocation = 0;

	Resources::SubMeshGeometry wallSubmesh;
	wallSubmesh.IndexCount = 18;
	wallSubmesh.StartIndexLocation = 6;
	wallSubmesh.BaseVertexLocation = 0;

	Resources::SubMeshGeometry mirrorSubmesh;
	mirrorSubmesh.IndexCount = 6;
	mirrorSubmesh.StartIndexLocation = 24;
	mirrorSubmesh.BaseVertexLocation = 0;

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalTextureVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "roomGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(),
	                                                          vbByteSize,
	                                                          geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(),
	                                                         ibByteSize,
	                                                         geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalTextureVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["floor"] = floorSubmesh;
	geo->DrawArgs["wall"] = wallSubmesh;
	geo->DrawArgs["mirror"] = mirrorSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12StencilShapesPlayground::BuildSkullGeometry() {
	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream modelsPathStream;
	modelsPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Models\\" <<
		"skull.txt";
	auto modelPath = modelsPathStream.str();
	spdlog::info("Loading Model from TXT: {}", modelPath);

	std::ifstream fin(modelPath);

	if (!fin) {
		spdlog::error("FILE NOT FOUND: {}", modelPath);
		return;
	}


	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Resources::CENormalTextureVertex> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i) {
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

		// Model does not have texture coordinates, so just zero them out.
		vertices[i].TexCoord = {0.0f, 0.0f};
	}

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

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

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

	geo->DrawArgs["skull"] = submesh;

	mGeometries[geo->Name] = std::move(geo);

}

void CEDX12StencilShapesPlayground::BuildMaterials() {
	auto bricks = std::make_unique<Resources::Material>();
	bricks->Name = "bricks";
	bricks->MatCBIndex = 0;
	bricks->DiffuseSrvHeapIndex = 0;
	bricks->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	bricks->Roughness = 0.25f;

	auto checkertile = std::make_unique<Resources::Material>();
	checkertile->Name = "checkertile";
	checkertile->MatCBIndex = 1;
	checkertile->DiffuseSrvHeapIndex = 1;
	checkertile->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	checkertile->FresnelR0 = XMFLOAT3(0.07f, 0.07f, 0.07f);
	checkertile->Roughness = 0.3f;

	auto icemirror = std::make_unique<Resources::Material>();
	icemirror->Name = "icemirror";
	icemirror->MatCBIndex = 2;
	icemirror->DiffuseSrvHeapIndex = 2;
	icemirror->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	icemirror->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	icemirror->Roughness = 0.5f;

	auto skullMat = std::make_unique<Resources::Material>();
	skullMat->Name = "skullMat";
	skullMat->MatCBIndex = 3;
	skullMat->DiffuseSrvHeapIndex = 3;
	skullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	skullMat->Roughness = 0.3f;

	auto shadowMat = std::make_unique<Resources::Material>();
	shadowMat->Name = "shadowMat";
	shadowMat->MatCBIndex = 4;
	shadowMat->DiffuseSrvHeapIndex = 3;
	shadowMat->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	shadowMat->FresnelR0 = XMFLOAT3(0.001f, 0.001f, 0.001f);
	shadowMat->Roughness = 0.0f;

	mMaterials["bricks"] = std::move(bricks);
	mMaterials["checkertile"] = std::move(checkertile);
	mMaterials["icemirror"] = std::move(icemirror);
	mMaterials["skullMat"] = std::move(skullMat);
	mMaterials["shadowMat"] = std::move(shadowMat);
}

void CEDX12StencilShapesPlayground::BuildRenderItems() {
	auto floorRitem = std::make_unique<Resources::LitShapesRenderItem>();
	floorRitem->World = Resources::MatrixIdentity4X4();
	floorRitem->TexTransform = Resources::MatrixIdentity4X4();
	floorRitem->ObjCBIndex = 0;
	floorRitem->Mat = mMaterials["checkertile"].get();
	floorRitem->Geo = mGeometries["roomGeo"].get();
	floorRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	floorRitem->IndexCount = floorRitem->Geo->DrawArgs["floor"].IndexCount;
	floorRitem->StartIndexLocation = floorRitem->Geo->DrawArgs["floor"].StartIndexLocation;
	floorRitem->BaseVertexLocation = floorRitem->Geo->DrawArgs["floor"].BaseVertexLocation;
	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(floorRitem.get());

	auto wallsRitem = std::make_unique<Resources::LitShapesRenderItem>();
	wallsRitem->World = Resources::MatrixIdentity4X4();
	wallsRitem->TexTransform = Resources::MatrixIdentity4X4();
	wallsRitem->ObjCBIndex = 1;
	wallsRitem->Mat = mMaterials["bricks"].get();
	wallsRitem->Geo = mGeometries["roomGeo"].get();
	wallsRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wallsRitem->IndexCount = wallsRitem->Geo->DrawArgs["wall"].IndexCount;
	wallsRitem->StartIndexLocation = wallsRitem->Geo->DrawArgs["wall"].StartIndexLocation;
	wallsRitem->BaseVertexLocation = wallsRitem->Geo->DrawArgs["wall"].BaseVertexLocation;
	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(wallsRitem.get());

	auto skullRitem = std::make_unique<Resources::LitShapesRenderItem>();
	skullRitem->World = Resources::MatrixIdentity4X4();
	skullRitem->TexTransform = Resources::MatrixIdentity4X4();
	skullRitem->ObjCBIndex = 2;
	skullRitem->Mat = mMaterials["skullMat"].get();
	skullRitem->Geo = mGeometries["skullGeo"].get();
	skullRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	skullRitem->IndexCount = skullRitem->Geo->DrawArgs["skull"].IndexCount;
	skullRitem->StartIndexLocation = skullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
	skullRitem->BaseVertexLocation = skullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;
	mSkullRitem = skullRitem.get();
	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(skullRitem.get());

	// Reflected skull will have different world matrix, so it needs to be its own render item.
	auto reflectedSkullRitem = std::make_unique<Resources::LitShapesRenderItem>();
	*reflectedSkullRitem = *skullRitem;
	reflectedSkullRitem->ObjCBIndex = 3;
	mReflectedSkullRitem = reflectedSkullRitem.get();
	mRitemLayer[(int)Resources::RenderLayer::Reflected].push_back(reflectedSkullRitem.get());

	// Shadowed skull will have different world matrix, so it needs to be its own render item.
	auto shadowedSkullRitem = std::make_unique<Resources::LitShapesRenderItem>();
	*shadowedSkullRitem = *skullRitem;
	shadowedSkullRitem->ObjCBIndex = 4;
	shadowedSkullRitem->Mat = mMaterials["shadowMat"].get();
	mShadowedSkullRitem = shadowedSkullRitem.get();
	mRitemLayer[(int)Resources::RenderLayer::Shadow].push_back(shadowedSkullRitem.get());

	auto mirrorRitem = std::make_unique<Resources::LitShapesRenderItem>();
	mirrorRitem->World = Resources::MatrixIdentity4X4();
	mirrorRitem->TexTransform = Resources::MatrixIdentity4X4();
	mirrorRitem->ObjCBIndex = 5;
	mirrorRitem->Mat = mMaterials["icemirror"].get();
	mirrorRitem->Geo = mGeometries["roomGeo"].get();
	mirrorRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mirrorRitem->IndexCount = mirrorRitem->Geo->DrawArgs["mirror"].IndexCount;
	mirrorRitem->StartIndexLocation = mirrorRitem->Geo->DrawArgs["mirror"].StartIndexLocation;
	mirrorRitem->BaseVertexLocation = mirrorRitem->Geo->DrawArgs["mirror"].BaseVertexLocation;
	mRitemLayer[(int)Resources::RenderLayer::Mirrors].push_back(mirrorRitem.get());
	mRitemLayer[(int)Resources::RenderLayer::Transparent].push_back(mirrorRitem.get());

	mAllRitems.push_back(std::move(floorRitem));
	mAllRitems.push_back(std::move(wallsRitem));
	mAllRitems.push_back(std::move(skullRitem));
	mAllRitems.push_back(std::move(reflectedSkullRitem));
	mAllRitems.push_back(std::move(shadowedSkullRitem));
	mAllRitems.push_back(std::move(mirrorRitem));
}

void CEDX12StencilShapesPlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<Resources::CEFrameResource>(
			m_dx12manager->GetD3D12Device().Get(),
			2,
			(UINT)mAllRitems.size(),
			(UINT)mMaterials.size(),
			(UINT)0
		));
	}
}

void CEDX12StencilShapesPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
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

float CEDX12StencilShapesPlayground::GetHillsHeight(float x, float z) const {
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 CEDX12StencilShapesPlayground::GetHillsNormal(float x, float z) const {
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void CEDX12StencilShapesPlayground::UpdateWaves(const CETimer& gt) {
	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if ((gt.TotalTime() - t_base) >= 0.25f) {
		t_base += 0.25f;

		int i = m_dx12manager->Rand(4, m_waves->RowCount() - 5);
		int j = m_dx12manager->Rand(4, m_waves->ColumnCount() - 5);

		float r = m_dx12manager->RandF(0.2f, 0.5f);

		m_waves->Disturb(i, j, r);
	}

	// Update the wave simulation.
	m_waves->Update(gt.DeltaTime());

	// Update the wave vertex buffer with the new solution.
	auto currWavesVB = mCurrFrameResource->NormalTextureWavesVB.get();
	for (int i = 0; i < m_waves->VertexCount(); ++i) {
		Resources::CENormalTextureVertex v;

		v.Pos = m_waves->Position(i);
		v.Normal = m_waves->Normal(i);

		// Derive tex-coords from position by 
		// mapping [-w/2,w/2] --> [0,1]
		v.TexCoord.x = 0.5f + v.Pos.x / m_waves->Width();
		v.TexCoord.y = 0.5f - v.Pos.z / m_waves->Depth();

		currWavesVB->CopyData(i, v);
	}

	// Set the dynamic VB of the wave renderitem to the current frame VB.
	static_cast<Resources::LitShapesRenderItem*>(mWavesRitem)->Geo->VertexBufferGPU = currWavesVB->Resource();
}
