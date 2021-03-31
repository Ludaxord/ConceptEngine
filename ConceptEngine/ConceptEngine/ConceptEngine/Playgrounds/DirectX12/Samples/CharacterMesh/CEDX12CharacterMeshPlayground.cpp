#include "CEDX12CharacterMeshPlayground.h"

#include <fstream>

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEModelLoader.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEShadowMap.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CESSAO.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/ModelsInstances/CESkinnedModel.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12CharacterMeshPlayground::CEDX12CharacterMeshPlayground() : CEDX12Playground() {
	/*
	 * Estimate scene bounding sphere manually since we know how scene was constructed
	 * The grid is "widest object" with a width of 20 and depth of 30.0f, and cetered at the world space origin.
	 * In general, you need to loop over every world space vertex position and compute bounding sphere
	 */
	mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSceneBounds.Radius = sqrtf(10.0f * 10.0f + 15.0f * 15.0f);
}

void CEDX12CharacterMeshPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	m_camera.SetPosition(0.0f, 2.0f, -15.0f);

	m_shadowMap = std::make_unique<Resources::CEShadowMap>(m_dx12manager->GetD3D12Device().Get(), 2048, 2048);

	m_ssao = std::make_unique<Resources::CESSAO>(m_dx12manager->GetD3D12Device().Get(),
	                                             m_dx12manager->GetD3D12CommandList().Get(),
	                                             m_dx12manager->GetWindowWidth(),
	                                             m_dx12manager->GetWindowHeight());

	LoadSkinnedModel();
	LoadTextures();
	// BuildRootSignature
	{
		CD3DX12_DESCRIPTOR_RANGE texTable0;
		texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 0);

		CD3DX12_DESCRIPTOR_RANGE texTable1;
		texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 48, 3, 0);

		//Root parameter can be a table, root descriptor or root constants
		CD3DX12_ROOT_PARAMETER slotRootParameter[6];

		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstantBufferView(1);
		slotRootParameter[2].InitAsConstantBufferView(2);
		slotRootParameter[3].InitAsShaderResourceView(0, 1);
		slotRootParameter[4].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[5].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

		auto staticSamplers = m_dx12manager->GetStaticSamplers();

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(6, slotRootParameter,
		                                        (UINT)staticSamplers.size(), staticSamplers.data(),
		                                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		m_rootSignature = m_dx12manager->CreateRootSignature(&rootSigDesc);
	}
	//Build SSAO RootSignature
	{
		CD3DX12_DESCRIPTOR_RANGE texTable0;
		texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

		CD3DX12_DESCRIPTOR_RANGE texTable1;
		texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);

		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstants(1, 1);
		slotRootParameter[2].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[3].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
			0, //ShaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, //addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, //addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP //addressW
		);
		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
			1, //ShaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, //addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, //addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP //addressW
		);
		const CD3DX12_STATIC_SAMPLER_DESC depthMapSam(
			2, //ShaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_BORDER, //addressU
			D3D12_TEXTURE_ADDRESS_MODE_BORDER, //addressV
			D3D12_TEXTURE_ADDRESS_MODE_BORDER, //addressW
			0.0f,
			0,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		);
		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
			3, //ShaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, //addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, //addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP //addressW
		);

		std::array<CD3DX12_STATIC_SAMPLER_DESC, 4> staticSamplers = {
			pointClamp,
			linearClamp,
			depthMapSam,
			linearWrap
		};

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4,
		                                        slotRootParameter,
		                                        (UINT)staticSamplers.size(),
		                                        staticSamplers.data(),
		                                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		m_ssaoRootSignature = m_dx12manager->CreateRootSignature(&rootSigDesc);
	}
	BuildDescriptorHeaps();
	// BuildShaders
	{
		const D3D_SHADER_MACRO alphaDefines[] = {
			"ALPHA", "1",
			"SSAO", "1",
			NULL, NULL
		};

		const D3D_SHADER_MACRO modelDefines[] = {
			"MODEL", "1",
			NULL, NULL
		};
		// ================= Standard
		m_shadersMap["standardVS"] = m_dx12manager->CompileShaders("CEBaseShadowVertexShader.hlsl",
		                                                           alphaDefines,
		                                                           "VS",
		                                                           // "vs_6_3"
		                                                           "vs_5_1"
		);
		m_shadersMap["modelVS"] = m_dx12manager->CompileShaders("CEBaseModelVertexShader.hlsl",
		                                                        modelDefines,
		                                                        "VS",
		                                                        // "vs_6_3"
		                                                        "vs_5_1"
		);
		m_shadersMap["opaquePS"] = m_dx12manager->CompileShaders("CEBaseShadowPixelShader.hlsl",
		                                                         alphaDefines,
		                                                         "PS",
		                                                         // "ps_6_3"
		                                                         "ps_5_1"
		);
		// ================= Shadow
		m_shadersMap["shadowVS"] = m_dx12manager->CompileShaders("CEShadowVertexShader.hlsl",
		                                                         alphaDefines,
		                                                         "VS",
		                                                         // "vs_6_3"
		                                                         "vs_5_1"
		);
		m_shadersMap["modelShadowVS"] = m_dx12manager->CompileShaders("CEModelShadowVertexShader.hlsl",
		                                                              modelDefines,
		                                                              "VS",
		                                                              // "vs_6_3"
		                                                              "vs_5_1"
		);
		m_shadersMap["shadowOpaquePS"] = m_dx12manager->CompileShaders("CEShadowPixelShader.hlsl",
		                                                               alphaDefines,
		                                                               "PS",
		                                                               // "ps_6_3"
		                                                               "ps_5_1"
		);
		m_shadersMap["shadowAlphaPS"] = m_dx12manager->CompileShaders("CEShadowPixelShader.hlsl",
		                                                              alphaDefines,
		                                                              "PS",
		                                                              // "ps_6_3"
		                                                              "ps_5_1"
		);
		// ================= Debug
		m_shadersMap["debugVS"] = m_dx12manager->CompileShaders("CEShadowDebugVertexShader.hlsl",
		                                                        alphaDefines,
		                                                        "VS",
		                                                        // "vs_6_3"
		                                                        "vs_5_1"
		);
		m_shadersMap["debugPS"] = m_dx12manager->CompileShaders("CEShadowDebugPixelShader.hlsl",
		                                                        alphaDefines,
		                                                        "PS",
		                                                        // "ps_6_3"
		                                                        "ps_5_1"
		);
		// ================= Draw Normals
		m_shadersMap["drawNormalsVS"] = m_dx12manager->CompileShaders("CEDrawNormalsVertexShader.hlsl",
		                                                              alphaDefines,
		                                                              "VS",
		                                                              // "vs_6_3"
		                                                              "vs_5_1"
		);
		m_shadersMap["modelDrawNormalsVS"] = m_dx12manager->CompileShaders("CEDrawNormalsVertexShader.hlsl",
		                                                                   alphaDefines,
		                                                                   "VS",
		                                                                   // "vs_6_3"
		                                                                   "vs_5_1"
		);
		m_shadersMap["drawNormalsPS"] = m_dx12manager->CompileShaders("CEDrawNormalsPixelShader.hlsl",
		                                                              alphaDefines,
		                                                              "PS",
		                                                              // "ps_6_3"
		                                                              "ps_5_1"
		);
		// ================= SSAO
		m_shadersMap["ssaoVS"] = m_dx12manager->CompileShaders("CESSAOVertexShader.hlsl",
		                                                       alphaDefines,
		                                                       "VS",
		                                                       // "vs_6_3"
		                                                       "vs_5_1"
		);
		m_shadersMap["ssaoPS"] = m_dx12manager->CompileShaders("CESSAOPixelShader.hlsl",
		                                                       alphaDefines,
		                                                       "PS",
		                                                       // "ps_6_3"
		                                                       "ps_5_1"
		);
		// ================= SSAO Blur
		m_shadersMap["ssaoBlurVS"] = m_dx12manager->CompileShaders("CESSAOBlurVertexShader.hlsl",
		                                                           alphaDefines,
		                                                           "VS",
		                                                           // "vs_6_3"
		                                                           "vs_5_1"
		);
		m_shadersMap["ssaoBlurPS"] = m_dx12manager->CompileShaders("CESSAOBlurPixelShader.hlsl",
		                                                           alphaDefines,
		                                                           "PS",
		                                                           // "ps_6_3"
		                                                           "ps_5_1"
		);
		// ================= Sky
		m_shadersMap["skyVS"] = m_dx12manager->CompileShaders("CECubeMapVertexShader.hlsl",
		                                                      alphaDefines,
		                                                      "VS",
		                                                      // "vs_6_3"
		                                                      "vs_5_1"
		);
		m_shadersMap["skyPS"] = m_dx12manager->CompileShaders("CECubeMapPixelShader.hlsl",
		                                                      alphaDefines,
		                                                      "PS",
		                                                      // "ps_6_3"
		                                                      "ps_5_1"
		);
	}
	//Build Input Layout
	{
		m_inputLayout = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};
		m_modelInputLayout = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};
	}
	BuildShapesGeometry();
	BuildModelGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs(m_rootSignature);

	m_ssao->SetPSOs(mPSOs["ssao"].Get(), mPSOs["ssaoBlur"].Get());

	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());
	m_dx12manager->ExecuteCommandLists();

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12CharacterMeshPlayground::Update(const CETimer& gt) {
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


	//
	// Animate the lights (and hence shadows).
	//

	mLightRotationAngle += 0.1f * gt.DeltaTime();

	XMMATRIX R = XMMatrixRotationY(mLightRotationAngle);
	for (int i = 0; i < 3; ++i) {
		XMVECTOR lightDir = XMLoadFloat3(&mBaseLightDirections[i]);
		lightDir = XMVector3TransformNormal(lightDir, R);
		XMStoreFloat3(&mRotatedLightDirections[i], lightDir);
	}

	AnimateMaterials(gt);
	UpdateObjectCBs(gt);
	UpdateSkinnedModelCBs(gt);
	UpdateMaterialCBs(gt);
	UpdateShadowTransform(gt);
	UpdateMainPassCB(gt);
	UpdateShadowPassCB(gt);
	UpdateSSAOCB(gt);
}

