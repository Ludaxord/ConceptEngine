#include "CEDX12Playground.h"

#include <spdlog/spdlog.h>


#include "CEDX12Manager.h"
#include "../CEGraphicsManager.h"
#include "../../Physics/CEPhysics.h"
#include "../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;

CEDX12Playground::CEDX12Playground() {
};

void CEDX12Playground::Init(CEGraphicsManager* manager) {
	CEDX12Manager* dx12Manager = reinterpret_cast<CEDX12Manager*>(manager);
	m_dx12manager = dx12Manager;
}

void CEDX12Playground::Create() {
	m_dx12manager->GetPhysics()->CreateScene();
}

void CEDX12Playground::Update(const CETimer& gt) {
	m_dx12manager->GetPhysics()->Update(gt.DeltaTime());
}

void CEDX12Playground::Render(const CETimer& gt) {
}

void CEDX12Playground::Resize() {
}

void CEDX12Playground::UpdateCamera(const CETimer& gt) {
}

void CEDX12Playground::AnimateMaterials(const CETimer& gt) {
}

void CEDX12Playground::OnMouseDown(Game::KeyCode key, int x, int y) {
}

void CEDX12Playground::OnMouseUp(Game::KeyCode key, int x, int y) {
}

void CEDX12Playground::OnMouseMove(Game::KeyCode key, int x, int y) {
}

void CEDX12Playground::OnKeyUp(Game::KeyCode key, char keyChar, const CETimer& gt) {
	switch (key) {
	case Game::KeyCode::F1:
		mIsWireframe = false;
		break;
	case Game::KeyCode::W:
		m_forward = 0.0f;
		break;
	case Game::KeyCode::A:
		m_left = 0.0f;
		break;
	case Game::KeyCode::S:
		m_backward = 0.0f;
		break;
	case Game::KeyCode::D:
		m_right = 0.0f;
		break;
	case Game::KeyCode::Q:
		m_down = 0.0f;
		break;
	case Game::KeyCode::E:
		m_up = 0.0f;
		break;
	case Game::KeyCode::ShiftKey:
		m_shift = false;
		break;
	}
}

void CEDX12Playground::OnKeyDown(Game::KeyCode key, char keyChar, const CETimer& gt) {
	switch (key) {
	case Game::KeyCode::F1:
		mIsWireframe = true;
		break;
	case Game::KeyCode::ShiftKey:
		m_shift = true;
		break;
	}
}

void CEDX12Playground::OnMouseWheel(Game::KeyCode key, float wheelDelta, int x, int y) {
}

void CEDX12Playground::UpdateObjectCBs(const CETimer& gt) {
}

void CEDX12Playground::UpdateMainPassCB(const CETimer& gt) {
}

void CEDX12Playground::UpdateMaterialCBs(const CETimer& gt) {
}

//TODO: Implement Shader Model 6 usage with DirectXShaderCompiler
//TODO: References: https://asawicki.info/news_1719_two_shader_compilers_of_direct3d_12
//TODO: References: https://github.com/Microsoft/DirectXShaderCompiler
void CEDX12Playground::BuildColorShadersAndInputLayout(std::string vertexShaderFileName, std::string pixelShaderFileName) {
	m_shadersMap["standardVS"] = m_dx12manager->CompileShaders(vertexShaderFileName,
	                                                           nullptr,
	                                                           "VS",
	                                                           // "vs_6_3"
	                                                           "vs_5_1"
	);
	m_shadersMap["opaquePS"] = m_dx12manager->CompileShaders(pixelShaderFileName,
	                                                         nullptr,
	                                                         "PS",
	                                                         // "ps_6_3"
	                                                         "ps_5_1"
	);

	m_inputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

void CEDX12Playground::BuildNormalShadersAndInputLayout(std::string vertexShaderFileName, std::string pixelShaderFileName) {
	m_shadersMap["standardVS"] = m_dx12manager->CompileShaders(vertexShaderFileName,
	                                                           nullptr,
	                                                           "VS",
	                                                           // "vs_6_3"
	                                                           "vs_5_1"
	);
	m_shadersMap["opaquePS"] = m_dx12manager->CompileShaders(pixelShaderFileName,
	                                                         nullptr,
	                                                         "PS",
	                                                         // "ps_6_3"
	                                                         "ps_5_1"
	);

	m_inputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

void CEDX12Playground::BuildInputLayout() {
	m_inputLayout = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

//TODO: Add RayTraced StateObject
void CEDX12Playground::BuildPSOs(ComPtr<ID3D12RootSignature> rootSignature) {
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
