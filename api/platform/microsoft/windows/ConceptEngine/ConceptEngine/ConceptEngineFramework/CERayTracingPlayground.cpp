#include "CERayTracingPlayground.h"



#include "CECommandList.h"
#include "CECommandQueue.h"
#include "CEDevice.h"
#include "CEGUI.h"
#include "CESwapChain.h"
#include "d3dx12.h"


using namespace Concept::GameEngine::Playground;
using namespace Concept::GraphicsEngine::Direct3D;
using namespace DirectX;

// An enum for root signature parameters.
// I'm not using scoped enums to avoid the explicit cast that would be required
// to use these as root indices in the root signature.
enum RootParameters {
	Buffer,
	AccelerationStructure,
	GeometryInputs,
	GlobalConstants,
	NumRootParameters
};

enum HitGroupParameters {
	Constants,
	NumParameters
};

CERayTracingPlayground::CERayTracingPlayground(const std::wstring& name,
                                               uint32_t width, uint32_t height, bool vSync): CEPlayground(name, width, height, vSync),
	                                              m_viewPort(CD3DX12_VIEWPORT(
			                                              0.0f,
			                                              0.0f,
			                                              static_cast<float>(width),
			                                              static_cast<float>(height))
	                                              ),
	                                              m_scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX)),
	                                              m_RenderScale(1.0f) {

	XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
	auto cameraTarget = XMVectorSet(0, 5, 0, 1);
	XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

	m_camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
	m_camera.set_Projection(45.0f, width / (float)height, 0.1f, 100.0f);

	m_pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

	m_pAlignedCameraData->m_initialCamPos = m_camera.get_Translation();
	m_pAlignedCameraData->m_initialCamRot = m_camera.get_Rotation();
	m_pAlignedCameraData->m_InitialFov = m_camera.get_FoV();
}

bool CERayTracingPlayground::LoadContent() {
	/*
 * Create devices
 */
	m_device = CEDevice::Create();
	m_swapChain = m_device->CreateSwapChain(m_window->GetWindowHandle(), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_swapChain->SetVSync(m_vSync);

	m_gui = m_device->CreateGUI(m_window->GetWindowHandle(), m_swapChain->GetRenderTarget());

	CEGame::Get().WndProcHandler += CEWindowProcEvent::slot(&CEGUI::WndProcHandler, m_gui);


	auto& commandQueue = m_device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	auto commandList = commandQueue.GetCommandList();

	m_cube = commandList->CreateCube();
	
	commandQueue.ExecuteCommandList(commandList);

	/*
	 * Allow input layout and deny unnecessary access to certain pipeline stages
	 * //TODO: Validate if it is correct
	 */
	{
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

		CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters];
		rootParameters[RootParameters::Buffer].InitAsUnorderedAccessView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[RootParameters::AccelerationStructure].InitAsShaderResourceView(0, 100, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[RootParameters::GeometryInputs].InitAsShaderResourceView(4, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[RootParameters::GlobalConstants].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);


		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(RootParameters::NumRootParameters, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

		m_RTXRootSignature = m_device->CreateRootSignature(rootSignatureDescription.Desc_1_1);
	}

	{
		CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters];
		
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(HitGroupParameters::NumParameters, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);
		m_hitGroupRootSignature = m_device->CreateRootSignature(rootSignatureDescription.Desc_1_1);
	}
	
	// Setup the Ray Tracing pipeline state.
	struct RayTracingPipelineStateStream {
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	} rayTracingPipelineStateStream;



	commandQueue.Flush();
	
	return true;
}

void Concept::GameEngine::Playground::CERayTracingPlayground::UnloadContent() {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnUpdate(UpdateEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnRender() {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnKeyPressed(KeyEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnKeyReleased(KeyEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnResize(ResizeEventArgs& e) {
}

void Concept::GameEngine::Playground::CERayTracingPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
}