void CEDX12CharacterMeshPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);


	auto cmdListAlloc = mCurrFrameResource->commandAllocator;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	// ================================= TODO: CODE HERE....

	ID3D12DescriptorHeap* descriptorHeaps[] = {m_dx12manager->GetSRVDescriptorHeap().Get()};
	m_dx12manager->GetD3D12CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	/*
	 * Shadow map pass
	 */

	//Bind all materials used in this scene. For structured buffers, we can bypass heap and set as root descriptor
	auto matBuffer = mCurrFrameResource->MaterialIndexBuffer->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

	//Bind null SRV for shadow map pass
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(4, m_nullSrv);

	//Bind all textures used in this scene . Observe that we only have to specify the first descriptor in table.
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(
		5, m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	DrawSceneToShadowMap();

	/*
	 * Normal/Depth pass
	 */
	DrawNormalsAndDepth();

	/*
	 * Compute SSAO
	 */

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_ssaoRootSignature.Get());
	m_ssao->ComputeSSAO(m_dx12manager->GetD3D12CommandList().Get(), mCurrFrameResource, 2);

	/*
	 * Main rendering pass
	 */
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	//Bind all materials used in this scene. For structured buffers, we can bypass heap and set as root descriptor
	matBuffer = mCurrFrameResource->MaterialIndexBuffer->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

	auto mScreenViewport = m_dx12manager->GetViewPort();
	auto mScissorRect = m_dx12manager->GetScissorRect();
	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &mScreenViewport);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &mScissorRect);

	auto transitionPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &transitionPresentRenderTarget);

	//Clear back buffer and depth buffer
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(m_dx12manager->CurrentBackBufferView(),
	                                                            Colors::LightSteelBlue,
	                                                            0,
	                                                            nullptr);

	// WE ALREADY WROTE THE DEPTH INFO TO THE DEPTH BUFFER IN DrawNormalsAndDepth,
	// SO DO NOT CLEAR DEPTH.
	// 
	/*
	 *
		m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(m_dx12manager->DepthStencilView(),
																D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
																1.0f,
																0,
																0,
																nullptr);
	 */


	//specify buffers we are going to render to.
	auto cbbv = m_dx12manager->CurrentBackBufferView();
	auto dsv = m_dx12manager->DepthStencilView();
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &cbbv, true, &dsv);

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(
		5, m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	auto passCB = mCurrFrameResource->PassSSAOCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	// Bind the sky cube map.  For our demos, we just use one "world" cube map representing the environment
	// from far away, so all objects will use the same cube map and we only need to set it once per-frame.  
	// If we wanted to use "local" cube maps, we would have to change them per-object, or dynamically
	// index into an array of cube maps.

	CD3DX12_GPU_DESCRIPTOR_HANDLE skyTexDescriptor(
		m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	skyTexDescriptor.Offset(m_skyTexHeapIndex,
	                        m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(4, skyTexDescriptor);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["opaque"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Opaque]);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["debug"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Debug]);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["sky"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Sky]);

	//Indicate a state transition on resource usage.
	auto transitionRenderTargetPresent = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &transitionRenderTargetPresent);

	// ================================= CODE END HERE

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

