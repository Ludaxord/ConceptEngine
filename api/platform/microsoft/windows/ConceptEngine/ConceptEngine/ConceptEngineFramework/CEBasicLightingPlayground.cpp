#include "CEBasicLightingPlayground.h"

#include <d3dcompiler.h>
#include <DirectXColors.h>


#include "CECommandList.h"
#include "CECommandQueue.h"
#include "CED3DSceneVisitor.h"
#include "CEDevice.h"
#include "CEGUI.h"
#include "CEHelper.h"
#include "CEMaterial.h"
#include "CERootSignature.h"
#include "CEScene.h"
#include "CESwapChain.h"
#include "CETexture.h"
#include "CETools.h"
#include "d3dx12.h"

using namespace Concept::GraphicsEngine::Direct3D;
using namespace DirectX;
using namespace Concept::GameEngine;
using namespace Playground;

struct Mat {
	XMMATRIX ModelMatrix;
	XMMATRIX ModelViewMatrix;
	XMMATRIX InverseTransposeModelViewMatrix;
	XMMATRIX ModelViewProjectionMatrix;
};

struct LightProperties {
	uint32_t NumPointLights;
	uint32_t NumSpotLights;
};

/*
 * Enum for root signature parameters
 * not using scoped enums to avoid explicit cat that would be required to use root indices in root signature.
 */
enum RootParameters {
	/*
	 * ConstantBuffer<Mat> MatCB : register(b0);
	 */
	MatricesCB,
	/*
	 * ConstantBuffer<Material> MaterialCB : register(b0, space1);
	 */
	MaterialCB,
	/*
	 * ConstantBuffer<LightProperties> LightPropertiesCB : register(b1);
	 */
	LightPropertiesCB,
	/*
	 * StructuredBuffer<PointLight> PointLights : register(t0);
	 */
	PointLights,
	/*
	 * StructuredBuffer<SpotLight> SpotLights : register(t1);
	 */
	SpotLights,
	/*
	 * Texture2D DiffuseTexture : register(t2);
	 */
	Textures,
	NumRootParameters
};

/*
 * Build look-at (world) matrix from a point, up and direction vectors
 */
XMMATRIX XM_CALLCONV LookAtMatrix(FXMVECTOR Position, FXMVECTOR Direction, FXMVECTOR Up) {
	assert(!XMVector3Equal(Direction, XMVectorZero()));
	assert(!XMVector3IsInfinite(Direction));
	assert(!XMVector3Equal(Up, XMVectorZero()));
	assert(!XMVector3IsInfinite(Up));

	XMVECTOR R2 = XMVector3Normalize(Direction);

	XMVECTOR R0 = XMVector3Cross(Up, R2);
	R0 = XMVector3Normalize(R0);

	XMVECTOR R1 = XMVector3Cross(R2, R0);
	XMMATRIX M(R0, R1, R2, Position);

	return M;
}

CEBasicLightingPlayground::CEBasicLightingPlayground(const std::wstring& name,
                                                     uint32_t width, uint32_t height, bool vSync): CEPlayground(
		name, width, height, vSync),
	m_viewPort(CD3DX12_VIEWPORT(
			0.0f,
			0.0f,
			static_cast<float>(width),
			static_cast<float>(height))
	),
	m_scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX)) {
	XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
	XMVECTOR cameraTarget = XMVectorSet(0, 5, 0, 1);
	XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

	m_camera.set_LookAt(cameraPos, cameraTarget, cameraUp);

	m_pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

	m_pAlignedCameraData->m_initialCamPos = m_camera.get_Translation();
	m_pAlignedCameraData->m_initialCamRot = m_camera.get_Rotation();
}

CEBasicLightingPlayground::~CEBasicLightingPlayground() {
	_aligned_free(m_pAlignedCameraData);
}

