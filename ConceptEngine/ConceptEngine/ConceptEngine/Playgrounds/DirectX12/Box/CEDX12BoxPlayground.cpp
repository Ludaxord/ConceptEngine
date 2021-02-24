#include "CEDX12BoxPlayground.h"

#include <DirectXColors.h>


#include "../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEDefaultBuffer.h"
#include "../../../../ConceptEngineFramework/Tools/CEUtils.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

using namespace ConceptEngineFramework::Graphics::DirectX12;

const int gNumFrameResources = 3;

CEDX12BoxPlayground::CEDX12BoxPlayground() : CEDX12Playground() {
}

void CEDX12BoxPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	m_dx12manager->CreateCSVDescriptorHeap();

	auto d3dDevice = m_dx12manager->GetD3D12Device();
	mObjectCB = std::make_unique<Resources::CEUploadBuffer<Resources::CEObjectConstants>>(d3dDevice.Get(), 1, true);

	UINT objCBByteSize = (sizeof(Resources::CEObjectConstants) + 255) & ~255;

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	m_dx12manager->CreateConstantBuffers(cbAddress, objCBByteSize);

	//Root parameter can be a table, root descriptor or root constants
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter,
	                                        0,
	                                        nullptr,
	                                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	m_rootSignature = m_dx12manager->CreateRootSignature(&rootSigDesc);
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();

	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	auto commandList = m_dx12manager->GetD3D12CommandList();
	// Execute the initialization commands.
	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* cmdsLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	m_dx12manager->FlushCommandQueue();
}

void CEDX12BoxPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);

	//Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float y = mRadius * sinf(mPhi) * sinf(mTheta);
	float z = mRadius * cos(mPhi);

	//Build view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view * proj;

	//Update constant buffer with latest worldViewProj matrix;
	Resources::CEObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProjection, XMMatrixTranspose(worldViewProj));
	mObjectCB->CopyData(0, objConstants);
}

void CEDX12BoxPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);

	//D3D12 and DXGI Resources
	auto commandAllocator = m_dx12manager->GetD3D12CommandAllocator();
	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	auto commandList = m_dx12manager->GetD3D12CommandList();
	auto swapChain = m_dx12manager->GetDXGISwapChain();

	//Reuse memory associated with command recording
	//We can only reset when associated command lists have finished execution on GPU
	ThrowIfFailed(commandAllocator->Reset());

	//Command list can be reset after it has been added to command queue via ExecuteCommandList
	//Reusing command list reuses memory
	ThrowIfFailed(commandList->Reset(commandAllocator.Get(), mPSO.Get()));

	auto viewport = m_dx12manager->GetViewPort();
	auto scissorRect = m_dx12manager->GetScissorRect();
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	//Indicate state transition on resource usage.
	auto resourceBarrierTransitionRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &resourceBarrierTransitionRenderTarget);

	const auto currentBackBufferIndex = m_dx12manager->GetCurrentBackBufferIndex();
	const auto currentBackBufferView = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_dx12manager->GetRTVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		currentBackBufferIndex,
		m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	const auto depthStencilView = m_dx12manager->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	//Clear back buffer and depth buffer
	commandList->ClearRenderTargetView(currentBackBufferView,
	                                   Colors::CornflowerBlue,
	                                   0,
	                                   nullptr);
	commandList->ClearDepthStencilView(depthStencilView,
	                                   D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	                                   1.0f,
	                                   0,
	                                   0,
	                                   nullptr);

	//Specify buffers we are going to render to
	commandList->OMSetRenderTargets(1,
	                                &currentBackBufferView,
	                                true,
	                                &depthStencilView);

	ID3D12DescriptorHeap* descriptorHeaps[] = {m_dx12manager->GetCBVDescriptorHeap().Get()};
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	auto boxVertexBufferView = m_boxGeo->VertexBufferView();
	commandList->IASetVertexBuffers(0, 1, &boxVertexBufferView);

	try {
		auto boxIndexBufferView = m_boxGeo->IndexBufferView();
		commandList->IASetIndexBuffer(&boxIndexBufferView);
	}
	catch (std::exception e) {
		OutputDebugStringA(e.what());
	}

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootDescriptorTable(
		0,
		m_dx12manager->GetCBVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
	);

	commandList->DrawIndexedInstanced(
		m_boxGeo->DrawArgs["box"].IndexCount,
		1,
		0,
		0,
		0
	);

	//Indicate state transition on resource usage
	auto resourceBarrierTransitionStatePresent = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &resourceBarrierTransitionStatePresent);

	//Done recording commands
	ThrowIfFailed(commandList->Close());

	//Add command list to queue for execution
	ID3D12CommandList* commandLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	// swap the back and front buffers
	ThrowIfFailed(swapChain->Present(0, 0));
	m_dx12manager->SetCurrentBackBufferIndex((currentBackBufferIndex + 1) % CEDX12Manager::GetBackBufferCount());

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	m_dx12manager->FlushCommandQueue();
}

