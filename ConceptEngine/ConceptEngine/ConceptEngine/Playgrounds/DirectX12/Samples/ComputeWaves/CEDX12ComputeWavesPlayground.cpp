#include "CEDX12ComputeWavesPlayground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12ComputeWavesPlayground::CEDX12ComputeWavesPlayground() : CEDX12Playground() {
}

void CEDX12ComputeWavesPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	//Create GPU based waves
	m_waves = std::make_unique<Resources::CEGpuWaves>(m_dx12manager->GetD3D12Device().Get(),
	                                                  m_dx12manager->GetD3D12CommandList().Get(),
	                                                  256, 256, 1.0f, 0.03f, 4.0f, 0.2f);

	m_sobelFilter = std::make_unique<Resources::CESobelFilter>(m_dx12manager->GetD3D12Device().Get(),
	                                                           m_dx12manager->GetWindowWidth(),
	                                                           m_dx12manager->GetWindowHeight(),
	                                                           m_dx12manager->GetBackBufferFormat()
	);

	m_offscreenRT = std::make_unique<Resources::CERenderTarget>(m_dx12manager->GetD3D12Device().Get(),
	                                                            m_dx12manager->GetWindowWidth(),
	                                                            m_dx12manager->GetWindowHeight(),
	                                                            m_dx12manager->GetBackBufferFormat());

	LoadTextures();
	//Root Signature
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE displacementMapTable;
		displacementMapTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

		CD3DX12_ROOT_PARAMETER slotRootParameter[5];

		slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_ALL);
		slotRootParameter[1].InitAsConstantBufferView(0);
		slotRootParameter[2].InitAsConstantBufferView(1);
		slotRootParameter[3].InitAsConstantBufferView(2);
		slotRootParameter[4].InitAsDescriptorTable(1, &displacementMapTable, D3D12_SHADER_VISIBILITY_ALL);

		auto staticSamplers = m_dx12manager->GetStaticSamplers();
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(5,
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
	//BuildPostProcessRootSignature
	{
		CD3DX12_DESCRIPTOR_RANGE srvTable0;
		srvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE srvTable1;
		srvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

		CD3DX12_DESCRIPTOR_RANGE uavTable0;
		uavTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

		CD3DX12_ROOT_PARAMETER slotRootParameter[3];

		slotRootParameter[0].InitAsDescriptorTable(1, &srvTable0);
		slotRootParameter[1].InitAsDescriptorTable(1, &srvTable1);
		slotRootParameter[2].InitAsDescriptorTable(1, &uavTable0);

		auto staticSamplers = m_dx12manager->GetStaticSamplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(3,
		                                              slotRootParameter,
		                                              (UINT)staticSamplers.size(),
		                                              staticSamplers.data(),
		                                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		m_postProcessRootSignature = m_dx12manager->CreateRootSignature(&rootSignatureDesc);
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
	BuildWavesGeometryBuffers();
	BuildBoxGeometry();
	BuildTreeSpritesGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs(m_rootSignature);
	{
		auto d3dDevice = m_dx12manager->GetD3D12Device();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC basePsoDesc;
		ZeroMemory(&basePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		basePsoDesc.InputLayout = {m_inputLayout.data(), (UINT)m_inputLayout.size()};
		basePsoDesc.pRootSignature = m_rootSignature.Get();
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
		basePsoDesc.SampleDesc.Quality = m_dx12manager->GetM4XMSAAState()
			                                 ? (m_dx12manager->GetM4XMSAAQuality() - 1)
			                                 : 0;
		basePsoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();

		//PSO for transparent objects
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
		ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&mPSOs["transparent"])));

		//PSO for alpha tested objects
		D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaPsoDesc = basePsoDesc;
		alphaPsoDesc.PS = {
			reinterpret_cast<BYTE*>(
				m_shadersMap["alphaPS"]->GetBufferPointer()
			),
			m_shadersMap["alphaPS"]->GetBufferSize()
		};
		alphaPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&alphaPsoDesc, IID_PPV_ARGS(&mPSOs["alpha"])));

		//PSO for tree sprites
		D3D12_GRAPHICS_PIPELINE_STATE_DESC spritePsoDesc = basePsoDesc;
		spritePsoDesc.VS = {
			reinterpret_cast<BYTE*>(m_shadersMap["treeSpriteVS"]->GetBufferPointer()),
			m_shadersMap["treeSpriteVS"]->GetBufferSize()
		};
		spritePsoDesc.GS = {
			reinterpret_cast<BYTE*>(m_shadersMap["treeSpriteGS"]->GetBufferPointer()),
			m_shadersMap["treeSpriteGS"]->GetBufferSize()
		};
		spritePsoDesc.PS = {
			reinterpret_cast<BYTE*>(m_shadersMap["treeSpritePS"]->GetBufferPointer()),
			m_shadersMap["treeSpritePS"]->GetBufferSize()
		};
		spritePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		spritePsoDesc.InputLayout = {m_spriteInputLayout.data(), (UINT)m_spriteInputLayout.size()};
		spritePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&spritePsoDesc, IID_PPV_ARGS(&mPSOs["treeSprites"])));

		//PSO for drawing waves
		D3D12_GRAPHICS_PIPELINE_STATE_DESC wavesRenderPSO = transparentPsoDesc;
		wavesRenderPSO.VS = {
			reinterpret_cast<BYTE*>(m_shadersMap["wavesVS"]->GetBufferPointer()),
			m_shadersMap["wavesVS"]->GetBufferSize()
		};
		ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&wavesRenderPSO, IID_PPV_ARGS(&mPSOs["wavesRender"])));

		//PSO for compositing post process
		D3D12_GRAPHICS_PIPELINE_STATE_DESC compositePSO = basePsoDesc;
		compositePSO.pRootSignature = m_postProcessRootSignature.Get();

		//Disable depth test
		compositePSO.DepthStencilState.DepthEnable = false;
		compositePSO.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		compositePSO.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		compositePSO.VS = {
			reinterpret_cast<BYTE*>(m_shadersMap["compositeVS"]->GetBufferPointer()),
			m_shadersMap["compositeVS"]->GetBufferSize()
		};
		compositePSO.PS = {
			reinterpret_cast<BYTE*>(m_shadersMap["compositePS"]->GetBufferPointer()),
			m_shadersMap["compositePS"]->GetBufferSize()
		};
		ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&compositePSO, IID_PPV_ARGS(&mPSOs["composite"])));

		//PSO for disturbing waves
		D3D12_COMPUTE_PIPELINE_STATE_DESC wavesDisturbPSO = {};
		wavesDisturbPSO.pRootSignature = m_wavesRootSignature.Get();
		wavesDisturbPSO.CS = {
			reinterpret_cast<BYTE*>(m_shadersMap["wavesDisturbCS"]->GetBufferPointer()),
			m_shadersMap["wavesDisturbCS"]->GetBufferSize()
		};
		wavesDisturbPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		ThrowIfFailed(d3dDevice->CreateComputePipelineState(&wavesDisturbPSO, IID_PPV_ARGS(&mPSOs["wavesDisturb"])));

		//PSO for updating waves
		D3D12_COMPUTE_PIPELINE_STATE_DESC wavesUpdatePSO = {};
		wavesUpdatePSO.pRootSignature = m_wavesRootSignature.Get();
		wavesUpdatePSO.CS = {
			reinterpret_cast<BYTE*>(m_shadersMap["wavesUpdateCS"]->GetBufferPointer()),
			m_shadersMap["wavesUpdateCS"]->GetBufferSize()
		};
		wavesUpdatePSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		ThrowIfFailed(d3dDevice->CreateComputePipelineState(&wavesUpdatePSO, IID_PPV_ARGS(&mPSOs["wavesUpdate"])));

		//PSO for sobel
		D3D12_COMPUTE_PIPELINE_STATE_DESC sobelPSO = {};
		sobelPSO.pRootSignature = m_postProcessRootSignature.Get();
		sobelPSO.CS = {
			reinterpret_cast<BYTE*>(m_shadersMap["sobelCS"]->GetBufferPointer()),
			m_shadersMap["sobelCS"]->GetBufferSize()
		};
		sobelPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		ThrowIfFailed(d3dDevice->CreateComputePipelineState(&sobelPSO, IID_PPV_ARGS(&mPSOs["sobel"])));
	}

	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());
	m_dx12manager->ExecuteCommandLists();

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12ComputeWavesPlayground::Update(const CETimer& gt) {
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

void CEDX12ComputeWavesPlayground::Render(const CETimer& gt) {
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


	//TODO: Add function as parameter to method to make creation of scene more dynamic
	ID3D12DescriptorHeap* descriptorHeaps[] = {m_dx12manager->GetSRVDescriptorHeap().Get()};
	m_dx12manager->GetD3D12CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	UpdateWavesGPU(gt);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["opaque"].Get());

	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &mScreenViewport);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &mScissorRect);

	//Change offscreen texture to be used as a render target output
	auto trOffScrGRRT = CD3DX12_RESOURCE_BARRIER::Transition(m_offscreenRT->Resource(),
	                                                         D3D12_RESOURCE_STATE_GENERIC_READ,
	                                                         D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trOffScrGRRT);

	//Clear back buffer and depth buffer
	auto dsv = m_dx12manager->DepthStencilView();
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(m_offscreenRT->Rtv(),
	                                                            (float*)&mMainPassCB.FogColor,
	                                                            0,
	                                                            nullptr);
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(dsv,
	                                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	                                                            1.0f,
	                                                            0,
	                                                            0,
	                                                            nullptr);

	//Specify buffer we are going to render to
	auto offScreenRTV = m_offscreenRT->Rtv();
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &offScreenRTV, true, &dsv);

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(4, m_waves->DisplacementMap());

	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Opaque]);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["alpha"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Alpha]);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["treeSprites"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::AlphaSprites]);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["transparent"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Transparent]);

	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["wavesRender"].Get());
	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::GpuWaves]);

	//Change offscreen texture to be used as an input
	auto trOffScreenRTGR = CD3DX12_RESOURCE_BARRIER::Transition(m_offscreenRT->Resource(),
	                                                            D3D12_RESOURCE_STATE_RENDER_TARGET,
	                                                            D3D12_RESOURCE_STATE_GENERIC_READ);
	// Change offscreen texture to be used as an input.
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trOffScreenRTGR);

	m_sobelFilter->Execute(m_dx12manager->GetD3D12CommandList().Get(),
	                       m_postProcessRootSignature.Get(),
	                       mPSOs["sobel"].Get(),
	                       m_offscreenRT->Srv());

	//
	// Switching back to back buffer rendering.
	//

	// Indicate a state transition on the resource usage.
	auto trCbb = CD3DX12_RESOURCE_BARRIER::Transition(m_dx12manager->CurrentBackBuffer(),
	                                                  D3D12_RESOURCE_STATE_PRESENT,
	                                                  D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trCbb);

	// Specify the buffers we are going to render to.
	auto cBBV = m_dx12manager->CurrentBackBufferView();
	auto cDSV = m_dx12manager->DepthStencilView();
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &cBBV, true, &cDSV);

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_postProcessRootSignature.Get());
	m_dx12manager->GetD3D12CommandList()->SetPipelineState(mPSOs["composite"].Get());
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(0, m_offscreenRT->Srv());
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(1, m_sobelFilter->OutputSrv());
	DrawFullscreenQuad(m_dx12manager->GetD3D12CommandList().Get());

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