bool CEBasicLightingPlayground::LoadContent() {
	/*
	 * Create DX12 device
	 */
	m_device = CEDevice::Create();

	/*
	 * Create swap chain.
	 */
	m_swapChain = m_device->CreateSwapChain(m_window->GetWindowHandle(), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_swapChain->SetVSync(GetVSync());

	m_gui = m_device->CreateGUI(m_window->GetWindowHandle(), m_swapChain->GetRenderTarget());

	/*
	 * Magic here allows ImGui to process window messages.
	 */
	CEGame::Get().WndProcHandler += CEWindowProcEvent::slot(&CEGUI::WndProcHandler, m_gui);

	auto& commandQueue = m_device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	auto commandList = commandQueue.GetCommandList();

	/*
	 * Create some geometry to render.
	 */
	m_cube = commandList->CreateCube();
	m_sphere = commandList->CreateSphere();
	m_cone = commandList->CreateCone();
	m_torus = commandList->CreateTorus();
	m_plane = commandList->CreatePlane();

	/*
	 * Load some textures
	 */

	/*
	 * Load resources to commandQueue
	 */
	commandQueue.ExecuteCommandList(commandList);

	/*
	 * Load shaders
	 */
	WCHAR assetsPath[512];
	CETools::GetAssetsPath(assetsPath, _countof(assetsPath));
	std::wstring m_assetsPath = assetsPath;
	std::wstring vs_assetsPath = m_assetsPath + L"CEGEBasicLightingVertexShader.cso";
	std::string vsDesc(vs_assetsPath.begin(), vs_assetsPath.end());
	/*
	 * Load vertex shader.
	 */
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(vs_assetsPath.c_str(), &vertexShaderBlob));

	/*
	 * Load pixel shader.
	 */
	std::wstring ps_assetsPath = m_assetsPath + L"CEGEBasicLightingPixelShader.cso";
	std::string psDesc(ps_assetsPath.begin(), ps_assetsPath.end());
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(ps_assetsPath.c_str(), &pixelShaderBlob));

	/*
	 * Load pixel shader for unlit geometry (geometric shapes representing light sources, should be unlit)
	 */
	std::wstring ups_assetsPath = m_assetsPath + L"CEGEBasicLightingPixelShader.cso";
	std::string upsDesc(ps_assetsPath.begin(), ps_assetsPath.end());
	Microsoft::WRL::ComPtr<ID3DBlob> unlitPixelShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(ups_assetsPath.c_str(), &unlitPixelShaderBlob));

	/*
	 * Create root signature
	 * allow input layout and deny unnecessary access to certain pipeline stages.
	 */
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

	CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters];
	rootParameters[RootParameters::MatricesCB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
	                                                                    D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[RootParameters::MaterialCB].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
	                                                                    D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[RootParameters::LightPropertiesCB].InitAsConstants(sizeof(LightProperties) / 4, 1, 0,
	                                                                  D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[RootParameters::PointLights].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
	                                                                     D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[RootParameters::SpotLights].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
	                                                                    D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[RootParameters::Textures].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
	CD3DX12_STATIC_SAMPLER_DESC anisotropicSampler(0, D3D12_FILTER_ANISOTROPIC);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(RootParameters::NumRootParameters, rootParameters, 1, &linearRepeatSampler,
	                                  rootSignatureFlags);
	m_rootSignature = m_device->CreateRootSignature(rootSignatureDescription.Desc_1_1);

	/*
	 * Setup pipeline state.
	 */
	struct PipelineStateStream {
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC SampleDesc;
	} pipelineStateStream;

	/*
	 * Create color buffer with sRGB for gamma correction.
	 */
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

	/*
	 * Check best Multi-Sample quality level that can be used for given back buffer format;
	 */
	DXGI_SAMPLE_DESC sampleDesc = m_device->GetMultiSampleQualityLevels(backBufferFormat);

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = backBufferFormat;

	pipelineStateStream.pRootSignature = m_rootSignature->GetD3D12RootSignature().Get();
	pipelineStateStream.InputLayout = CEVertexPositionNormalTangentBitangentTexture::InputLayout;
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
	pipelineStateStream.DSVFormat = depthBufferFormat;
	pipelineStateStream.RTVFormats = rtvFormats;
	pipelineStateStream.SampleDesc = sampleDesc;

	m_pipelineState = m_device->CreatePipelineStateObject(pipelineStateStream);

	/*
	 * For unlit PSO only pixel shader is different.
	 */
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(unlitPixelShaderBlob.Get());

	m_unlitPipelineState = m_device->CreatePipelineStateObject(pipelineStateStream);

	/*
	 * Create off-screen render target with single color buffer and depth buffer.
	 */
	auto colorDesc = CD3DX12_RESOURCE_DESC::Tex2D(backBufferFormat, GetScreenWidth(), GetScreenHeight(), 1, 1,
	                                              sampleDesc.Count, sampleDesc.Quality,
	                                              D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	D3D12_CLEAR_VALUE colorClearValue;
	colorClearValue.Format = colorDesc.Format;
	colorClearValue.Color[0] = 0.4f;
	colorClearValue.Color[1] = 0.6f;
	colorClearValue.Color[2] = 0.9f;
	colorClearValue.Color[3] = 1.0f;

	auto colorTexture = m_device->CreateTexture(colorDesc, &colorClearValue);
	colorTexture->SetName(L"Color Render Target");

	/*
	 * Create depth buffer
	 */
	auto depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthBufferFormat, GetScreenWidth(), GetScreenHeight(), 1, 1,
	                                              sampleDesc.Count, sampleDesc.Quality,
	                                              D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	D3D12_CLEAR_VALUE depthClearValue;
	depthClearValue.Format = depthDesc.Format;
	depthClearValue.DepthStencil = {1.0f, 0};

	auto depthTexture = m_device->CreateTexture(depthDesc, &depthClearValue);
	depthTexture->SetName(L"Depth Render Target");

	/*
	 * Attach textures to render target
	 */
	m_renderTarget.AttachTexture(AttachmentPoint::Color0, colorTexture);
	m_renderTarget.AttachTexture(AttachmentPoint::DepthStencil, depthTexture);

	/*
	 * Wait for loading opperations to complete before rendering first frame
	 */
	commandQueue.Flush();

	return true;
}

void CEBasicLightingPlayground::UnloadContent() {
	m_cube.reset();
	m_sphere.reset();
	m_cone.reset();
	m_torus.reset();
	m_plane.reset();

	m_defaultTexture.reset();
	m_directXTexture.reset();
	m_earthTexture.reset();
	m_monaLisaTexture.reset();

	m_renderTarget.Reset();

	m_rootSignature.reset();
	m_pipelineState.reset();

	m_gui.reset();
	m_swapChain.reset();
	m_device.reset();
}

void CEBasicLightingPlayground::OnUpdate(UpdateEventArgs& e) {
	DisplayDebugFPSOnUpdate(e);

	m_swapChain->WaitForSwapChain();

	/*
	 * Update camera
	 */
	float speedMultiPier = (GetShift() ? 16.0f : 4.0f);

	XMVECTOR cameraTranslate = XMVectorSet(GetCameraRight() - GetCameraLeft(), 0.0f,
	                                       GetCameraForward() - GetCameraBackward(), 1.0f) * speedMultiPier *
		static_cast<float>(e.DeltaTime);
	XMVECTOR cameraPan = XMVectorSet(0.0f, GetCameraUp() - GetCameraDown(), 0.0f, 1.0f) * speedMultiPier * static_cast<
		float>(e.DeltaTime);
	m_camera.Translate(cameraTranslate, GraphicsEngine::Space::Local);
	m_camera.Translate(cameraPan, GraphicsEngine::Space::Local);

	XMVECTOR cameraRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(GetPitch()),
	                                                           XMConvertToRadians(GetYaw()), 0.0f);
	m_camera.set_Rotation(cameraRotation);

	XMMATRIX viewMatrix = m_camera.get_ViewMatrix();

	const int numPointLights = 4;
	const int numSpotLights = 4;

	static const XMVECTORF32 LightColors[] = {
		Colors::White,
		Colors::Orange,
		Colors::Yellow,
		Colors::Green,
		Colors::Blue,
		Colors::Indigo,
		Colors::Violet,
		Colors::White
	};

	static float lightAnimTime = 0.0f;
	if (GetAnimateLights()) {
		lightAnimTime += static_cast<float>(e.DeltaTime) * 0.5f * XM_PI;
	}

	const float radius = 8.0f;
	const float offset = 2.0f * XM_PI / numPointLights;
	const float offset2 = offset + (offset / 2.0f);

	/*
	 * Setup light buffers
	 */
	m_pointLights.resize(numPointLights);
	for (int i = 0; i < numPointLights; ++i) {
		Lighting::PointLight& l = m_pointLights[i];
		l.PositionWS = {
			static_cast<float>(std::sin(lightAnimTime + offset * i)) * radius,
			9.0f,
			static_cast<float>(std::cos(lightAnimTime + offset * i)) * radius,
			1.0f
		};
		XMVECTOR positionWS = XMLoadFloat4(&l.PositionWS);
		XMVECTOR positionVS = XMVector3TransformCoord(positionWS, viewMatrix);
		XMStoreFloat4(&l.PositionVS, positionVS);

		l.Color = XMFLOAT4(LightColors[i]);
		l.ConstantAttenuation = 1.0f;
		l.LinearAttenuation = 0.08f;
		l.QuadraticAttenuation = 0.0f;
	}

	m_spotLights.resize(numSpotLights);
	for (int i = 0; i < numSpotLights; ++i) {
		Lighting::SpotLight& l = m_spotLights[i];

		l.PositionWS = {
			static_cast<float>(std::sin(lightAnimTime + offset * i + offset2)) * radius,
			9.0f,
			static_cast<float>(std::cos(lightAnimTime + offset * i + offset2)) * radius,
			1.0f
		};
		XMVECTOR positionWS = XMLoadFloat4(&l.PositionWS);
		XMVECTOR positionVS = XMVector3TransformCoord(positionWS, viewMatrix);
		XMStoreFloat4(&l.PositionVS, positionVS);

		XMVECTOR directionWS = XMVector3Normalize(XMVectorSetW(XMVectorNegate(positionWS), 0));
		XMVECTOR directionVS = XMVector3Normalize(XMVector3TransformNormal(directionWS, viewMatrix));
		XMStoreFloat4(&l.DirectionWS, directionWS);
		XMStoreFloat4(&l.DirectionVS, directionVS);

		l.Color = XMFLOAT4(LightColors[numPointLights + i]);
		l.SpotAngle = XMConvertToRadians(45.0f);
		l.ConstantAttenuation = 1.0f;
		l.LinearAttenuation = 0.08f;
		l.QuadraticAttenuation = 0.0f;
	}

	OnRender();
}

void XM_CALLCONV ComputeMatrices(FXMMATRIX model, CXMMATRIX view, CXMMATRIX viewProjection, Mat& mat) {
	mat.ModelMatrix = model;
	mat.ModelViewMatrix = model * view;
	mat.InverseTransposeModelViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, mat.ModelViewMatrix));
	mat.ModelViewProjectionMatrix = model * viewProjection;
}

