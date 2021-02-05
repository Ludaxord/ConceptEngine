#include "CERayTracingPlayground.h"


#include "CEDevice.h"
#include "CEGUI.h"
#include "CESwapChain.h"
#include "d3dx12.h"


using namespace Concept::GameEngine::Playground;
using namespace Concept::GraphicsEngine::Direct3D;
using namespace DirectX;

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