void CEDX12ComputeWavesPlayground::Resize() {
	CEDX12Playground::Resize();
	static const float Pi = 3.1415926535f;
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);

	if (m_sobelFilter != nullptr) {
		m_sobelFilter->OnResize(m_dx12manager->GetWindowWidth(), m_dx12manager->GetWindowHeight());
	}

	if (m_offscreenRT != nullptr) {
		m_offscreenRT->OnResize(m_dx12manager->GetWindowWidth(), m_dx12manager->GetWindowHeight());
	}
}

void CEDX12ComputeWavesPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12ComputeWavesPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);

	ReleaseCapture();
}

void CEDX12ComputeWavesPlayground::OnMouseMove(KeyCode key, int x, int y) {
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

void CEDX12ComputeWavesPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12ComputeWavesPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
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

void CEDX12ComputeWavesPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12ComputeWavesPlayground::UpdateCamera(const CETimer& gt) {
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

void CEDX12ComputeWavesPlayground::AnimateMaterials(const CETimer& gt) {
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

void CEDX12ComputeWavesPlayground::UpdateObjectCBs(const CETimer& gt) {
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

void CEDX12ComputeWavesPlayground::UpdateMaterialCBs(const CETimer& gt) {
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

void CEDX12ComputeWavesPlayground::UpdateMainPassCB(const CETimer& gt) {
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

void CEDX12ComputeWavesPlayground::LoadTextures() {
	auto grassTex = m_dx12manager->LoadTextureFromFile("grass.dds", "grassTex");
	auto waterTex = m_dx12manager->LoadTextureFromFile("water1.dds", "waterTex");
	auto fenceTex = m_dx12manager->LoadTextureFromFile("WireFence.dds", "fenceTex");
	auto treeArrayTex = m_dx12manager->LoadTextureFromFile("treeArray2.dds", "treeArrayTex");

	mTextures[grassTex->Name] = std::move(grassTex);
	mTextures[waterTex->Name] = std::move(waterTex);
	mTextures[fenceTex->Name] = std::move(fenceTex);
	mTextures[treeArrayTex->Name] = std::move(treeArrayTex);
}

void CEDX12ComputeWavesPlayground::BuildDescriptorHeaps() {
	//Offscreen RTV goes after swap chain descriptors

	auto d3dDevice = m_dx12manager->GetD3D12Device();
	auto srvUavSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto rtvSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	int rtvOffset = m_dx12manager->GetBackBufferCount();

	UINT srvCount = 4;

	int waveSrvOffset = srvCount;
	int sobelSrvOffset = waveSrvOffset + m_waves->DescriptorCount();
	int offscreenSrvOffset = sobelSrvOffset + m_sobelFilter->DescriptorCount();

	/*
	 * Create SRV heap
	 */
	m_dx12manager->
		CreateSRVDescriptorHeap(srvCount + m_waves->DescriptorCount() + m_sobelFilter->DescriptorCount() + 1);

	//Fill out heap with actual descriptors
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		m_dx12manager->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	auto grassTex = mTextures["grassTex"]->Resource;
	auto waterTex = mTextures["waterTex"]->Resource;
	auto fenceTex = mTextures["fenceTex"]->Resource;
	auto treeArrayTex = mTextures["treeArrayTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = grassTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	d3dDevice->CreateShaderResourceView(grassTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvUavSize);

	srvDesc.Format = waterTex->GetDesc().Format;
	d3dDevice->CreateShaderResourceView(waterTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvUavSize);

	srvDesc.Format = fenceTex->GetDesc().Format;
	d3dDevice->CreateShaderResourceView(fenceTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvUavSize);

	auto desc = treeArrayTex->GetDesc();
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Format = treeArrayTex->GetDesc().Format;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = -1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = treeArrayTex->GetDesc().DepthOrArraySize;
	d3dDevice->CreateShaderResourceView(treeArrayTex.Get(), &srvDesc, hDescriptor);

	auto srvCpuStart = m_dx12manager->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	auto srvGpuStart = m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	auto rtvCpuStart = m_dx12manager->GetRTVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	m_waves->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, waveSrvOffset, srvUavSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, waveSrvOffset, srvUavSize),
		srvUavSize
	);

	m_sobelFilter->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, sobelSrvOffset, srvUavSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, sobelSrvOffset, srvUavSize),
		srvUavSize
	);

	m_offscreenRT->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, offscreenSrvOffset, srvUavSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, offscreenSrvOffset, srvUavSize),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCpuStart, rtvOffset, rtvSize));
}

