#include "CEDX12LandscapePlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"

using namespace ConceptEngine::Playgrounds::DirectX12;
using namespace ConceptEngineFramework::Game;

void CEDX12LandscapePlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	//Create basic waves

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

	//Create geometry buffers

	//Create Render items

	//Create Frame resources

	//Create Pipeline State Objects
}

void CEDX12LandscapePlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);

}

void CEDX12LandscapePlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12LandscapePlayground::Resize() {
	CEDX12Playground::Resize();
}

void CEDX12LandscapePlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
}

void CEDX12LandscapePlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
}

void CEDX12LandscapePlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
}

void CEDX12LandscapePlayground::OnKeyUp(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyUp(key, keyChar);
}

void CEDX12LandscapePlayground::OnKeyDown(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyDown(key, keyChar);
}

void CEDX12LandscapePlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
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
