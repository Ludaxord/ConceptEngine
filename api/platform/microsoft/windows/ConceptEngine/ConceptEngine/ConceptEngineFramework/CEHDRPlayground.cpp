#include "CEHDRPlayground.h"

#include <d3dcompiler.h>
#include <DirectXColors.h>


#include "CECommandList.h"
#include "CECommandQueue.h"
#include "CED3DSceneVisitor.h"
#include "CEDevice.h"
#include "CEGUI.h"
#include "CEHelper.h"
#include "CEMaterial.h"
#include "CEPipelineStateObject.h"
#include "CERootSignature.h"
#include "CEScene.h"
#include "CESwapChain.h"
#include "CETexture.h"
#include "CETools.h"
#include "d3dx12.h"

using namespace ConceptFramework::GameEngine::Playground;
using namespace ConceptFramework::GraphicsEngine::Direct3D;
using namespace DirectX;

/*
 * TODO: create separate class for all properties and helper functions, to get rid of copying same code in multiple places
 */
struct Mat {
	DirectX::XMMATRIX ModelMatrix;
	DirectX::XMMATRIX ModelViewMatrix;
	DirectX::XMMATRIX InverseTransposeModelViewMatrix;
	DirectX::XMMATRIX ModelViewProjectionMatrix;
};

struct LightProperties {
	uint32_t NumPointLights;
	uint32_t NumSpotLights;
};

enum TonemapMethod : uint32_t {
	TM_Linear,
	TM_Reinhard,
	TM_ReinhardSq,
	TM_ACESFilmic,
};

struct TonemapParameters {
	TonemapParameters()
		: TonemapMethod(TM_Reinhard)
		  , Exposure(0.0f)
		  , MaxLuminance(1.0f)
		  , K(1.0f)
		  , A(0.22f)
		  , B(0.3f)
		  , C(0.1f)
		  , D(0.2f)
		  , E(0.01f)
		  , F(0.3f)
		  , LinearWhite(11.2f)
		  , Gamma(2.2f) {
	}

	// The method to use to perform tonemapping.
	TonemapMethod TonemapMethod;
	// Exposure should be expressed as a relative exposure value (-2, -1, 0, +1, +2 )
	float Exposure;

	// The maximum luminance to use for linear tonemapping.
	float MaxLuminance;

	// Reinhard constant. Generally this is 1.0.
	float K;

	// ACES Filmic parameters
	// See: https://www.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting/142
	float A; // Shoulder strength
	float B; // Linear strength
	float C; // Linear angle
	float D; // Toe strength
	float E; // Toe Numerator
	float F; // Toe denominator
	// Note E/F = Toe angle.
	float LinearWhite;
	float Gamma;
};

TonemapParameters g_TonemapParameters;

// An enum for root signature parameters.
// I'm not using scoped enums to avoid the explicit cast that would be required
// to use these as root indices in the root signature.
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