void CEDX12CharacterMeshPlayground::Resize() {
	CEDX12Playground::Resize();
	m_camera.SetLens(0.25f * Resources::Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);

	if (m_ssao != nullptr) {
		m_ssao->Resize(m_dx12manager->GetWindowWidth(), m_dx12manager->GetWindowHeight());

		//Resource changed, so need to rebuild descriptors
		m_ssao->RebuildDescriptors(m_dx12manager->GetDepthStencilBuffer().Get());
	}
}

void CEDX12CharacterMeshPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12CharacterMeshPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
	ReleaseCapture();
}

void CEDX12CharacterMeshPlayground::OnMouseMove(KeyCode key, int x, int y) {
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

void CEDX12CharacterMeshPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12CharacterMeshPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyDown(key, keyChar, gt);
	const float dt = gt.DeltaTime();
	switch (key) {
	case KeyCode::A:
		m_camera.Strafe(-10.0f * dt);
		break;
	case KeyCode::D:
		m_camera.Strafe(10.0f * dt);
		break;
	case KeyCode::W:
		m_camera.Walk(10.0f * dt);
		break;
	case KeyCode::S:
		m_camera.Walk(-10.0f * dt);
		break;
	}
}

void CEDX12CharacterMeshPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12CharacterMeshPlayground::UpdateObjectCBs(const CETimer& gt) {
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

void CEDX12CharacterMeshPlayground::UpdateMainPassCB(const CETimer& gt) {
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

	//Transform NDC space [-1, +1]^2 to texture space [0, 1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);

	XMMATRIX viewProjTex = XMMatrixMultiply(viewProj, T);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProjTex, XMMatrixTranspose(viewProjTex));
	XMStoreFloat4x4(&mMainPassCB.ShadowTransform, XMMatrixTranspose(shadowTransform));
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
	mMainPassCB.Lights[0].Direction = mRotatedLightDirections[0];
	mMainPassCB.Lights[0].Strength = {0.9f, 0.8f, 0.7f};
	mMainPassCB.Lights[1].Direction = mRotatedLightDirections[1];
	mMainPassCB.Lights[1].Strength = {0.4f, 0.4f, 0.4f};
	mMainPassCB.Lights[2].Direction = mRotatedLightDirections[2];
	mMainPassCB.Lights[2].Strength = {0.2f, 0.2f, 0.2f};

	auto currPassCB = mCurrFrameResource->PassSSAOCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CEDX12CharacterMeshPlayground::UpdateMaterialCBs(const CETimer& gt) {
	CEDX12Playground::UpdateMaterialCBs(gt);
	auto currMaterialBuffer = mCurrFrameResource->MaterialIndexBuffer.get();
	for (auto& e : mMaterials) {
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		Resources::Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0) {
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			Resources::MaterialIndexData matData;
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;
			matData.NormalMapIndex = mat->NormalSrvHeapIndex;

			currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}
}

void CEDX12CharacterMeshPlayground::BuildPSOs(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature) {
	auto d3dDevice = m_dx12manager->GetD3D12Device();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC basePsoDesc;

	/*
	 * PSO for opaque objects
	 */
	ZeroMemory(&basePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	basePsoDesc.InputLayout = {m_inputLayout.data(), (UINT)m_inputLayout.size()};
	basePsoDesc.pRootSignature = rootSignature.Get();
	basePsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shadersMap["standardVS"]->GetBufferPointer()),
		m_shadersMap["standardVS"]->GetBufferSize()
	};
	basePsoDesc.PS = {
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
	basePsoDesc.SampleDesc.Quality = m_dx12manager->GetM4XMSAAState() ? (m_dx12manager->GetM4XMSAAQuality() - 1) : 0;
	basePsoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();

	/*
	 * PSO for Opaque Objects
	 */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc = basePsoDesc;
	opaquePsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
	opaquePsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC modelOpaquePsoDesc = opaquePsoDesc;
	modelOpaquePsoDesc.InputLayout = {m_modelInputLayout.data(), (UINT)m_modelInputLayout.size()};
	modelOpaquePsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shadersMap["modelVS"]->GetBufferPointer()),
		m_shadersMap["modelVS"]->GetBufferSize()
	};
	modelOpaquePsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_shadersMap["opaquePS"]->GetBufferPointer()),
		m_shadersMap["opaquePS"]->GetBufferSize()
	};
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&modelOpaquePsoDesc, IID_PPV_ARGS(&mPSOs["modelOpaque"])));

	//
	// PSO for shadow map pass.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC smapPsoDesc = basePsoDesc;
	smapPsoDesc.RasterizerState.DepthBias = 100000;
	smapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	smapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	smapPsoDesc.pRootSignature = m_rootSignature.Get();
	smapPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_shadersMap["shadowVS"]->GetBufferPointer()),
		m_shadersMap["shadowVS"]->GetBufferSize()
	};
	smapPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_shadersMap["shadowOpaquePS"]->GetBufferPointer()),
		m_shadersMap["shadowOpaquePS"]->GetBufferSize()
	};

	// Shadow map pass does not have a render target.
	smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	smapPsoDesc.NumRenderTargets = 0;
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(&mPSOs["shadow_opaque"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC modelSmapPsoDesc = smapPsoDesc;
	modelSmapPsoDesc.InputLayout = {};
	modelSmapPsoDesc.VS = {};
	modelSmapPsoDesc.PS = {};
	ThrowIfFailed(
		d3dDevice->CreateGraphicsPipelineState(&modelSmapPsoDesc, IID_PPV_ARGS(&mPSOs["modelShadow_opaque"]))
	);

	//
	// PSO for debug layer.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC debugPsoDesc = basePsoDesc;
	debugPsoDesc.pRootSignature = m_rootSignature.Get();
	debugPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_shadersMap["debugVS"]->GetBufferPointer()),
		m_shadersMap["debugVS"]->GetBufferSize()
	};
	debugPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_shadersMap["debugPS"]->GetBufferPointer()),
		m_shadersMap["debugPS"]->GetBufferSize()
	};
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&debugPsoDesc, IID_PPV_ARGS(&mPSOs["debug"])));

	/*
	 * PSO for drawing normals
	 */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC drawNormalsPsoDesc = basePsoDesc;
	drawNormalsPsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shadersMap["drawNormalsVS"]->GetBufferPointer()),
		m_shadersMap["drawNormalsVS"]->GetBufferSize()
	};
	drawNormalsPsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_shadersMap["drawNormalsPS"]->GetBufferPointer()),
		m_shadersMap["drawNormalsPS"]->GetBufferSize()
	};
	drawNormalsPsoDesc.RTVFormats[0] = Resources::CESSAO::NormalMapFormat;
	drawNormalsPsoDesc.SampleDesc.Count = 1;
	drawNormalsPsoDesc.SampleDesc.Quality = 0;
	drawNormalsPsoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&drawNormalsPsoDesc, IID_PPV_ARGS(&mPSOs["drawNormals"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC modelDrawNormalsPsoDesc = drawNormalsPsoDesc;
	modelDrawNormalsPsoDesc.InputLayout = {};
	modelDrawNormalsPsoDesc.VS = {};
	modelDrawNormalsPsoDesc.PS = {};
	ThrowIfFailed(
		d3dDevice->CreateGraphicsPipelineState(&modelDrawNormalsPsoDesc, IID_PPV_ARGS(&mPSOs["modelDrawNormals"])));

	/*
	 * PSO for SSAO
	 */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC ssaoPsoDesc = basePsoDesc;
	ssaoPsoDesc.InputLayout = {nullptr, 0};
	ssaoPsoDesc.pRootSignature = m_ssaoRootSignature.Get();
	ssaoPsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shadersMap["ssaoVS"]->GetBufferPointer()),
		m_shadersMap["ssaoVS"]->GetBufferSize()
	};
	ssaoPsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_shadersMap["ssaoPS"]->GetBufferPointer()),
		m_shadersMap["ssaoPS"]->GetBufferSize()
	};

	// SSAO effect does not need depth buffer
	ssaoPsoDesc.DepthStencilState.DepthEnable = false;
	ssaoPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	ssaoPsoDesc.RTVFormats[0] = Resources::CESSAO::AmbientMapFormat;
	ssaoPsoDesc.SampleDesc.Count = 1;
	ssaoPsoDesc.SampleDesc.Quality = 0;
	ssaoPsoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&ssaoPsoDesc, IID_PPV_ARGS(&mPSOs["ssao"])));

	/*
	 * PSO for SSAO Blur
	 */
	D3D12_GRAPHICS_PIPELINE_STATE_DESC ssaoBlurPsoDesc = ssaoPsoDesc;
	ssaoBlurPsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shadersMap["ssaoBlurVS"]->GetBufferPointer()),
		m_shadersMap["ssaoBlurVS"]->GetBufferSize()
	};
	ssaoBlurPsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_shadersMap["ssaoBlurPS"]->GetBufferPointer()),
		m_shadersMap["ssaoBlurPS"]->GetBufferSize()
	};
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&ssaoBlurPsoDesc, IID_PPV_ARGS(&mPSOs["ssaoBlur"])));

	//
	// PSO for sky.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = basePsoDesc;

	// The camera is inside the sky sphere, so just turn off culling.
	skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	skyPsoDesc.pRootSignature = m_rootSignature.Get();
	skyPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_shadersMap["skyVS"]->GetBufferPointer()),
		m_shadersMap["skyVS"]->GetBufferSize()
	};
	skyPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_shadersMap["skyPS"]->GetBufferPointer()),
		m_shadersMap["skyPS"]->GetBufferSize()
	};
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&mPSOs["sky"])));

}