void CEBasicLightingPlayground::OnRender() {
	auto& commandQueue = m_device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandList = commandQueue.GetCommandList();

	/*
	 * Create scene visitor that is used to perform actual rendering of meshes in scenes
	 */
	Visitor::CED3DSceneVisitor visitor(*commandList);

	/*
	 * Clear render targets
	 */
	{
		FLOAT clearColor[] = {0.4f, 0.6f, 0.9f, 1.0f};
		commandList->ClearTexture(m_renderTarget.GetTexture(AttachmentPoint::Color0), clearColor);
		commandList->ClearDepthStencilTexture(m_renderTarget.GetTexture(AttachmentPoint::DepthStencil),
		                                      D3D12_CLEAR_FLAG_DEPTH);
	}

	commandList->SetPipelineState(m_pipelineState);
	commandList->SetGraphicsRootSignature(m_rootSignature);

	/*
	 * Upload Lights
	 */
	LightProperties lightProperties;
	lightProperties.NumPointLights = static_cast<uint32_t>(m_pointLights.size());
	lightProperties.NumSpotLights = static_cast<uint32_t>(m_spotLights.size());

	commandList->SetGraphics32BitConstants(RootParameters::LightPropertiesCB, lightProperties);
	commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::PointLights, m_pointLights);
	commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::SpotLights, m_spotLights);

	commandList->SetViewport(m_viewPort);
	commandList->SetScissorRect(m_scissorRect);

	commandList->SetRenderTarget(m_renderTarget);

	/*
	 * Draw earth sphere.
	 */
	XMMATRIX translationMatrix = XMMatrixTranslation(-4.0f, 2.0f, -4.0f);
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
	XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	XMMATRIX viewMatrix = m_camera.get_ViewMatrix();
	XMMATRIX viewProjectionMatrix = viewMatrix * m_camera.get_ProjectionMatrix();

	Mat matrices;
	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::Chrome);
	// commandList->SetShaderResourceView(RootParameters::Textures, 0, m_earthTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	/*
	 * Render earth sphere using SceneVisitor
	 */
	m_sphere->Accept(visitor);

	/*
	 * Add more elements to scene visitor by using Accept function
	 */

	/*
	 * Draw shapes to visualize position of lights in scene using unlit pixel shader.
	 */
	commandList->SetPipelineState(m_unlitPipelineState);

	MaterialProperties lightMaterial = CEMaterial::Zero;
	for (const auto& l : m_pointLights) {
		lightMaterial.Emissive = l.Color;
		XMVECTOR lightPos = XMLoadFloat4(&l.PositionWS);
		worldMatrix = XMMatrixTranslationFromVector(lightPos);
		ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

		commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
		commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, lightMaterial);
		m_sphere->Accept(visitor);
	}

	for (const auto& l : m_spotLights) {
		lightMaterial.Emissive = l.Color;
		XMVECTOR lightPos = XMLoadFloat4(&l.PositionWS);
		XMVECTOR lightDir = XMLoadFloat4(&l.PositionVS);
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);

		/*
		 * Rotate cone so it is facing Z axis
		 */
		rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));
		worldMatrix = rotationMatrix * LookAtMatrix(lightPos, lightDir, up);

		ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

		commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
		commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, lightMaterial);

		m_cone->Accept(visitor);
	}

	/*
	 * Resolve MSAA render target to swapchain back buffer
	 */
	auto& swapChainRT = m_swapChain->GetRenderTarget();
	auto swapChainBackBuffer = swapChainRT.GetTexture(AttachmentPoint::Color0);
	auto msaaRenderTarget = m_renderTarget.GetTexture(AttachmentPoint::Color0);

	commandList->ResolveSubResource(swapChainBackBuffer, msaaRenderTarget);

	/*
	 * Render GUI directly to swap chain render target
	 */
	OnGUI(commandList, swapChainRT);

	commandQueue.ExecuteCommandList(commandList);

	m_swapChain->Present();
}