// Helper to display a little (?) mark which shows a tooltip when hovered.
static void ShowHelpMarker(const char* desc) {
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

// Number of values to plot in the tonemapping curves.
static const int VALUES_COUNT = 256;
// Maximum HDR value to normalize the plot samples.
static const float HDR_MAX = 12.0f;

float LinearTonemapping(float HDR, float max) {
	if (max > 0.0f) {
		return std::clamp(HDR / max, 0.0f, 1.0f);
	}
	return HDR;
}

float LinearTonemappingPlot(void*, int index) {
	return LinearTonemapping(index / (float)VALUES_COUNT * HDR_MAX, g_TonemapParameters.MaxLuminance);
}

// Reinhard tone mapping.
// See: http://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf
float ReinhardTonemapping(float HDR, float k) {
	return HDR / (HDR + k);
}

float ReinhardTonemappingPlot(void*, int index) {
	return ReinhardTonemapping(index / (float)VALUES_COUNT * HDR_MAX, g_TonemapParameters.K);
}

float ReinhardSqrTonemappingPlot(void*, int index) {
	float reinhard = ReinhardTonemapping(index / (float)VALUES_COUNT * HDR_MAX, g_TonemapParameters.K);
	return reinhard * reinhard;
}

// ACES Filmic
// See: https://www.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting/142
float ACESFilmicTonemapping(float x, float A, float B, float C, float D, float E, float F) {
	return (((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - (E / F));
}

float ACESFilmicTonemappingPlot(void*, int index) {
	float HDR = index / (float)VALUES_COUNT * HDR_MAX;
	return ACESFilmicTonemapping(HDR, g_TonemapParameters.A, g_TonemapParameters.B, g_TonemapParameters.C,
	                             g_TonemapParameters.D, g_TonemapParameters.E, g_TonemapParameters.F) /
		ACESFilmicTonemapping(g_TonemapParameters.LinearWhite, g_TonemapParameters.A, g_TonemapParameters.B,
		                      g_TonemapParameters.C, g_TonemapParameters.D, g_TonemapParameters.E,
		                      g_TonemapParameters.F);
}

// Builds a look-at (world) matrix from a point, up and direction vectors.
DirectX::XMMATRIX XM_CALLCONV LookAtMatrix(DirectX::FXMVECTOR Position, DirectX::FXMVECTOR Direction,
                                           DirectX::FXMVECTOR Up) {
	assert(!DirectX::XMVector3Equal(Direction, DirectX::XMVectorZero()));
	assert(!DirectX::XMVector3IsInfinite(Direction));
	assert(!DirectX::XMVector3Equal(Up, DirectX::XMVectorZero()));
	assert(!DirectX::XMVector3IsInfinite(Up));

	DirectX::XMVECTOR R2 = DirectX::XMVector3Normalize(Direction);

	DirectX::XMVECTOR R0 = DirectX::XMVector3Cross(Up, R2);
	R0 = DirectX::XMVector3Normalize(R0);

	DirectX::XMVECTOR R1 = DirectX::XMVector3Cross(R2, R0);

	DirectX::XMMATRIX M(R0, R1, R2, Position);

	return M;
}


void XM_CALLCONV ComputeMatrices(DirectX::FXMMATRIX model, DirectX::CXMMATRIX view, DirectX::CXMMATRIX viewProjection,
                                 Mat& mat) {
	mat.ModelMatrix = model;
	mat.ModelViewMatrix = model * view;
	mat.InverseTransposeModelViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, mat.ModelViewMatrix));
	mat.ModelViewProjectionMatrix = model * viewProjection;
}

CEHDRPlayground::CEHDRPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync): CEPlayground(
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

CEHDRPlayground::~CEHDRPlayground() {
	_aligned_free(m_pAlignedCameraData);
}

bool CEHDRPlayground::LoadContent() {
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

	// Create some geometry to render.
	m_cube = commandList->CreateCube();
	m_sphere = commandList->CreateSphere();
	m_cone = commandList->CreateCone();
	m_torus = commandList->CreateTorus();
	m_plane = commandList->CreatePlane();
	m_cylinder = commandList->CreateCylinder();

	// Create an inverted (reverse winding order) cube so the insides are not clipped.
	m_skybox = commandList->CreateCube(1.0f, true);

	WCHAR assetsPath[512];
	CETools::GetAssetsPath(assetsPath, _countof(assetsPath));
	std::wstring m_assetsPath = assetsPath;
	// Load some textures
	m_defaultTexture = commandList->LoadTextureFromFile(m_assetsPath + L"DefaultWhite.bmp", true);
	m_directXTexture = commandList->LoadTextureFromFile(m_assetsPath + L"DirectX12Ultimate.png", true);
	m_earthTexture = commandList->LoadTextureFromFile(m_assetsPath + L"earth.dds", true);
	m_monaLisaTexture = commandList->LoadTextureFromFile(m_assetsPath + L"Mona_Lisa.jpg", true);
	// m_GraceCathedralTexture = commandList->LoadTextureFromFile(m_assetsPath + L"grace-new.hdr", true);
	// m_GraceCathedralTexture = commandList->LoadTextureFromFile(m_assetsPath + L"peppermint_powerplant_4k.hdr", true);
	m_GraceCathedralTexture = commandList->LoadTextureFromFile(m_assetsPath + L"shanghai_bund_8k.hdr", true);

	auto cubeMapDesc = m_GraceCathedralTexture->GetD3D12ResourceDesc();
	cubeMapDesc.Width = cubeMapDesc.Height = 1920;
	cubeMapDesc.DepthOrArraySize = 6;
	cubeMapDesc.MipLevels = 0;

	m_GraceCathedralCubemap = m_device->CreateTexture(cubeMapDesc);
	m_GraceCathedralCubemap->SetName(L"Grace Cathedral Cubemap");

	/*
	 * convert 2D panorama to 3D cubemap
	 */
	commandList->PanoToCubeMap(m_GraceCathedralCubemap, m_GraceCathedralTexture);

	/*
	 * Start loading resources while rest of resources are created
	 */
	commandQueue.ExecuteCommandList(commandList);

	D3D12_SHADER_RESOURCE_VIEW_DESC cubeMapSRVDesc = {};
	cubeMapSRVDesc.Format = cubeMapDesc.Format;
	cubeMapSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	cubeMapSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	cubeMapSRVDesc.TextureCube.MipLevels = (UINT)-1;

	m_GraceCathedralCubemapSRV = m_device->CreateShaderResourceView(m_GraceCathedralCubemap, &cubeMapSRVDesc);

	/*
	 * Create HDR intermediate render target;
	 */
	DXGI_FORMAT HDRFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

	/*
	 * Create off-screen render target with single color buffer and a depth buffer;
	 */
	auto colorDesc = CD3DX12_RESOURCE_DESC::Tex2D(HDRFormat, m_width, m_height);
	colorDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE colorClearValue;
	colorClearValue.Format = colorDesc.Format;
	colorClearValue.Color[0] = 0.4f;
	colorClearValue.Color[1] = 0.6f;
	colorClearValue.Color[2] = 0.9f;
	colorClearValue.Color[3] = 1.0f;

	m_HDRTexture = m_device->CreateTexture(colorDesc, &colorClearValue);
	m_HDRTexture->SetName(L"HDR Texture");

	/*
	 * Create depth buffer for HDR render target
	 */
	auto depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthBufferFormat, m_width, m_height);
	depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthClearValue;
	depthClearValue.Format = depthDesc.Format;
	depthClearValue.DepthStencil = {1.0f, 0};

	auto depthTexture = m_device->CreateTexture(depthDesc, &depthClearValue);
	depthTexture->SetName(L"Depth Render Target");

	/*
	 * Attach HDR texture to HDR render target
	 */
	m_renderTarget.AttachTexture(AttachmentPoint::Color0, m_HDRTexture);
	m_renderTarget.AttachTexture(AttachmentPoint::DepthStencil, depthTexture);

	/*
	 * Create root signature and PSO for skybox shaders
	 * TODO: move function
	 */
	{
		/*
		 * Load SkyBox
		 */
		std::wstring skybox_vs_assetsPath = m_assetsPath + L"CEGEHDRSkyBoxVertexShader.cso";
		Microsoft::WRL::ComPtr<ID3DBlob> skyboxVertexShader;
		ThrowIfFailed(D3DReadFileToBlob(skybox_vs_assetsPath.c_str(), &skyboxVertexShader));

		std::wstring skybox_ps_assetsPath = m_assetsPath + L"CEGEHDRSkyBoxPixelShader.cso";
		Microsoft::WRL::ComPtr<ID3DBlob> skyboxPixelShader;
		ThrowIfFailed(D3DReadFileToBlob(skybox_ps_assetsPath.c_str(), &skyboxPixelShader));

		/*
		 * Setup input layout for skybox vertex shader
		 */
		D3D12_INPUT_ELEMENT_DESC inputLayout[1] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			}
		};

		/*
		 * Allow input layout and deny unnecessary access to certain pipeline stages
		 */
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters[1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		                                               D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		                                               D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		                                               D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(2, rootParameters, 1, &linearClampSampler, rootSignatureFlags);

		m_SkyboxSignature = m_device->CreateRootSignature(rootSignatureDescription.Desc_1_1);

		/*
		 * Skybox pipeline state
		 */
		struct SkyBoxPipelineState {
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} skyBoxPipelineStateStream;

		skyBoxPipelineStateStream.pRootSignature = m_SkyboxSignature->GetD3D12RootSignature().Get();
		skyBoxPipelineStateStream.InputLayout = {inputLayout, 1};
		skyBoxPipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		skyBoxPipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(skyboxVertexShader.Get());
		skyBoxPipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(skyboxPixelShader.Get());
		skyBoxPipelineStateStream.RTVFormats = m_renderTarget.GetRenderTargetFormats();

		m_SkyboxPipelineState = m_device->CreatePipelineStateObject(skyBoxPipelineStateStream);
	}

	/*
	 * Create root signature for HDR pipeline
	 */
	{
		Microsoft::WRL::ComPtr<ID3DBlob> hdrVertexShader;
		std::wstring skybox_vs_assetsPath = m_assetsPath + L"CEGEHDRVertexShader.cso";
		ThrowIfFailed(D3DReadFileToBlob(skybox_vs_assetsPath.c_str(), &hdrVertexShader));

		Microsoft::WRL::ComPtr<ID3DBlob> hdrPixelShader;
		std::wstring skybox_ps_assetsPath = m_assetsPath + L"CEGEHDRPixelShader.cso";
		ThrowIfFailed(D3DReadFileToBlob(skybox_ps_assetsPath.c_str(), &hdrPixelShader));

		/*
		 * Allow input layout and deny unnecessary access to certain pipeline stages
		 */
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
		CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters];
		rootParameters[RootParameters::MatricesCB].InitAsConstantBufferView(
			0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters[RootParameters::MaterialCB].InitAsConstantBufferView(
			0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[RootParameters::LightPropertiesCB].InitAsConstants(
			sizeof(LightProperties) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[RootParameters::PointLights].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
		                                                                     D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[RootParameters::SpotLights].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
		                                                                    D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[RootParameters::Textures].InitAsDescriptorTable(1, &descriptorRange,
		                                                               D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
		CD3DX12_STATIC_SAMPLER_DESC anisotropicSampler(0, D3D12_FILTER_ANISOTROPIC);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(RootParameters::NumRootParameters, rootParameters, 1, &linearRepeatSampler,
		                                  rootSignatureFlags);

		m_HDRRootSignature = m_device->CreateRootSignature(rootSignatureDescription.Desc_1_1);

		// Setup the HDR pipeline state.
		struct HDRPipelineStateStream {
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} hdrPipelineStateStream;

		hdrPipelineStateStream.pRootSignature = m_HDRRootSignature->GetD3D12RootSignature().Get();
		hdrPipelineStateStream.InputLayout = CEVertexPositionNormalTangentBitangentTexture::InputLayout;
		hdrPipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		hdrPipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(hdrVertexShader.Get());
		hdrPipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(hdrPixelShader.Get());
		hdrPipelineStateStream.DSVFormat = m_renderTarget.GetDepthStencilFormat();
		hdrPipelineStateStream.RTVFormats = m_renderTarget.GetRenderTargetFormats();

		m_HDRPipelineState = m_device->CreatePipelineStateObject(hdrPipelineStateStream);

		// The unlit pipeline state is similar to the HDR pipeline state except a different pixel shader.
		Microsoft::WRL::ComPtr<ID3DBlob> hdrUnlitPixelShader;
		std::wstring skybox_unlit_ps_assetsPath = m_assetsPath + L"CEGEHDRUnlitPixelShader.cso";
		ThrowIfFailed(D3DReadFileToBlob(skybox_unlit_ps_assetsPath.c_str(), &hdrUnlitPixelShader));

		hdrPipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(hdrUnlitPixelShader.Get());

		m_UnlitPipelineState = m_device->CreatePipelineStateObject(hdrPipelineStateStream);
	}

	/*
	 * Create SDR Root Signature
	 */
	{
		CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsConstants(sizeof(TonemapParameters) / 4, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_STATIC_SAMPLER_DESC linearClampsSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		                                                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		                                                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		                                                D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(2, rootParameters, 1, &linearClampsSampler);

		m_SDRRootSignature = m_device->CreateRootSignature(rootSignatureDescription.Desc_1_1);

		/*
		 * Create SDR PSO
		 */
		Microsoft::WRL::ComPtr<ID3DBlob> sdrVertexShader;
		std::wstring skybox_vs_assetsPath = m_assetsPath + L"CEGEHDRSDRVertexShader.cso";
		ThrowIfFailed(D3DReadFileToBlob(skybox_vs_assetsPath.c_str(), &sdrVertexShader));

		Microsoft::WRL::ComPtr<ID3DBlob> sdrPixelShader;
		std::wstring skybox_ps_assetsPath = m_assetsPath + L"CEGEHDRSDRPixelShader.cso";
		ThrowIfFailed(D3DReadFileToBlob(skybox_ps_assetsPath.c_str(), &sdrPixelShader));

		CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

		struct SDRPipelineStateStream {
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} sdrPipelineStateStream;

		sdrPipelineStateStream.pRootSignature = m_SDRRootSignature->GetD3D12RootSignature().Get();
		sdrPipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		sdrPipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(sdrVertexShader.Get());
		sdrPipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(sdrPixelShader.Get());
		sdrPipelineStateStream.Rasterizer = rasterizerDesc;
		sdrPipelineStateStream.RTVFormats = m_swapChain->GetRenderTarget().GetRenderTargetFormats();

		m_SDRPipelineState = m_device->CreatePipelineStateObject(sdrPipelineStateStream);
	}


	/*
	 * Make sure command queue is finished loading resources before rendering first frame
	 */
	commandQueue.Flush();

	return true;
}

void CEHDRPlayground::UnloadContent() {
	m_sphere.reset();
	m_cone.reset();
	m_torus.reset();
	m_plane.reset();
	m_cylinder.reset();
	m_skybox.reset();

	m_defaultTexture.reset();
	m_directXTexture.reset();
	m_earthTexture.reset();
	m_monaLisaTexture.reset();
	m_GraceCathedralTexture.reset();
	m_GraceCathedralCubemap.reset();

	m_SkyboxSignature.reset();
	m_HDRRootSignature.reset();
	m_SDRRootSignature.reset();
	m_SkyboxPipelineState.reset();
	m_HDRPipelineState.reset();
	m_SDRPipelineState.reset();
	m_UnlitPipelineState.reset();

	m_renderTarget.Reset();

	m_gui.reset();
	m_swapChain.reset();
	m_device.reset();
}

void CEHDRPlayground::OnUpdate(UpdateEventArgs& e) {
	DisplayDebugFPSOnUpdate(e);

	m_swapChain->WaitForSwapChain();
	// Update the camera.
	float speedMultipler = (m_shift ? 16.0f : 4.0f);

	XMVECTOR cameraTranslate = XMVectorSet(m_right - m_left, 0.0f, m_forward - m_backward, 1.0f) * speedMultipler *
		static_cast<float>(e.DeltaTime);
	DirectX::XMVECTOR cameraPan =
		DirectX::XMVectorSet(0.0f, m_up - m_down, 0.0f, 1.0f) * speedMultipler * static_cast<float>(e.DeltaTime);
	m_camera.Translate(cameraTranslate, GraphicsEngine::Space::Local);
	m_camera.Translate(cameraPan, GraphicsEngine::Space::Local);

	DirectX::XMVECTOR cameraRotation =
		DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(m_pitch),
		                                          DirectX::XMConvertToRadians(m_yaw), 0.0f);
	m_camera.set_Rotation(cameraRotation);

	DirectX::XMMATRIX viewMatrix = m_camera.get_ViewMatrix();

	const int numPointLights = 4;
	const int numSpotLights = 4;

	static const DirectX::XMVECTORF32 LightColors[] = {
		DirectX::Colors::White, DirectX::Colors::Orange, DirectX::Colors::Yellow, DirectX::Colors::Green,
		DirectX::Colors::Blue, DirectX::Colors::Indigo, DirectX::Colors::Violet, DirectX::Colors::White
	};

	static float lightAnimTime = 0.0f;
	if (m_animateLights) {
		lightAnimTime += static_cast<float>(e.DeltaTime) * 0.5f * DirectX::XM_PI;
	}

	const float radius = 8.0f;
	const float offset = 2.0f * DirectX::XM_PI / numPointLights;
	const float offset2 = offset + (offset / 2.0f);

	// Setup the light buffers.
	m_pointLights.resize(numPointLights);
	for (int i = 0; i < numPointLights; ++i) {
		Lighting::HDRPointLight& l = m_pointLights[i];

		l.PositionWS = {
			static_cast<float>(std::sin(lightAnimTime + offset * i)) * radius, 9.0f,
			static_cast<float>(std::cos(lightAnimTime + offset * i)) * radius, 1.0f
		};
		DirectX::XMVECTOR positionWS = XMLoadFloat4(&l.PositionWS);
		DirectX::XMVECTOR positionVS = XMVector3TransformCoord(positionWS, viewMatrix);
		XMStoreFloat4(&l.PositionVS, positionVS);

		l.Color = DirectX::XMFLOAT4(LightColors[i]);
		l.Intensity = 1.0f;
		l.Attenuation = 0.0f;
	}

	m_spotLights.resize(numSpotLights);
	for (int i = 0; i < numSpotLights; ++i) {
		Lighting::HDRSpotLight& l = m_spotLights[i];

		l.PositionWS = {
			static_cast<float>(std::sin(lightAnimTime + offset * i + offset2)) * radius, 9.0f,
			static_cast<float>(std::cos(lightAnimTime + offset * i + offset2)) * radius, 1.0f
		};
		DirectX::XMVECTOR positionWS = XMLoadFloat4(&l.PositionWS);
		DirectX::XMVECTOR positionVS = XMVector3TransformCoord(positionWS, viewMatrix);
		XMStoreFloat4(&l.PositionVS, positionVS);

		DirectX::XMVECTOR directionWS = DirectX::XMVector3Normalize(
			DirectX::XMVectorSetW(DirectX::XMVectorNegate(positionWS), 0));
		DirectX::XMVECTOR directionVS = DirectX::XMVector3Normalize(XMVector3TransformNormal(directionWS, viewMatrix));
		XMStoreFloat4(&l.DirectionWS, directionWS);
		XMStoreFloat4(&l.DirectionVS, directionVS);

		l.Color = DirectX::XMFLOAT4(LightColors[numPointLights + i]);
		l.Intensity = 1.0f;
		l.SpotAngle = DirectX::XMConvertToRadians(45.0f);
		l.Attenuation = 0.0f;
	}

	OnRender();
}

void CEHDRPlayground::OnRender() {
	auto& commandQueue = m_device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandList = commandQueue.GetCommandList();

	/*
	 * Create scene visitor
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

	commandList->SetRenderTarget(m_renderTarget);
	commandList->SetViewport(m_renderTarget.GetViewPort());
	commandList->SetScissorRect(m_scissorRect);

	/*
	 * Render skybox
	 */
	{
		auto viewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationQuaternion(m_camera.get_Rotation()));
		auto projMatrix = m_camera.get_ProjectionMatrix();
		auto viewProjMatrix = viewMatrix * projMatrix;

		commandList->SetPipelineState(m_SkyboxPipelineState);
		commandList->SetGraphicsRootSignature(m_SkyboxSignature);

		commandList->SetGraphics32BitConstants(0, viewProjMatrix);

		commandList->SetShaderResourceView(1, 0, m_GraceCathedralCubemapSRV,
		                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_skybox->Accept(visitor);
	}

	commandList->SetPipelineState(m_HDRPipelineState);
	commandList->SetGraphicsRootSignature(m_HDRRootSignature);

	// Upload lights
	LightProperties lightProps;
	lightProps.NumPointLights = static_cast<uint32_t>(m_pointLights.size());
	lightProps.NumSpotLights = static_cast<uint32_t>(m_spotLights.size());

	commandList->SetGraphics32BitConstants(RootParameters::LightPropertiesCB, lightProps);
	commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::PointLights, m_pointLights);
	commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::SpotLights, m_spotLights);

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

	m_cube->Accept(visitor);

	// Draw a torus
	translationMatrix = XMMatrixTranslation(4.0f, 0.6f, -4.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
	scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::Ruby);
	commandList->SetShaderResourceView(RootParameters::Textures, 0, m_defaultTexture,
	                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	m_torus->Accept(visitor);

	// Draw a cylinder
	translationMatrix = XMMatrixTranslation(-4.0f, 4.0f, 4.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
	scaleMatrix = XMMatrixScaling(4.0f, 8.0f, 4.0f);
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	
	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);
	
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::Gold);
	commandList->SetShaderResourceView(RootParameters::Textures, 0, m_defaultTexture,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	m_cylinder->Accept(visitor);

	// Floor plane.
	float scalePlane = 20.0f;
	float translateOffset = scalePlane / 2.0f;

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	rotationMatrix = XMMatrixIdentity();
	scaleMatrix = XMMatrixScaling(scalePlane, 1.0f, scalePlane);
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::BlackRubber);
	commandList->SetShaderResourceView(RootParameters::Textures, 0, m_directXTexture,
	                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	m_plane->Accept(visitor);

	// Back wall
	translationMatrix = XMMatrixTranslation(0, translateOffset, translateOffset);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

	m_plane->Accept(visitor);

	// Ceiling plane
	translationMatrix = XMMatrixTranslation(0, translateOffset * 2.0f, 0);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(180));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

	m_plane->Accept(visitor);

	// Front wall
	translationMatrix = XMMatrixTranslation(0, translateOffset, -translateOffset);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(90));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

	m_plane->Accept(visitor);

	// Left wall
	translationMatrix = XMMatrixTranslation(-translateOffset, translateOffset, 0);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) * XMMatrixRotationY(XMConvertToRadians(-90));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);
	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::BlackRubber);
	commandList->SetShaderResourceView(RootParameters::Textures, 0, m_defaultTexture,
	                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	m_plane->Accept(visitor);

	// Right wall
	translationMatrix = XMMatrixTranslation(translateOffset, translateOffset, 0);
	rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) * XMMatrixRotationY(XMConvertToRadians(90));
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
	commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, CEMaterial::BlackRubber);

	m_plane->Accept(visitor);

	// Draw shapes to visualize the position of the lights in the scene.
	commandList->SetPipelineState(m_UnlitPipelineState);

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

		m_cone->Accept(visitor);
	}

	// Perform HDR -> SDR tonemapping directly to the SwapChain's render target.
	commandList->SetRenderTarget(m_swapChain->GetRenderTarget());
	commandList->SetViewport(m_swapChain->GetRenderTarget().GetViewPort());
	commandList->SetPipelineState(m_SDRPipelineState);
	commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootSignature(m_SDRRootSignature);
	commandList->SetGraphics32BitConstants(0, g_TonemapParameters);
	commandList->SetShaderResourceView(1, 0, m_HDRTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	commandList->Draw(3);

	// Render GUI.
	OnGUI(commandList, m_swapChain->GetRenderTarget());

	commandQueue.ExecuteCommandList(commandList);

	// Present
	m_swapChain->Present();
}