void CEDX12CharacterMeshPlayground::LoadTextures() {
	std::unordered_map<std::string, std::string> textures = {
		{"bricksDiffuseMap", "bricks2.dds"},
		{"bricksNormalMap", "bricks2_nmap.dds"},
		{"tileDiffuseMap", "tile.dds"},
		{"tileNormalMap", "tile_nmap.dds"},
		{"defaultDiffuseMap", "white1x1.dds"},
		{"defaultNormalMap", "default_nmap.dds"},
		// {"skyCubeMap", "grasscube1024.dds"}
		{"skyCubeMap", "desertcube1024.dds"}
		// {"skyCubeMap", "sunsetcube1024.dds"}
	};

	for (auto texPair : textures) {
		auto tex = m_dx12manager->LoadTextureFromFile(texPair.second, texPair.first);
		mTextures[tex->Name] = std::move(tex);
	}
}

void CEDX12CharacterMeshPlayground::BuildDescriptorHeaps() {
	m_dx12manager->CreateSRVDescriptorHeap(64);

	auto srvSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto dsvSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	auto rtvSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	auto srvHeap = m_dx12manager->GetSRVDescriptorHeap();
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		srvHeap->GetCPUDescriptorHandleForHeapStart());

	std::vector<ComPtr<ID3D12Resource>> tex2DList =
	{
		mTextures["bricksDiffuseMap"]->Resource,
		mTextures["bricksNormalMap"]->Resource,
		mTextures["tileDiffuseMap"]->Resource,
		mTextures["tileNormalMap"]->Resource,
		mTextures["defaultDiffuseMap"]->Resource,
		mTextures["defaultNormalMap"]->Resource
	};

	m_modelSrvHeapStart = (UINT)tex2DList.size();

	for (UINT i = 0; i < (UINT)m_modelTextureNames.size(); ++i) {
		auto texResource = mTextures[m_modelTextureNames[i]]->Resource;
		assert(texResource != nullptr);
		tex2DList.push_back(texResource);
	}

	auto skyCubeMap = mTextures["skyCubeMap"]->Resource;

	auto d3dDevice = m_dx12manager->GetD3D12Device();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	for (UINT i = 0; i < (UINT)tex2DList.size(); ++i) {
		srvDesc.Format = tex2DList[i]->GetDesc().Format;
		srvDesc.Texture2D.MipLevels = tex2DList[i]->GetDesc().MipLevels;
		d3dDevice->CreateShaderResourceView(tex2DList[i].Get(), &srvDesc, hDescriptor);

		// next descriptor
		hDescriptor.Offset(1, srvSize);
	}

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = skyCubeMap->GetDesc().MipLevels;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Format = skyCubeMap->GetDesc().Format;
	d3dDevice->CreateShaderResourceView(skyCubeMap.Get(), &srvDesc, hDescriptor);

	m_skyTexHeapIndex = (UINT)tex2DList.size();
	m_shadowMapHeapIndex = m_skyTexHeapIndex + 1;

	m_ssaoHeapIndexStart = m_shadowMapHeapIndex + 1;
	m_ssaoAmbientMapIndex = m_ssaoHeapIndexStart + 3;

	m_nullCubeSrvIndex = m_ssaoHeapIndexStart + 5;
	m_nullTexSrvIndex1 = m_nullCubeSrvIndex + 1;
	m_nullTexSrvIndex2 = m_nullTexSrvIndex1 + 1;

	auto nullSrv = m_dx12manager->GetCpuSRV(m_nullCubeSrvIndex);
	m_nullSrv = m_dx12manager->GetGpuSRV(m_nullCubeSrvIndex);

	d3dDevice->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);
	nullSrv.Offset(1, srvSize);

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	d3dDevice->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);

	nullSrv.Offset(1, srvSize);
	d3dDevice->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);

	m_shadowMap->BuildDescriptors(
		m_dx12manager->GetCpuSRV(m_shadowMapHeapIndex),
		m_dx12manager->GetGpuSRV(m_shadowMapHeapIndex),
		m_dx12manager->GetDSV(1)
	);

	m_ssao->BuildDescriptors(
		m_dx12manager->GetDepthStencilBuffer().Get(),
		m_dx12manager->GetCpuSRV(m_ssaoHeapIndexStart),
		m_dx12manager->GetGpuSRV(m_ssaoHeapIndexStart),
		m_dx12manager->GetRTV(m_dx12manager->BufferCount),
		srvSize,
		rtvSize
	);

}

