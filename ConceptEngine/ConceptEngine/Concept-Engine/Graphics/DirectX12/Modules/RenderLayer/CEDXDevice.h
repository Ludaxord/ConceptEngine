#pragma once
#include <d3d12.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <DXProgrammableCapture.h>

#include "../../../../Core/Common/CETypes.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::RenderLayer {

	void DeviceRemovedHandler(class CEDXDevice* Device);

	class CEDXDevice {
	public:
		CEDXDevice(bool enableDebugLayer, bool enableGPUValidation, bool EnableDRED);
		~CEDXDevice();

		bool Create();
	protected:
	private:
		Microsoft::WRL::ComPtr<IDXGIFactory2> Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> Adapter;
		Microsoft::WRL::ComPtr<ID3D12Device> Device;
		Microsoft::WRL::ComPtr<ID3D12Device5> DXRDevice;

		Microsoft::WRL::ComPtr<IDXGraphicsAnalysis> PIXCaptureInterface;

		D3D_FEATURE_LEVEL MinFeatureLevel = D3D_FEATURE_LEVEL_12_0;
		D3D_FEATURE_LEVEL ActiveFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		D3D12_RAYTRACING_TIER RayTracingTier = D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
		D3D12_SAMPLER_FEEDBACK_TIER SamplerFeedBackTier = D3D12_SAMPLER_FEEDBACK_TIER_NOT_SUPPORTED;
		D3D12_MESH_SHADER_TIER MeshShaderTier = D3D12_MESH_SHADER_TIER_NOT_SUPPORTED;
		D3D12_VARIABLE_SHADING_RATE_TIER VariableShadingRateTier = D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED;
		uint32 VariableShadingRateTileSize = 0;

		HMODULE DXGILib = 0;
		HMODULE D3D12Lib = 0;
		HMODULE PIXLib = 0;

		uint32 AdapterID = 0;

		bool AllowTearing = false;
		bool EnableDebugLayer = false;
		bool EnableGPUValidation = false;
		bool EnableDRED = false;
	};
}