static bool g_allowFullscreenToggle = true;
static bool g_allowMouseVisibleToggle = true;


void CEHDRPlayground::OnKeyPressed(KeyEventArgs& e) {
 
	if (!ImGui::GetIO().WantCaptureKeyboard) {
		switch (e.Key) {
		case KeyCode::Escape:
			CEGame::Get().Stop();
			break;
		case KeyCode::Enter:
			if (e.Alt) {
			case KeyCode::F11:
				if (g_allowFullscreenToggle) {
					m_window->ToggleFullscreen(); // Defer window resizing until OnUpdate();
					// Prevent the key repeat to cause multiple resizes.
					g_allowFullscreenToggle = false;
				}
				break;
			}
		case KeyCode::M: {
			if (e.Control) {
				if (g_allowMouseVisibleToggle) {
					m_window->ToggleCursor();
					g_allowMouseVisibleToggle = false;
				}
				break;
			}
		}
		case KeyCode::V:
			m_swapChain->ToggleVSync();
			break;
		case KeyCode::R:
			// Reset camera transform
			m_camera.set_Translation(m_pAlignedCameraData->m_initialCamPos);
			m_camera.set_Rotation(m_pAlignedCameraData->m_initialCamRot);
			m_camera.Set_FoV(m_pAlignedCameraData->m_InitialFov);
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

void CEHDRPlayground::OnKeyReleased(KeyEventArgs& e) {
	if (!ImGui::GetIO().WantCaptureKeyboard) {
		switch (e.Key) {
		case KeyCode::Enter:
			if (e.Alt) {
			case KeyCode::F11:
				g_allowFullscreenToggle = true;
			}
			break;
		case KeyCode::M: {
			if (e.Control) {
				g_allowMouseVisibleToggle = true;

			}
			break;
		}
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
}

void CEHDRPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
	const float mouseSpeed = 0.1f;
	if (!ImGui::GetIO().WantCaptureMouse) {
		if (e.LeftButton) {
			m_pitch -= e.RelY * mouseSpeed;

			m_pitch = std::clamp(m_pitch, -90.0f, 90.0f);

			m_yaw -= e.RelX * mouseSpeed;
		}
	}
}

void CEHDRPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
	if (!ImGui::GetIO().WantCaptureMouse) {
		auto fov = m_camera.get_FoV();

		fov -= e.WheelDelta;
		fov = std::clamp(fov, 12.0f, 90.0f);

		m_camera.Set_FoV(fov);

		m_logger->info("FoV: {:.7}", fov);
	}
}

void CEHDRPlayground::OnResize(ResizeEventArgs& e) {
	m_width = std::max(1, e.Width);
	m_height = std::max(1, e.Height);

	float fov = m_camera.get_FoV();
	float aspectRatio = m_width / (float)m_height;
	m_camera.set_Projection(fov, aspectRatio, 0.1f, 100.0f);

	RescaleHDRRenderTarget(m_RenderScale);

	m_swapChain->Resize(m_width, m_height);
}

void CEHDRPlayground::OnGUI(const std::shared_ptr<GraphicsEngine::Direct3D::CECommandList>& commandList,
                            const GraphicsEngine::Direct3D::CERenderTarget& renderTarget) {
	static bool toneMappingVisible = false;

	m_gui->NewFrame();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit", "Esc")) {
				CEGame::Get().Stop();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			ImGui::MenuItem("Tonemapping", nullptr, &toneMappingVisible);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Options")) {
			bool vSync = m_swapChain->GetVSync();
			if (ImGui::MenuItem("V-Sync", "V", &vSync)) {
				m_swapChain->SetVSync(vSync);
			}

			bool fullscreen = m_window->IsFullscreen();
			if (ImGui::MenuItem("Full screen", "Alt+Enter", &fullscreen)) {
				m_window->SetFullscreen(fullscreen);
			}

			bool cursorVisibility = m_window->IsCursorVisible();
			if (ImGui::MenuItem("Show Cursor", "Ctrl+M", &cursorVisibility)) {
				m_window->SetCursor(cursorVisibility);
			}

			//TODO: Implement
			bool rayTracingSupported = m_device->GetRayTracingTier();
			if (rayTracingSupported != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
				bool rayTracingEnabled = false;
				auto rtxDesc = m_device->GetCurrentRayTracingSupportName();
				std::string rtxDescription(rtxDesc.begin(), rtxDesc.end());
				auto rdesc = "RTX " + rtxDescription;
				if (ImGui::MenuItem(rdesc.c_str(), "Ctrl+X", &rayTracingEnabled)) {
				}
			}

			ImGui::EndMenu();
		}

		char buffer[256];
		{
			// Output a slider to scale the resolution of the HDR render target.
			float renderScale = m_RenderScale;
			ImGui::PushItemWidth(300.0f);
			ImGui::SliderFloat("Resolution Scale", &renderScale, 0.1f, 2.0f);
			// Using Ctrl+Click on the slider, the user can set values outside of the
			// specified range. Make sure to clamp to a sane range to avoid creating
			// giant render targets.
			renderScale = std::clamp(renderScale, 0.0f, 2.0f);

			// Output current resolution of render target.
			auto size = m_renderTarget.GetSize();
			ImGui::SameLine();
			sprintf_s(buffer, _countof(buffer), "(%ux%u)", size.x, size.y);
			ImGui::Text(buffer);

			// Resize HDR render target if the scale changed.
			if (renderScale != m_RenderScale) {
				m_RenderScale = renderScale;
				RescaleHDRRenderTarget(m_RenderScale);
			}
		}

		{
			sprintf_s(buffer, _countof(buffer), "FPS: %.2f (%.2f ms)  ", g_FPS, 1.0 / g_FPS * 1000.0);
			auto fpsTextSize = ImGui::CalcTextSize(buffer);
			ImGui::SameLine(ImGui::GetWindowWidth() - fpsTextSize.x);
			ImGui::Text(buffer);
		}

		ImGui::EndMainMenuBar();
	}

	if (toneMappingVisible) {
		ImGui::Begin("Tonemapping", &toneMappingVisible);
		{
			ImGui::TextWrapped("Use the Exposure slider to adjust the overall exposure of the HDR scene.");
			ImGui::SliderFloat("Exposure", &g_TonemapParameters.Exposure, -10.0f, 10.0f);
			ImGui::SameLine();
			ShowHelpMarker("Adjust the overall exposure of the HDR scene.");
			ImGui::SliderFloat("Gamma", &g_TonemapParameters.Gamma, 0.01f, 5.0f);
			ImGui::SameLine();
			ShowHelpMarker("Adjust the Gamma of the output image.");

			const char* toneMappingMethods[] = {"Linear", "Reinhard", "Reinhard Squared", "ACES Filmic"};

			ImGui::Combo("Tonemapping Methods", (int*)(&g_TonemapParameters.TonemapMethod), toneMappingMethods, 4);

			switch (g_TonemapParameters.TonemapMethod) {
			case TonemapMethod::TM_Linear:
				ImGui::PlotLines("Linear Tonemapping", &LinearTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f,
				                 1.0f, ImVec2(0, 250));
				ImGui::SliderFloat("Max Brightness", &g_TonemapParameters.MaxLuminance, 1.0f, HDR_MAX);
				ImGui::SameLine();
				ShowHelpMarker("Linearly scale the HDR image by the maximum brightness.");
				break;
			case TonemapMethod::TM_Reinhard:
				ImGui::PlotLines("Reinhard Tonemapping", &ReinhardTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr,
				                 0.0f, 1.0f, ImVec2(0, 250));
				ImGui::SliderFloat("Reinhard Constant", &g_TonemapParameters.K, 0.01f, 10.0f);
				ImGui::SameLine();
				ShowHelpMarker("The Reinhard constant is used in the denominator.");
				break;
			case TonemapMethod::TM_ReinhardSq:
				ImGui::PlotLines("Reinhard Squared Tonemapping", &ReinhardSqrTonemappingPlot, nullptr, VALUES_COUNT, 0,
				                 nullptr, 0.0f, 1.0f, ImVec2(0, 250));
				ImGui::SliderFloat("Reinhard Constant", &g_TonemapParameters.K, 0.01f, 10.0f);
				ImGui::SameLine();
				ShowHelpMarker("The Reinhard constant is used in the denominator.");
				break;
			case TonemapMethod::TM_ACESFilmic:
				ImGui::PlotLines("ACES Filmic Tonemapping", &ACESFilmicTonemappingPlot, nullptr, VALUES_COUNT, 0,
				                 nullptr, 0.0f, 1.0f, ImVec2(0, 250));
				ImGui::SliderFloat("Shoulder Strength", &g_TonemapParameters.A, 0.01f, 5.0f);
				ImGui::SliderFloat("Linear Strength", &g_TonemapParameters.B, 0.0f, 100.0f);
				ImGui::SliderFloat("Linear Angle", &g_TonemapParameters.C, 0.0f, 1.0f);
				ImGui::SliderFloat("Toe Strength", &g_TonemapParameters.D, 0.01f, 1.0f);
				ImGui::SliderFloat("Toe Numerator", &g_TonemapParameters.E, 0.0f, 10.0f);
				ImGui::SliderFloat("Toe Denominator", &g_TonemapParameters.F, 1.0f, 10.0f);
				ImGui::SliderFloat("Linear White", &g_TonemapParameters.LinearWhite, 1.0f, 120.0f);
				break;
			default:
				break;
			}
		}

		if (ImGui::Button("Reset to Defaults")) {
			TonemapMethod method = g_TonemapParameters.TonemapMethod;
			g_TonemapParameters = TonemapParameters();
			g_TonemapParameters.TonemapMethod = method;
		}

		ImGui::End();
	}
	m_gui->Render(commandList, renderTarget);
}

void CEHDRPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
	m_gui->SetScaling(e.DPIScale);
}

void CEHDRPlayground::RescaleHDRRenderTarget(float scale) {
	uint32_t width = static_cast<uint32_t>(m_width * scale);
	uint32_t height = static_cast<uint32_t>(m_height * scale);

	width = std::clamp<uint32_t>(width, 1, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);
	height = std::clamp<uint32_t>(height, 1, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);

	m_renderTarget.Resize(width, height);
}