void CEDX12CharacterMeshPlayground::BuildModelGeometry() {
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

	std::vector<Resources::CENormalTextureTangentVertex> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i) {
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

		vertices[i].TexCoord = {0.0f, 0.0f};

		XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);

		XMVECTOR N = XMLoadFloat3(&vertices[i].Normal);

		// Generate a tangent vector so normal mapping works.  We aren't applying
		// a texture map to the skull, so we just need any tangent vector so that
		// the math works out to give us the original interpolated vertex normal.
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		if (fabsf(XMVectorGetX(XMVector3Dot(N, up))) < 1.0f - 0.001f) {
			XMVECTOR T = XMVector3Normalize(XMVector3Cross(up, N));
			XMStoreFloat3(&vertices[i].TangentU, T);
		}
		else {
			up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			XMVECTOR T = XMVector3Normalize(XMVector3Cross(N, up));
			XMStoreFloat3(&vertices[i].TangentU, T);
		}


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

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalTextureTangentVertex);

	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::int32_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "skullGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalTextureTangentVertex);
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

void CEDX12CharacterMeshPlayground::LoadSkinnedModel() {
	std::vector<Resources::CEModelLoader::ModelVertex> vertices;
	std::vector<std::uint16_t> indices;

	std::string modelFileName = "soldier.m3d";
	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream modelPathStream;
	modelPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Models\\" <<
		modelFileName;
	auto modelPath = modelPathStream.str();

	Resources::CEModelLoader modelLoader;
	modelLoader.LoadM3D(modelPath, vertices, indices, m_modelSubsets, m_modelMaterials, m_modelInfo);

	m_model = std::make_unique<Resources::ModelsInstances::CESkinnedModel>();
	m_model->ModelInfo = &m_modelInfo;
	m_model->FinalTransforms.resize(m_modelInfo.ElementCount());
	m_model->ClipName = "Take1";
	m_model->TimePos = 0.0f;

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CEModelVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = modelFileName;

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CEModelVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	for (UINT i = 0; i < (UINT)m_modelSubsets.size(); ++i) {
		Resources::SubMeshGeometry subMesh;
		std::string name = "sm_" + std::to_string(i);

		subMesh.IndexCount = (UINT)m_modelSubsets[i].FaceCount * 3;
		subMesh.StartIndexLocation = m_modelSubsets[i].FaceStart * 3;
		subMesh.BaseVertexLocation = 0;

		geo->DrawArgs[name] = subMesh;
	}

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12CharacterMeshPlayground::BuildShapesGeometry() {
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
	GeometryGenerator::MeshData quad = geoGen.CreateQuad(0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();
	UINT quadVertexOffset = cylinderVertexOffset + (UINT)cylinder.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();
	UINT quadIndexOffset = cylinderIndexOffset + (UINT)cylinder.Indices32.size();

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

	Resources::SubMeshGeometry quadSubmesh;
	quadSubmesh.IndexCount = (UINT)quad.Indices32.size();
	quadSubmesh.StartIndexLocation = quadIndexOffset;
	quadSubmesh.BaseVertexLocation = quadVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size() +
		quad.Vertices.size();

	std::vector<Resources::CENormalTextureTangentVertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].TexCoord = box.Vertices[i].TexC;
		vertices[k].TangentU = box.Vertices[i].TangentU;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].TexCoord = grid.Vertices[i].TexC;
		vertices[k].TangentU = grid.Vertices[i].TangentU;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].TexCoord = sphere.Vertices[i].TexC;
		vertices[k].TangentU = sphere.Vertices[i].TangentU;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].TexCoord = cylinder.Vertices[i].TexC;
		vertices[k].TangentU = cylinder.Vertices[i].TangentU;
	}

	for (int i = 0; i < quad.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = quad.Vertices[i].Position;
		vertices[k].Normal = quad.Vertices[i].Normal;
		vertices[k].TexCoord = quad.Vertices[i].TexC;
		vertices[k].TangentU = quad.Vertices[i].TangentU;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));
	indices.insert(indices.end(), std::begin(quad.GetIndices16()), std::end(quad.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalTextureTangentVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalTextureTangentVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;
	geo->DrawArgs["quad"] = quadSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12CharacterMeshPlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		//TODO: create new constructor
		mFrameResources.push_back(std::make_unique<Resources::CEFrameResource>(m_dx12manager->GetD3D12Device().Get(),
		                                                                       2,
		                                                                       (UINT)mAllRitems.size(),
		                                                                       (UINT)mMaterials.size(),
		                                                                       0,
		                                                                       Resources::WavesNormalTextureVertex,
		                                                                       false));
	}
}