void CEDX12BoxPlayground::Resize() {
	static const float Pi = 3.1415926535f;
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void CEDX12BoxPlayground::OnMouseDown(ConceptEngineFramework::Game::KeyCode key, int x, int y) {
	std::string keyName;
	switch (key) {
	case ConceptEngineFramework::Game::KeyCode::LButton:
		keyName = "Left Mouse button";
		break;
	case ConceptEngineFramework::Game::KeyCode::RButton:
		keyName = "Right Mouse Button";
		break;
	case ConceptEngineFramework::Game::KeyCode::MButton:
		keyName = "Middle Mouse Button";
		break;
	default:
		keyName = "None";
		break;
	}
	std::stringstream ss;
	ss << "MOUSE BUTTON DOWN: " << keyName << " X: " << x << " Y: " << y;
	spdlog::info(ss.str());
}

void CEDX12BoxPlayground::OnMouseUp(ConceptEngineFramework::Game::KeyCode key, int x, int y) {
	std::string keyName;
	switch (key) {
	case ConceptEngineFramework::Game::KeyCode::LButton:
		keyName = "Left Mouse button";
		break;
	case ConceptEngineFramework::Game::KeyCode::RButton:
		keyName = "Right Mouse Button";
		break;
	case ConceptEngineFramework::Game::KeyCode::MButton:
		keyName = "Middle Mouse Button";
		break;
	default:
		keyName = "None";
		break;
	}
	std::stringstream ss;
	ss << "MOUSE BUTTON UP: " << keyName << " X: " << x << " Y: " << y;
	spdlog::info(ss.str());
}

void CEDX12BoxPlayground::OnMouseMove(ConceptEngineFramework::Game::KeyCode key, int x, int y) {
	std::string keyName;
	switch (key) {
	case ConceptEngineFramework::Game::KeyCode::LButton:
		keyName = "Left Mouse button";
		break;
	case ConceptEngineFramework::Game::KeyCode::RButton:
		keyName = "Right Mouse Button";
		break;
	case ConceptEngineFramework::Game::KeyCode::MButton:
		keyName = "Middle Mouse Button";
		break;
	default:
		keyName = "None";
		break;
	}
	std::stringstream ss;
	ss << "MOUSE MOVE: " << keyName << " X: " << x << " Y: " << y;
	spdlog::info(ss.str());
}

void CEDX12BoxPlayground::OnKeyUp(ConceptEngineFramework::Game::KeyCode key, char keyChar) {
	spdlog::info("KEYBOARD UP KEY: {}", keyChar);
}

void CEDX12BoxPlayground::OnKeyDown(ConceptEngineFramework::Game::KeyCode key, char keyChar) {
	spdlog::info("KEYBOARD DOWN KEY: {}", keyChar);
}

void CEDX12BoxPlayground::BuildShadersAndInputLayout() {
	HRESULT hr = S_OK;

	m_vsByteCode = m_dx12manager->CompileShaders("CEBoxVertexShader.hlsl",
	                                             nullptr,
	                                             "VS",
	                                             // "vs_6_3"
	                                             "vs_5_0"
	);
	m_psByteCode = m_dx12manager->CompileShaders("CEBoxPixelShader.hlsl",
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

void CEDX12BoxPlayground::BuildBoxGeometry() {

	auto d3dDevice = m_dx12manager->GetD3D12Device();
	auto d3dCommandList = m_dx12manager->GetD3D12CommandList();

	std::vector<Resources::CEVertex> vertices = {
		Resources::CEVertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::White)}),
		Resources::CEVertex({XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Red)}),
		Resources::CEVertex({XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::DarkRed)}),
		Resources::CEVertex({XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Green)}),
		Resources::CEVertex({XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Blue)}),
		Resources::CEVertex({XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Yellow)}),
		Resources::CEVertex({XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Magenta)}),
		Resources::CEVertex({XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::AliceBlue)}),
	};

	std::array<std::uint16_t, 36> indices = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CEVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_boxGeo = std::make_unique<Resources::MeshGeometry>();
	m_boxGeo->Name = "GeoBox";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_boxGeo->VertexBufferCPU));
	CopyMemory(m_boxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_boxGeo->IndexBufferCPU));
	CopyMemory(m_boxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_boxGeo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(d3dDevice.Get(),
	                                                               d3dCommandList.Get(),
	                                                               vertices.data(),
	                                                               vbByteSize,
	                                                               m_boxGeo->VertexBufferUploader);

	m_boxGeo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(d3dDevice.Get(),
	                                                              d3dCommandList.Get(),
	                                                              indices.data(),
	                                                              ibByteSize,
	                                                              m_boxGeo->IndexBufferUploader);

	m_boxGeo->VertexByteStride = sizeof(Resources::CEVertex);
	m_boxGeo->VertexBufferByteSize = vbByteSize;
	m_boxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_boxGeo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry subMesh;
	subMesh.IndexCount = (UINT)indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;

	m_boxGeo->DrawArgs["box"] = subMesh;
}

//TODO: Add RayTraced StateObject
void CEDX12BoxPlayground::BuildPSO() {
	auto d3dDevice = m_dx12manager->GetD3D12Device();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = {m_inputLayout.data(), (UINT)m_inputLayout.size()};
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = {
		static_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize()
	};
	psoDesc.PS = {
		static_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_dx12manager->GetBackBufferFormat();
	psoDesc.SampleDesc.Count = m_dx12manager->GetM4XMSAAState() ? 4 : 1;
	psoDesc.SampleDesc.Quality = m_dx12manager->GetM4XMSAAState() ? (m_dx12manager->GetM4XMSAAQuality() - 1) : 0;
	psoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}