void CEBasicLightingPlayground::OnGUI(const std::shared_ptr<CECommandList>& commandList,
                                      const CERenderTarget& renderTarget) {
	m_gui->NewFrame();

	static bool showDemoWindow = false;
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	m_gui->Render(commandList, renderTarget);
}

static bool g_AllowFullscreenToggle = true;

void CEBasicLightingPlayground::OnKeyPressed(KeyEventArgs& e) {
	if (!ImGui::GetIO().WantCaptureKeyboard) {
		switch (e.Key) {
		case KeyCode::Escape:
			CEGame::Get().Stop();
			break;
		case KeyCode::Enter:
			if (e.Alt) {
			case KeyCode::F11:
				if (g_AllowFullscreenToggle) {
					m_window->ToggleFullscreen();
					g_AllowFullscreenToggle = false;
				}
				break;
			}
		case KeyCode::V:
			m_swapChain->ToggleVSync();
			break;
		case KeyCode::R:
			/*
			 * Reset camera transform
			 */
			m_camera.set_Translation(m_pAlignedCameraData->m_initialCamPos);
			m_camera.set_Rotation(m_pAlignedCameraData->m_initialCamRot);
			SetPitch(0.0f);
			SetYaw(0.0f);
			break;
		case KeyCode::Up:
		case KeyCode::W:
			SetCameraForward(1.0f);
			break;
		case KeyCode::Left:
		case KeyCode::A:
			SetCameraLeft(1.0f);
			break;
		case KeyCode::Down:
		case KeyCode::S:
			SetCameraBackward(1.0f);
			break;
		case KeyCode::Right:
		case KeyCode::D:
			SetCameraRight(1.0f);
			break;
		case KeyCode::Q:
			SetCameraDown(1.0f);
			break;
		case KeyCode::E:
			SetCameraUp(1.0f);
			break;
		case KeyCode::Space:
			SetAnimateLights(!GetAnimateLights());
			break;
		case KeyCode::ShiftKey:
			SetShift(true);
			break;
		}
	}
}

