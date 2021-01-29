#include "CECubePlayground.h"

#include <d3dcompiler.h>


#include "CECommandList.h"
#include "CECommandQueue.h"
#include "CEDevice.h"
#include "CEHelper.h"
#include "CEIndexBuffer.h"
#include "CERootSignature.h"
#include "CESwapChain.h"
#include "CETools.h"
#include "ConceptEngine.h"
#include "d3dx12.h"

using namespace Concept::GameEngine::Playground;

using namespace Concept::GraphicsEngine::Direct3D;

CECubePlayground::CECubePlayground(const std::wstring& name, uint32_t width,
                                   uint32_t height, bool vSync): CEPlayground(
	name, width, height, vSync) {
}

bool CECubePlayground::LoadContent() {

	/*
	 * Create DX12 Device
	 */
	m_device = CEDevice::Create();

	auto description = m_device->GetDescription();
	m_logger->info("Device Created: {}", description);

	auto& commandQueue = m_device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	auto commandList = commandQueue.GetCommandList();

	/*
	 * Cube object
	 */
	m_cube = std::make_shared<Object::CED3DCubeObject>();

	auto vertices = m_cube->GetVertices();
	auto vertexStride = m_cube->GetVertexStride();
	auto indices = m_cube->GetIndices();

	/*
	 * Load vertex data:
	 */
	m_vertexBuffer = commandList->CopyVertexBuffer(vertices.size(), vertexStride, vertices.data());

	/*
	 * Load index data:
	 */
	m_indexBuffer = commandList->CopyIndexBuffer(indices.size(), DXGI_FORMAT_R16_UINT, indices.data());

	/*
	 * Execute command list to upload resources to GPU.
	 */
	commandQueue.ExecuteCommandList(commandList);

	/*
	 * Create Swap chain for window
	 */
	m_swapChain = m_device->CreateSwapChain(m_window->GetWindowHandle());
	m_swapChain->SetVSync(false);

	/*
	 * Create vertex input layout
	 */
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	/*
	 * Create root signature
	 * Allow input layout and deny unnecessary access to certain pipeline stages.
	 */
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	/*
	 * Single 32-bit constant root parameter that is used by vertex shader.
	 */
	CD3DX12_ROOT_PARAMETER1 rootParameters[1];
	rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription(
		_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	m_rootSignature = m_device->CreateRootSignature(rootSignatureDescription.Desc_1_1);

	WCHAR assetsPath[512];
	CETools::GetAssetsPath(assetsPath, _countof(assetsPath));
	std::wstring m_assetsPath = assetsPath;
	std::wstring vs_assetsPath = m_assetsPath + L"CEGEBasicVertexShader.cso";
	std::string vsDesc(vs_assetsPath.begin(), vs_assetsPath.end());
	/*
	 * Load shaders
	 * Load vertex shader.
	 */
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(vs_assetsPath.c_str(), &vertexShaderBlob));

	/*
	 * Load index shader.
	 */

	std::wstring ps_assetsPath = m_assetsPath + L"CEGEBasicPixelShader.cso";
	std::string psDesc(ps_assetsPath.begin(), ps_assetsPath.end());
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(ps_assetsPath.c_str(), &pixelShaderBlob));

	/*
	 * Create pipeline state object
	 */
	struct PipelineStateStream {
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	} pipelineStateStream;

	pipelineStateStream.pRootSignature = m_rootSignature->GetD3D12RootSignature().Get();
	pipelineStateStream.InputLayout = {inputLayout, _countof(inputLayout)};
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = m_swapChain->GetRenderTarget().GetRenderTargetFormats();

	m_pipelineStateObject = m_device->CreatePipelineStateObject(pipelineStateStream);

	/*
	 * Make sure index/vertex buffers are loaded before rendering first frame.
	 */
	commandQueue.Flush();

	return true;
}

void CECubePlayground::UnloadContent() {
	/*
 * Release global variables
 */
	m_indexBuffer.reset();
	m_vertexBuffer.reset();
	m_pipelineStateObject.reset();
	m_rootSignature.reset();
	m_depthTexture.reset();
	m_device.reset();
	m_swapChain.reset();
	m_window.reset();

}

