#include "CERTXPlayground.h"


#include "CECommandQueue.h"
#include "CEDevice.h"
#include "CEGUI.h"
#include "CESwapChain.h"
#include "d3dx12.h"

using namespace Concept::GameEngine::Playground;
using namespace Concept::GraphicsEngine::Direct3D;
using namespace DirectX;

CERTXPlayground::CERTXPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync) : CEPlayground(
		name, width, height, vSync),
	m_viewPort(CD3DX12_VIEWPORT(
			0.0f,
			0.0f,
			static_cast<float>(width),
			static_cast<float>(height))
	),
	m_scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX)), m_RenderScale(1.0f) {

	DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(0, 5, -20, 1);
	DirectX::XMVECTOR cameraTarget = DirectX::XMVectorSet(0, 5, 0, 1);
	DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 0);

	m_camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
	m_camera.set_Projection(45.0f, width / (float)height, 0.1f, 100.0f);

	m_pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

	m_pAlignedCameraData->m_initialCamPos = m_camera.get_Translation();
	m_pAlignedCameraData->m_initialCamRot = m_camera.get_Rotation();
	m_pAlignedCameraData->m_InitialFov = m_camera.get_FoV();
}

CERTXPlayground::~CERTXPlayground() {
}

bool CERTXPlayground::LoadContent() {
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

	/*
	 * Create scene based on Microsoft Tutorial.
	 * TODO: Move attributes to CEMaterial, for now just test rtx
	 */
	{
		auto SetAttributes = [&](
			UINT primitiveIndex,
			const XMFLOAT4& albedo,
			float reflectanceCoef = 0.0f,
			float diffuseCoef = 0.9f,
			float specularCoef = 0.7f,
			float specularPower = 50.0f,
			float stepScale = 1.0f
		) {
			auto& attributes = m_aabbMaterialCB[primitiveIndex];
			attributes.albedo = albedo;
			attributes.reflectanceCoef = reflectanceCoef;
			attributes.diffuseCoef = diffuseCoef;
			attributes.specularCoef = specularCoef;
			attributes.specularPower = specularPower;
			attributes.stepScale = stepScale;
		};

		m_planeMaterialCB = {XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f), 0.25f, 1, 0.4f, 50, 1};

		/*
		 * Albedos
		 */
		XMFLOAT4 green = XMFLOAT4(0.1f, 1.0f, 0.5f, 1.0f);
		XMFLOAT4 red = XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
		XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.5f, 1.0f);

		UINT offset = 0;
		/*
		 * Analytic primitives
		 */
		{
			using namespace AnalyticPrimitive;
			SetAttributes(offset + AABB, red);
			SetAttributes(offset + Spheres, ChromiumReflectance, 1);
			offset += AnalyticPrimitive::Count;
		}

		/*
		 * Volumetric primitives
		 */
		{
			using namespace VolumetricPrimitive;
			SetAttributes(offset + Metaballs, ChromiumReflectance, 1);
			offset += VolumetricPrimitive::Count;
		}

		/*
		 * Signed distance primitives
		 */
		{
			using namespace SignedDistancePrimitive;
			using namespace SignedDistancePrimitive;
			SetAttributes(offset + MiniSpheres, green);
			SetAttributes(offset + IntersectedRoundCube, green);
			SetAttributes(offset + SquareTorus, ChromiumReflectance, 1);
			SetAttributes(offset + TwistedTorus, yellow, 0, 1.0f, 0.7f, 50, 0.5f);
			SetAttributes(offset + Cog, yellow, 0, 1.0f, 0.1f, 2);
			SetAttributes(offset + Cylinder, red);
			SetAttributes(offset + FractalPyramid, green, 0, 1, 0.1f, 4, 0.8f);
		}

		/*
		 * Camera setup in constructor
		 */
		{
		}

		/*
		 * Setup lights
		 */
		{
			XMFLOAT4 lightPosition;
			XMFLOAT4 lightAmbientColor;
			XMFLOAT4 lightDiffuseColor;

			lightPosition = XMFLOAT4(0.0f, 18.0f, -20.0f, 0.0f);
			m_sceneCB->lightPosition = XMLoadFloat4(&lightPosition);
			
			lightAmbientColor = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
			m_sceneCB->lightAmbientColor = XMLoadFloat4(&lightAmbientColor);
			
			float d = 0.6f;
			lightDiffuseColor = XMFLOAT4(d, d, d, 1.0f);
			m_sceneCB->lightDiffuseColor = XMLoadFloat4(&lightDiffuseColor);
		}
	}

	//TODO: Create Ray Tracing Resources

	return true;
}

void CERTXPlayground::UnloadContent() {
}

void CERTXPlayground::OnUpdate(UpdateEventArgs& e) {
}

void CERTXPlayground::OnRender() {
}

void CERTXPlayground::OnKeyPressed(KeyEventArgs& e) {
}

void CERTXPlayground::OnKeyReleased(KeyEventArgs& e) {
}

void CERTXPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
}

void CERTXPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
}

void CERTXPlayground::OnResize(ResizeEventArgs& e) {
}

void CERTXPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
}

void CERTXPlayground::OnGUI(const std::shared_ptr<GraphicsEngine::Direct3D::CECommandList>& commandList,
                            const GraphicsEngine::Direct3D::CERenderTarget& renderTarget) {
}

void CERTXPlayground::RescaleHDRRenderTarget(float scale) {
}
