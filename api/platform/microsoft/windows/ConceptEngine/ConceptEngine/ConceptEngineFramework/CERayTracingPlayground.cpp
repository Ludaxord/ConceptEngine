#include "CERayTracingPlayground.h"


#include "CECommandList.h"
#include "CECommandQueue.h"
#include "CEDevice.h"
#include "CEGUI.h"
#include "CEScene.h"
#include "CESwapChain.h"
#include "CETools.h"
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
                                               uint32_t width, uint32_t height, bool vSync): CEPlayground(name, width,
		height, vSync),
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
	auto description = m_device->GetDescription();
	m_logger->info("Device Created: {}", description);
	m_swapChain = m_device->CreateSwapChain(m_window->GetWindowHandle(), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_swapChain->SetVSync(m_vSync);

	m_gui = m_device->CreateGUI(m_window->GetWindowHandle(), m_swapChain->GetRenderTarget());

	CEGame::Get().WndProcHandler += CEWindowProcEvent::slot(&CEGUI::WndProcHandler, m_gui);


	auto& commandQueue = m_device->GetCommandQueue(
		D3D12_COMMAND_LIST_TYPE_DIRECT /* D3D12_COMMAND_LIST_TYPE_COPY/D3D12_COMMAND_LIST_TYPE_DIRECT */);
	auto commandList = commandQueue.GetCommandList();

	m_cube = commandList->CreateCube();

	commandList->CreateAccelerationStructures(m_cube);

	commandQueue.ExecuteCommandList(commandList);


	/**
	 * Create State Object based on passed template PipelineStateStream
	 * To create basic Ray Tracing Pipeline State
	 * Need 16 subobjects:
	* 1 for DXIL library
	* 3 for the hit-groups (triangle hit group, plane hit-group, shadow-hit group)
	* 2 for RayGen root-signature (root-signature and the subobject association)
	* 2 for triangle hit-program root-signature (root-signature and the subobject association)
	* 2 for the plane-hit root-signature (root-signature and the subobject association)
	* 2 for shadow-program and miss root-signature (root-signature and the subobject association)
	* 2 for shader config (shared between all programs. 1 for the config, 1 for association)
	* 1 for pipeline config
	* 1 for the global root signature
	 */
	{
		WCHAR assetsPath[512];
		CETools::GetAssetsPath(assetsPath, _countof(assetsPath));
		std::wstring m_assetsPath = assetsPath;
		std::vector<D3D12_STATE_SUBOBJECT> subobjects;
		const WCHAR* entryPoints[] = {L"rayGen", L"miss", L"planeChs", L"triangleChs", L"shadowMiss", L"shadowChs"};
		auto dxilLibrary = m_device->LoadDXILLibrary(m_assetsPath + L"CEGERayTracingVertexShader.hlsl", entryPoints);

		
	}

	commandQueue.Flush();

	return true;
}

void CERayTracingPlayground::UnloadContent() {
	m_gui.reset();
	m_swapChain.reset();
	m_device.reset();
}

void CERayTracingPlayground::OnUpdate(UpdateEventArgs& e) {
}

void CERayTracingPlayground::OnRender() {
}

void CERayTracingPlayground::OnKeyPressed(KeyEventArgs& e) {
}

void CERayTracingPlayground::OnKeyReleased(KeyEventArgs& e) {
}

void CERayTracingPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
}

void CERayTracingPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
}

void CERayTracingPlayground::OnResize(ResizeEventArgs& e) {
}

void CERayTracingPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
}
