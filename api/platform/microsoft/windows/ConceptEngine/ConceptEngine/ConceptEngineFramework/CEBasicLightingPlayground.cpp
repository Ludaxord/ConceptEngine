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
	MatricesCB,
	// ConstantBuffer<Mat> MatCB : register(b0);
	MaterialCB,
	// ConstantBuffer<Material> MaterialCB : register( b0, space1 );
	LightPropertiesCB,
	// ConstantBuffer<LightProperties> LightPropertiesCB : register( b1 );
	PointLights,
	// StructuredBuffer<PointLight> PointLights : register( t0 );
	SpotLights,
	// StructuredBuffer<SpotLight> SpotLights : register( t1 );
	Textures,
	// Texture2D DiffuseTexture : register( t2 );
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
	// Create the DX12 device.
	m_device = CEDevice::Create();

	// Create a swap chain.
	m_swapChain = m_device->CreateSwapChain(m_window->GetWindowHandle(), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_swapChain->SetVSync(m_vSync);

	m_gui = m_device->CreateGUI(m_window->GetWindowHandle(), m_swapChain->GetRenderTarget());

	// This magic here allows ImGui to process window messages.
	CEGame::Get().WndProcHandler += CEWindowProcEvent::slot(&CEGUI::WndProcHandler, m_gui);

	auto& commandQueue = m_device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	auto commandList = commandQueue.GetCommandList();

	// Create some geometry to render.
	m_cube = commandList->CreateCube();
	m_sphere = commandList->CreateSphere();
	m_cone = commandList->CreateCone();
	m_torus = commandList->CreateTorus();
	m_plane = commandList->CreatePlane();

	WCHAR assetsPath[512];
	CETools::GetAssetsPath(assetsPath, _countof(assetsPath));
	std::wstring m_assetsPath = assetsPath;
	// Load some textures
	m_defaultTexture = commandList->LoadTextureFromFile(m_assetsPath + L"DefaultWhite.bmp", true);
	m_directXTexture = commandList->LoadTextureFromFile(m_assetsPath + L"Directx9.png", true);
	m_earthTexture = commandList->LoadTextureFromFile(m_assetsPath + L"earth.dds", true);
	m_monaLisaTexture = commandList->LoadTextureFromFile(m_assetsPath + L"Mona_Lisa.jpg", true);

	// Start loading resources...
	commandQueue.ExecuteCommandList(commandList);
	/*
	 * Load shaders
	 */
	/*
	 * Load vertex shader.
	 */
	std::wstring vs_assetsPath = m_assetsPath + L"CEGEBasicLightingVertexShader.cso";
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(vs_assetsPath.c_str(), &vertexShaderBlob));

	/*
	 * Load pixel shader.
	 */
	std::wstring ps_assetsPath = m_assetsPath + L"CEGEBasicLightingPixelShader.cso";
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(ps_assetsPath.c_str(), &pixelShaderBlob));

	/*
	 * Load pixel shader for unlit geometry (geometric shapes representing light sources, should be unlit)
	 */
	std::wstring ups_assetsPath = m_assetsPath + L"CEGEBasicLightingPixelShader.cso";
	Microsoft::WRL::ComPtr<ID3DBlob> unlitPixelShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(ups_assetsPath.c_str(), &unlitPixelShaderBlob));

	// Create a root signature.
	// Allow input layout and deny unnecessary access to certain pipeline stages.
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

	// Setup the pipeline state.
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

	// Create a color buffer with sRGB for gamma correction.
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

	// Check the best multisample quality level that can be used for the given back buffer format.
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

	// For the unlit PSO, only the pixel shader is different.
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(unlitPixelShaderBlob.Get());

	m_unlitPipelineState = m_device->CreatePipelineStateObject(pipelineStateStream);

	// Create an off-screen render target with a single color buffer and a depth buffer.
	auto colorDesc = CD3DX12_RESOURCE_DESC::Tex2D(backBufferFormat, m_width, m_height, 1, 1, sampleDesc.Count,
	                                              sampleDesc.Quality, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	D3D12_CLEAR_VALUE colorClearValue;
	colorClearValue.Format = colorDesc.Format;
	colorClearValue.Color[0] = 0.4f;
	colorClearValue.Color[1] = 0.6f;
	colorClearValue.Color[2] = 0.9f;
	colorClearValue.Color[3] = 1.0f;

	auto colorTexture = m_device->CreateTexture(colorDesc, &colorClearValue);
	colorTexture->SetName(L"Color Render Target");

	// Create a depth buffer.
	auto depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthBufferFormat, m_width, m_height, 1, 1, sampleDesc.Count,
	                                              sampleDesc.Quality, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	D3D12_CLEAR_VALUE depthClearValue;
	depthClearValue.Format = depthDesc.Format;
	depthClearValue.DepthStencil = {1.0f, 0};

	auto depthTexture = m_device->CreateTexture(depthDesc, &depthClearValue);
	depthTexture->SetName(L"Depth Render Target");

	// Attach the textures to the render target.
	m_renderTarget.AttachTexture(AttachmentPoint::Color0, colorTexture);
	m_renderTarget.AttachTexture(AttachmentPoint::DepthStencil, depthTexture);

	commandQueue.Flush(); // Wait for loading operations to complete before rendering the first frame.

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

	// Update the camera.
	float speedMultipler = (m_shift ? 16.0f : 4.0f);

	XMVECTOR cameraTranslate = XMVectorSet(m_right - m_left, 0.0f, m_forward - m_backward, 1.0f) * speedMultipler *
		static_cast<float>(e.DeltaTime);
	XMVECTOR cameraPan =
		XMVectorSet(0.0f, m_up - m_down, 0.0f, 1.0f) * speedMultipler * static_cast<float>(e.DeltaTime);
	m_camera.Translate(cameraTranslate, GraphicsEngine::Space::Local);
	m_camera.Translate(cameraPan, GraphicsEngine::Space::Local);

	XMVECTOR cameraRotation =
		XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_pitch), XMConvertToRadians(m_yaw), 0.0f);
	m_camera.set_Rotation(cameraRotation);

	XMMATRIX viewMatrix = m_camera.get_ViewMatrix();

	const int numPointLights = 4;
	const int numSpotLights = 4;

	static const XMVECTORF32 LightColors[] = {
		Colors::White, Colors::Orange, Colors::Yellow, Colors::Green,
		Colors::Blue, Colors::Indigo, Colors::Violet, Colors::White
	};

	static float lightAnimTime = 0.0f;
	if (m_animateLights) {
		lightAnimTime += static_cast<float>(e.DeltaTime) * 0.5f * XM_PI;
	}

	const float radius = 8.0f;
	const float offset = 2.0f * XM_PI / numPointLights;
	const float offset2 = offset + (offset / 2.0f);
	// Setup the light buffers.
	m_pointLights.resize(numPointLights);
	for (int i = 0; i < numPointLights; ++i) {
		Lighting::PointLight& l = m_pointLights[i];

		l.PositionWS = {
			static_cast<float>(std::sin(lightAnimTime + offset * i)) * radius, 9.0f,
			static_cast<float>(std::cos(lightAnimTime + offset * i)) * radius, 1.0f
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
			static_cast<float>(std::sin(lightAnimTime + offset * i + offset2)) * radius, 9.0f,
			static_cast<float>(std::cos(lightAnimTime + offset * i + offset2)) * radius, 1.0f
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

	// Create a scene visitor that is used to perform the actual rendering of the meshes in the scenes.
	Visitor::CED3DSceneVisitor visitor(*commandList);

	// Clear the render targets.
	{
		FLOAT clearColor[] = {0.4f, 0.6f, 0.9f, 1.0f};

		commandList->ClearTexture(m_renderTarget.GetTexture(AttachmentPoint::Color0), clearColor);
		commandList->ClearDepthStencilTexture(m_renderTarget.GetTexture(AttachmentPoint::DepthStencil),
		                                      D3D12_CLEAR_FLAG_DEPTH);
	}

	commandList->SetPipelineState(m_pipelineState);
	commandList->SetGraphicsRootSignature(m_rootSignature);

	// Upload lights
	LightProperties lightProps;
	lightProps.NumPointLights = static_cast<uint32_t>(m_pointLights.size());
	lightProps.NumSpotLights = static_cast<uint32_t>(m_spotLights.size());

	commandList->SetGraphics32BitConstants(RootParameters::LightPropertiesCB, lightProps);
	commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::PointLights, m_pointLights);
	commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::SpotLights, m_spotLights);

	commandList->SetViewport(m_viewPort);
	commandList->SetScissorRect(m_scissorRect);

	commandList->SetRenderTarget(m_renderTarget);

	// Draw the earth sphere
	XMMATRIX translationMatrix = XMMatrixTranslation(-4.0f, 2.0f, -4.0f);
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
	XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	XMMATRIX viewMatrix = m_camera.get_ViewMatrix();
	XMMATRIX viewProjectionMatrix = viewMatrix * m_camera.get_ProjectionMatrix();

	Mat matrices;
	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::White);
	commandList->SetShaderResourceView(RootParameters::Textures, 0, m_earthTexture,
	                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// Render the earth sphere using the SceneVisitor.
	m_sphere->Accept(visitor);

	// Draw a cube
	translationMatrix = XMMatrixTranslation(4.0f, 4.0f, 4.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
	scaleMatrix = XMMatrixScaling(4.0f, 8.0f, 4.0f);
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::White);
	commandList->SetShaderResourceView(RootParameters::Textures, 0, m_monaLisaTexture,
	                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// Render the Mona Lisa cube with the SceneVisitor.
	m_cube->Accept(visitor);

	// Draw a torus
	translationMatrix = XMMatrixTranslation(4.0f, 0.6f, -4.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
	scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::PianoBlack);
	commandList->SetShaderResourceView(RootParameters::Textures, 0, m_defaultTexture,
	                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	m_torus->Accept(visitor);

	// Floor plane.
	float scalePlane = 20.0f;
	float translateOffset = scalePlane / 2.0f;

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	rotationMatrix = XMMatrixIdentity();
	scaleMatrix = XMMatrixScaling(scalePlane, 1.0f, scalePlane);
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::Chrome);
	// commandList->SetShaderResourceView(RootParameters::Textures, 0, m_directXTexture,
	//                                    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// Render the plane using the SceneVisitor.
	m_plane->Accept(visitor);

	// Back wall
	translationMatrix = XMMatrixTranslation(0, translateOffset, translateOffset);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

	// Render the plane using the SceneVisitor.
	m_plane->Accept(visitor);

	// Ceiling plane
	translationMatrix = XMMatrixTranslation(0, translateOffset * 2.0f, 0);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(180));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

	// Render the plane using the SceneVisitor.
	m_plane->Accept(visitor);

	// Front wall
	translationMatrix = XMMatrixTranslation(0, translateOffset, -translateOffset);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(90));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

	// Render the plane using the SceneVisitor.
	m_plane->Accept(visitor);

	// Left wall
	translationMatrix = XMMatrixTranslation(-translateOffset, translateOffset, 0);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) * XMMatrixRotationY(XMConvertToRadians(-90));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::Chrome);
	commandList->SetShaderResourceView(RootParameters::Textures, 0, m_defaultTexture,
	                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// Render the plane using the SceneVisitor.
	m_plane->Accept(visitor);

	// Right wall
	translationMatrix = XMMatrixTranslation(translateOffset, translateOffset, 0);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) * XMMatrixRotationY(XMConvertToRadians(90));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::Chrome);

	// Render the plane using the SceneVisitor.
	m_plane->Accept(visitor);

	// Draw shapes to visualize the position of the lights in the scene using an unlit pixel shader.
	commandList->SetPipelineState(m_unlitPipelineState);

	MaterialProperties lightMaterial = CEMaterial::PianoBlack;
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
		XMVECTOR lightDir = XMLoadFloat4(&l.DirectionWS);
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);

		// Rotate the cone so it is facing the Z axis.
		rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));
		worldMatrix = rotationMatrix * LookAtMatrix(lightPos, lightDir, up);

		ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

		commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
		commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, lightMaterial);

		m_sphere->Accept(visitor);
	}

	// Resolve the MSAA render target to the swapchain's backbuffer.
	auto& swapChainRT = m_swapChain->GetRenderTarget();
	auto swapChainBackBuffer = swapChainRT.GetTexture(AttachmentPoint::Color0);
	auto msaaRenderTarget = m_renderTarget.GetTexture(AttachmentPoint::Color0);

	commandList->ResolveSubResource(swapChainBackBuffer, msaaRenderTarget);

	// Render the GUI directly to the swap chain's render target.
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
			m_pitch = 0.0f;
			m_yaw = 0.0f;
			break;
		case KeyCode::Up:
		case KeyCode::W:
			m_forward = 1.0f;
			break;
		case KeyCode::Left:
		case KeyCode::A:
			m_left = 1.0f;
			break;
		case KeyCode::Down:
		case KeyCode::S:
			m_backward = 1.0f;
			break;
		case KeyCode::Right:
		case KeyCode::D:
			m_right = 1.0f;
			break;
		case KeyCode::Q:
			m_down = 1.0f;
			break;
		case KeyCode::E:
			m_up = 1.0f;
			break;
		case KeyCode::Space:
			m_animateLights = !m_animateLights;
			break;
		case KeyCode::ShiftKey:
			m_shift = true;
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
		m_forward = 0.0f;
		break;
	case KeyCode::Left:
	case KeyCode::A:
		m_left = 0.0f;
		break;
	case KeyCode::Down:
	case KeyCode::S:
		m_backward = 0.0f;
		break;
	case KeyCode::Right:
	case KeyCode::D:
		m_right = 0.0f;
		break;
	case KeyCode::Q:
		m_down = 0.0f;
		break;
	case KeyCode::E:
		m_up = 0.0f;
		break;
	case KeyCode::ShiftKey:
		m_shift = false;
		break;
	}
}

void CEBasicLightingPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
	const float mouseSpeed = 0.1f;

	if (!ImGui::GetIO().WantCaptureMouse) {
		if (e.LeftButton) {
			m_pitch -= e.RelY * mouseSpeed;

			m_pitch = std::clamp(m_pitch, -90.0f, 90.0f);

			m_yaw -= e.RelX * mouseSpeed;
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
	m_width = std::max(1, e.Width);
	m_height = std::max(1, e.Height);

	m_swapChain->Resize(m_width, m_height);

	float aspectRatio = m_width / (float)m_height;
	m_camera.set_Projection(45.0f, aspectRatio, 0.1f, 100.0f);

	m_viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height));

	m_renderTarget.Resize(m_width, m_height);
}