void CEDX12CharacterMeshPlayground::BuildMaterials() {

	auto bricks0 = std::make_unique<Resources::Material>();
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 0;
	bricks0->NormalSrvHeapIndex = 1;
	bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks0->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	bricks0->Roughness = 0.3f;

	auto tile0 = std::make_unique<Resources::Material>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 1;
	tile0->DiffuseSrvHeapIndex = 2;
	tile0->NormalSrvHeapIndex = 3;
	tile0->DiffuseAlbedo = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	tile0->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	tile0->Roughness = 0.1f;

	auto mirror0 = std::make_unique<Resources::Material>();
	mirror0->Name = "mirror0";
	mirror0->MatCBIndex = 2;
	mirror0->DiffuseSrvHeapIndex = 4;
	mirror0->NormalSrvHeapIndex = 5;
	mirror0->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mirror0->FresnelR0 = XMFLOAT3(0.98f, 0.97f, 0.95f);
	mirror0->Roughness = 0.1f;

	auto sky = std::make_unique<Resources::Material>();
	sky->Name = "sky";
	sky->MatCBIndex = 3;
	sky->DiffuseSrvHeapIndex = 6;
	sky->NormalSrvHeapIndex = 7;
	sky->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sky->Roughness = 1.0f;

	mMaterials["bricks0"] = std::move(bricks0);
	mMaterials["tile0"] = std::move(tile0);
	mMaterials["mirror0"] = std::move(mirror0);
	mMaterials["sky"] = std::move(sky);

	UINT matCBIndex = 4;
	UINT srvHeapIndex = m_modelSrvHeapStart;
	for (UINT i = 0; i < m_modelMaterials.size(); ++i) {
		auto material = std::make_unique<Resources::Material>();
		material->Name = m_modelMaterials[i].Name;
		material->MatCBIndex = matCBIndex++;
		material->DiffuseSrvHeapIndex = srvHeapIndex++;
		material->NormalSrvHeapIndex = srvHeapIndex++;
		material->DiffuseAlbedo = m_modelMaterials[i].DiffuseAlbedo;
		material->FresnelR0 = m_modelMaterials[i].FresnelR0;
		material->Roughness = m_modelMaterials[i].Roughness;

		mMaterials[material->Name] = std::move(material);
	}
}