void CECubePlayground::OnUpdate(UpdateEventArgs& e) {
	DisplayDebugFPSOnUpdate(e);

	/*
	 * Use render target from swapchain
	 */
	auto renderTarget = m_swapChain->GetRenderTarget();

	/*
	 * Set render target (with depth texture)
	 */
	renderTarget.AttachTexture(AttachmentPoint::DepthStencil, m_depthTexture);

	m_viewport = renderTarget.GetViewPort();

	/*
	 * Update model matrix
	 */
	float angle = static_cast<float>(e.TotalTime * 90.0);
	const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
	DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));

	/*
	 * Update view matrix
	 */
	const DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 0, -10, 1);
	const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
	const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(cameraPosition, focusPoint, upDirection);

	/*
	 * Update projection matrix.
	 */
	float aspectRatio = m_viewport.Width / m_viewport.Height;
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(m_cube->GetFieldOfView()), aspectRatio, 0.1f, 100.0f);
	DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(modelMatrix, viewMatrix);
	mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projectionMatrix);

	auto& commandQueue = m_device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandList = commandQueue.GetCommandList();

	/*
	 * Set pipeline state object
	 */
	commandList->SetPipelineState(m_pipelineStateObject);
	/*
	 * set root signatures
	 */
	commandList->SetGraphicsRootSignature(m_rootSignature);

	/*
	 * Set root parameters
	 */
	commandList->SetGraphics32BitConstants(0, mvpMatrix);

	const FLOAT clearColor[] = {0.4f, 0.6f, 0.9f, 1.0f};
	commandList->ClearTexture(renderTarget.GetTexture(AttachmentPoint::Color0), clearColor);
	commandList->ClearDepthStencilTexture(m_depthTexture, D3D12_CLEAR_FLAG_DEPTH);

	commandList->SetRenderTarget(renderTarget);
	commandList->SetViewport(renderTarget.GetViewPort());
	commandList->SetScissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX));

	/*
	 * Render Cube
	 */
	commandList->SetVertexBuffer(0, m_vertexBuffer);
	commandList->SetIndexBuffer(m_indexBuffer);
	commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawIndexed(m_indexBuffer->GetNumIndices());

	commandQueue.ExecuteCommandList(commandList);

	/*
	 * Present image to window
	 */
	m_swapChain->Present();

	OnRender();
}

void CECubePlayground::OnRender() {

}

void CECubePlayground::OnKeyPressed(KeyEventArgs& e) {
	switch (e.Key) {
	case KeyCode::V:
		m_swapChain->ToggleVSync();
		break;
	case KeyCode::Escape:
		/*
		 * Stop application if Escape key is pressed.
		 */
		CEGame::Get().Stop();
		break;
	case KeyCode::Enter:
		if (e.Alt) {
			[[fallthrough]];
		case KeyCode::F11:
			m_window->ToggleFullscreen();
			break;
		}
	}
}

void CECubePlayground::OnKeyReleased(KeyEventArgs& e) {
}

void CECubePlayground::OnMouseMoved(MouseMotionEventArgs& e) {

}

void CECubePlayground::OnMouseWheel(MouseWheelEventArgs& e) {
	auto tempFieldOfView = m_cube->GetFieldOfView();
	tempFieldOfView -= e.WheelDelta;
	m_cube->SetFieldOfView(std::clamp(tempFieldOfView, 12.0f, 90.0f));
	m_logger->info("Field of View: {}", m_cube->GetFieldOfView());
}

void CECubePlayground::OnResize(ResizeEventArgs& e) {
	m_logger->info("Window Resize: {}, {}", e.Width, e.Height);
	CEGame::Get().SetDisplaySize(e.Width, e.Height);

	/*
	 * Flush any pending commands before resizing resources
	 */
	m_device->Flush();

	/*
	 * Resize swap chain
	 */
	m_swapChain->Resize(e.Width, e.Height);

	/*
	 * Resize depth texture
	 */
	auto depthTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, e.Width, e.Height);
	/*
	 * Must be set on textures that will be used as a depth-stencil buffer
	 */
	depthTextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	/*
	 * Specify optimized clear values for depth buffer
	 */
	D3D12_CLEAR_VALUE optimizedClearValue = {};
	optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	optimizedClearValue.DepthStencil = {1.0f, 0};

	m_depthTexture = m_device->CreateTexture(depthTextureDesc, &optimizedClearValue);
}