void CEDX12ComputeWavesPlayground::BuildLandGeometry() {
	GeometryGenerator geometry;
	GeometryGenerator::MeshData grid = geometry.CreateGrid(160.0f, 160.0f, 50, 50);

	/*
	 * Extract vertex elements we are interested and apply height function to each vertex
	 * In addition, color vertices based on their height so we have sandy looking beaches, grassy low hills, and snow mountain peaks
	 */

	std::vector<Resources::CENormalTextureVertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i) {
		auto& p = grid.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Pos.y = GetHillsHeight(p.x, p.z);
		vertices[i].Normal = GetHillsNormal(p.x, p.z);
		vertices[i].TexCoord = grid.Vertices[i].TexC;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalTextureVertex);

	std::vector<std::uint16_t> indices = grid.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "landGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->VertexBufferGPU->SetName(L"Landscape Geometry Vertex Resource");

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);
	geo->IndexBufferGPU->SetName(L"Landscape Geometry Index Resource");

	geo->VertexByteStride = sizeof(Resources::CENormalTextureVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = submesh;

	mGeometries["landGeo"] = std::move(geo);
}

void CEDX12ComputeWavesPlayground::BuildTreeSpritesGeometry() {
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

void CEDX12ComputeWavesPlayground::BuildWavesGeometryBuffers() {

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

void CEDX12ComputeWavesPlayground::BuildBoxGeometry() {
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

void CEDX12ComputeWavesPlayground::BuildMaterials() {
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

	mMaterials["grass"] = std::move(grass);
	mMaterials["water"] = std::move(water);
	mMaterials["wirefence"] = std::move(wirefence);
	mMaterials["treeSprites"] = std::move(treeSprites);
}

void CEDX12ComputeWavesPlayground::BuildRenderItems() {
	auto wavesRitem = std::make_unique<Resources::LitShapesRenderItem>();
	wavesRitem->World = Resources::MatrixIdentity4X4();
	XMStoreFloat4x4(&wavesRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	wavesRitem->DisplacementMapTexelSize.x = 1.0f / m_waves->ColumnCount();
	wavesRitem->DisplacementMapTexelSize.y = 1.0f / m_waves->RowCount();
	wavesRitem->GridSpatialStep = m_waves->SpatialStep();
	wavesRitem->ObjCBIndex = 0;
	wavesRitem->Mat = mMaterials["water"].get();
	wavesRitem->Geo = mGeometries["waterGeo"].get();
	wavesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
	wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::GpuWaves].push_back(wavesRitem.get());

	auto gridRitem = std::make_unique<Resources::LitShapesRenderItem>();
	gridRitem->World = Resources::MatrixIdentity4X4();
	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	gridRitem->ObjCBIndex = 1;
	gridRitem->Mat = mMaterials["grass"].get();
	gridRitem->Geo = mGeometries["landGeo"].get();
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(gridRitem.get());

	auto boxRitem = std::make_unique<Resources::LitShapesRenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixTranslation(3.0f, 2.0f, -9.0f));
	boxRitem->ObjCBIndex = 2;
	boxRitem->Mat = mMaterials["wirefence"].get();
	boxRitem->Geo = mGeometries["boxGeo"].get();
	boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Alpha].push_back(boxRitem.get());

	auto treeSpritesRitem = std::make_unique<Resources::LitShapesRenderItem>();
	treeSpritesRitem->World = Resources::MatrixIdentity4X4();
	treeSpritesRitem->ObjCBIndex = 3;
	treeSpritesRitem->Mat = mMaterials["treeSprites"].get();
	treeSpritesRitem->Geo = mGeometries["treeSpritesGeo"].get();
	treeSpritesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	treeSpritesRitem->IndexCount = treeSpritesRitem->Geo->DrawArgs["points"].IndexCount;
	treeSpritesRitem->StartIndexLocation = treeSpritesRitem->Geo->DrawArgs["points"].StartIndexLocation;
	treeSpritesRitem->BaseVertexLocation = treeSpritesRitem->Geo->DrawArgs["points"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::AlphaSprites].push_back(treeSpritesRitem.get());

	mAllRitems.push_back(std::move(wavesRitem));
	mAllRitems.push_back(std::move(gridRitem));
	mAllRitems.push_back(std::move(boxRitem));
	mAllRitems.push_back(std::move(treeSpritesRitem));
}

void CEDX12ComputeWavesPlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<Resources::CEFrameResource>(m_dx12manager->GetD3D12Device().Get(),
		                                                                       1,
		                                                                       (UINT)mAllRitems.size(),
		                                                                       (UINT)mMaterials.size(),
		                                                                       0,
		                                                                       Resources::WavesNormalTextureVertex));
	}
}

void CEDX12ComputeWavesPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
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

float CEDX12ComputeWavesPlayground::GetHillsHeight(float x, float z) const {
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 CEDX12ComputeWavesPlayground::GetHillsNormal(float x, float z) const {
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void CEDX12ComputeWavesPlayground::DrawFullscreenQuad(ID3D12GraphicsCommandList* cmdList) {
	//Null-out IA stage since we build vertex off SV_VertexID in shader
	cmdList->IASetVertexBuffers(0, 1, nullptr);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->DrawInstanced(6, 1, 0, 0);
}

void CEDX12ComputeWavesPlayground::UpdateWavesGPU(const CETimer& gt) {
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