void CEDX12CharacterMeshPlayground::BuildRenderItems() {
	//
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

	auto quadRitem = std::make_unique<Resources::LitShapesRenderItem>();
	quadRitem->World = Resources::MatrixIdentity4X4();
	quadRitem->TexTransform = Resources::MatrixIdentity4X4();
	quadRitem->ObjCBIndex = 1;
	quadRitem->Mat = mMaterials["bricks0"].get();
	quadRitem->Geo = mGeometries["shapeGeo"].get();
	quadRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	quadRitem->IndexCount = quadRitem->Geo->DrawArgs["quad"].IndexCount;
	quadRitem->StartIndexLocation = quadRitem->Geo->DrawArgs["quad"].StartIndexLocation;
	quadRitem->BaseVertexLocation = quadRitem->Geo->DrawArgs["quad"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Debug].push_back(quadRitem.get());
	mAllRitems.push_back(std::move(quadRitem));

	auto boxRitem = std::make_unique<Resources::LitShapesRenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 1.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));
	boxRitem->ObjCBIndex = 2;
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
	skullRitem->ObjCBIndex = 3;
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
	gridRitem->ObjCBIndex = 4;
	gridRitem->Mat = mMaterials["tile0"].get();
	gridRitem->Geo = mGeometries["shapeGeo"].get();
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(gridRitem.get());
	mAllRitems.push_back(std::move(gridRitem));

	XMMATRIX brickTexTransform = XMMatrixScaling(1.5f, 2.0f, 1.0f);
	UINT objCBIndex = 5;
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

void CEDX12CharacterMeshPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
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