void CEBasicLightingPlayground::OnKeyReleased(KeyEventArgs& e) {
	switch (e.Key) {
	case KeyCode::Enter:
		if (e.Alt) {
		case KeyCode::F11:
			g_AllowFullscreenToggle = true;
		}
		break;
	case KeyCode::Up:
	case KeyCode::W:
		SetCameraForward(0.0f);
		break;
	case KeyCode::Left:
	case KeyCode::A:
		SetCameraLeft(0.0f);
		break;
	case KeyCode::Down:
	case KeyCode::S:
		SetCameraBackward(0.0f);
		break;
	case KeyCode::Right:
	case KeyCode::D:
		SetCameraRight(0.0f);
		break;
	case KeyCode::Q:
		SetCameraDown(0.0f);
		break;
	case KeyCode::E:
		SetCameraUp(0.0f);
		break;
	case KeyCode::ShiftKey:
		SetShift(false);
		break;
	}
}

void CEBasicLightingPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
	const float mouseSpeed = 0.1f;
	if (!ImGui::GetIO().WantCaptureMouse) {
		if (e.LeftButton) {
			auto tempPitch = GetPitch() - e.RelY * mouseSpeed;
			SetPitch(std::clamp(tempPitch, -90.0f, 90.0f));

			auto tempYaw = GetYaw() - e.RelX * mouseSpeed;
			SetYaw(tempYaw);
		}
	}
}

void CEBasicLightingPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
	if (!ImGui::GetIO().WantCaptureMouse) {
		auto fov = m_camera.get_FoV();
		fov -= e.WheelDelta;
		fov = std::clamp(fov, 12.0f, 90.0f);
		m_camera.Set_FoV(fov);
		m_logger->info("FoV: {:.7}", fov);
	}
}

void CEBasicLightingPlayground::OnResize(ResizeEventArgs& e) {
	SetScreenResolution(std::max(1, e.Width), std::max(1, e.Height));

	m_swapChain->Resize(GetScreenWidth(), GetScreenHeight());

	float aspectRatio = GetScreenWidth() / (float)GetScreenHeight();
	m_camera.set_Projection(45.0f, aspectRatio, 0.1f, 100.0f);

	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(GetScreenWidth()),
	                              static_cast<float>(GetScreenHeight()));

	m_renderTarget.Resize(GetScreenWidth(), GetScreenHeight());
}