void CEDX12CharacterMeshPlayground::BuildCubeFaceCamera(float x, float y, float z) {
	// Generate the cube map about the given position.
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	// Look along each coordinate axis.
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f) // -Z
	};

	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
	// are looking down +Y or -Y, so we need a different "up" vector.
	XMFLOAT3 ups[6] =
	{
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

void CEDX12CharacterMeshPlayground::BuildCubeDepthStencil() {
	//Create depth/stencil buffer and view
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = CubeMapSize;
	depthStencilDesc.Height = CubeMapSize;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = m_dx12manager->GetDepthStencilFormat();
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_dx12manager->GetDepthStencilFormat();
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	auto defHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(
		m_dx12manager->GetD3D12Device()->CreateCommittedResource(
			&defHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(m_cubeDepthStencilBuffer.GetAddressOf())
		)
	);

	//Create descriptor to mip level 0 of entire resource using format of resource
	m_dx12manager->GetD3D12Device()->CreateDepthStencilView(m_cubeDepthStencilBuffer.Get(), nullptr, m_cubeDSV);

	//Transition resource from its initial state to be used as depth buffer
	auto trCDW = CD3DX12_RESOURCE_BARRIER::Transition(m_cubeDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
	                                                  D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trCDW);
}

void CEDX12CharacterMeshPlayground::AnimateSkullMovement(const CETimer& gt) const {
	//Animate skull around center sphere

	XMMATRIX skullScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	XMMATRIX skullOffset = XMMatrixTranslation(3.0f, 2.0f, 0.0f);
	XMMATRIX skullLocalRotate = XMMatrixRotationY(2.0f * gt.TotalTime());
	XMMATRIX skullGlobalRotate = XMMatrixRotationY(0.5f * gt.TotalTime());
	XMStoreFloat4x4(&static_cast<Resources::LitShapesRenderItem*>(mSkullRitem)->World,
	                skullScale * skullLocalRotate * skullOffset * skullGlobalRotate);
	static_cast<Resources::LitShapesRenderItem*>(mSkullRitem)->NumFramesDirty = gNumFrameResources;

}

void CEDX12CharacterMeshPlayground::UpdateShadowTransform(const CETimer& gt) {
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&mRotatedLightDirections[0]);
	XMVECTOR lightPos = -2.0f * mSceneBounds.Radius * lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	XMStoreFloat3(&mLightPosW, lightPos);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;

	mLightNearZ = n;
	mLightFarZ = f;
	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = lightView * lightProj * T;
	XMStoreFloat4x4(&mLightView, lightView);
	XMStoreFloat4x4(&mLightProj, lightProj);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void CEDX12CharacterMeshPlayground::UpdateSSAOCB(const CETimer& gt) {
	Resources::SSAOConstants ssaoCB;

	XMMATRIX P = m_camera.GetProj();

	//Transform NDC space [-1, +1]^2 to texture space [0, 1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);

	ssaoCB.Proj = mMainPassCB.Proj;
	ssaoCB.InvProj = mMainPassCB.InvProj;
	XMStoreFloat4x4(&ssaoCB.ProjTex, XMMatrixTranspose(P * T));

	m_ssao->GetOffsetVectors(ssaoCB.OffsetVectors);

	auto blurWeights = m_ssao->CalcGaussWeights(2.5f);
	ssaoCB.BlurWeights[0] = XMFLOAT4(&blurWeights[0]);
	ssaoCB.BlurWeights[1] = XMFLOAT4(&blurWeights[4]);
	ssaoCB.BlurWeights[2] = XMFLOAT4(&blurWeights[8]);

	ssaoCB.InvRenderTargetSize = XMFLOAT2(1.0f / m_ssao->SSAOMapWidth(), 1.0f / m_ssao->SSAOMapHeight());

	//Coordinates given in view space
	ssaoCB.OcclusionRadius = 0.5f;
	ssaoCB.OcclusionFadeStart = 0.2f;
	ssaoCB.OcclusionFadeEnd = 1.0f;
	ssaoCB.SurfaceEpsilon = 0.05f;
}

void CEDX12CharacterMeshPlayground::UpdateSkinnedModelCBs(const CETimer& gt) {
}

void CEDX12CharacterMeshPlayground::DrawNormalsAndDepth() {
	auto mScreenViewport = m_dx12manager->GetViewPort();
	auto mScissorRect = m_dx12manager->GetScissorRect();
	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &mScreenViewport);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &mScissorRect);

	auto normalMap = m_ssao->NormalMap();
	auto normalMapRTV = m_ssao->NormalMapRtv();

	//Change to RENDER_TARGET
	auto transitionGenericReadRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
		normalMap, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &transitionGenericReadRenderTarget);

	auto dsv = m_dx12manager->DepthStencilView();

	//Clear the screen normal map and depth buffer
	float clearValue[] = {0.0f, 0.0f, 1.0f, 0.0f};
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(normalMapRTV, clearValue, 0, nullptr);
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(dsv,
	                                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	                                                            1.0f,
	                                                            0,
	                                                            0,
	                                                            nullptr);
	//Specify buffers we are going to render to
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &normalMapRTV, true, &dsv);

	//Bind constant buffer for this pass
	auto passCB = mCurrFrameResource->PassSSAOCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["drawNormals"].Get());

	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Opaque]);

	//Change to GENERIC_READ so we can read texture in shader
	auto transitionRenderTargetGenericRead = CD3DX12_RESOURCE_BARRIER::Transition(
		normalMap, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &transitionRenderTargetGenericRead);
}

void CEDX12CharacterMeshPlayground::UpdateShadowPassCB(const CETimer& gt) {
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	auto detView = XMMatrixDeterminant(view);
	XMMATRIX invView = XMMatrixInverse(&detView, view);

	auto detProj = XMMatrixDeterminant(proj);
	XMMATRIX invProj = XMMatrixInverse(&detProj, proj);

	auto detViewProj = XMMatrixDeterminant(viewProj);
	XMMATRIX invViewProj = XMMatrixInverse(&detViewProj, viewProj);

	UINT w = m_shadowMap->Width();
	UINT h = m_shadowMap->Height();

	XMStoreFloat4x4(&mShadowPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mShadowPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mShadowPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mShadowPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mShadowPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mShadowPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mShadowPassCB.EyePosW = mLightPosW;
	mShadowPassCB.RenderTargetSize = XMFLOAT2((float)w, (float)h);
	mShadowPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / w, 1.0f / h);
	mShadowPassCB.NearZ = mLightNearZ;
	mShadowPassCB.FarZ = mLightFarZ;

	auto currPassCB = mCurrFrameResource->PassSSAOCB.get();
	currPassCB->CopyData(1, mShadowPassCB);
}

void CEDX12CharacterMeshPlayground::DrawSceneToShadowMap() {
	auto viewPort = m_shadowMap->Viewport();
	auto scissorRect = m_shadowMap->ScissorRect();
	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &viewPort);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &scissorRect);

	//Change to DEPTH_WRITE
	auto transitionGenericReadDepthWrite = CD3DX12_RESOURCE_BARRIER::Transition(m_shadowMap->Resource(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &transitionGenericReadDepthWrite);

	UINT passCBByteSize = (sizeof(Resources::PassSSAOConstants) + 255) & ~255;

	//Clear back buffer and depth buffer 
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(m_shadowMap->Dsv(),
	                                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	                                                            1.0f,
	                                                            0,
	                                                            0,
	                                                            nullptr);

	/*
	 * Set null render target  because we are only going to draw to depth buffer.
	 * Setting a null render target will disable color writes
	 * NOTE: Active PSO also must specify render target count of 0
	 */
	auto shadowDsv = m_shadowMap->Dsv();
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(0, nullptr, false, &shadowDsv);

	//Bind the pass constant buffer for shadow map pass
	auto passCB = mCurrFrameResource->PassSSAOCB->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(1, passCBAddress);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["shadow_opaque"].Get());

	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Opaque]);

	//Change back to GENERIC_READ so we can read texture in a shader
	auto transitionDepthWriteGenericRead = CD3DX12_RESOURCE_BARRIER::Transition(
		m_shadowMap->Resource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_GENERIC_READ);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &transitionDepthWriteGenericRead);
}
