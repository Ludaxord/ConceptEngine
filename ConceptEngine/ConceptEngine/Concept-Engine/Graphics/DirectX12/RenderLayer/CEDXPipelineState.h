#pragma once
#include <d3d12.h>
#include <unordered_map>

#include "CEDXDeviceElement.h"
#include "CEDXRootSignature.h"
#include "CEDXShader.h"

#include "../../Main/RenderLayer/CEPipelineState.h"

#include "../../../Utilities/CEStringUtilities.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXInputLayoutState : public Main::RenderLayer::CEInputLayoutState, public CEDXDeviceElement {

	};

	class CEDXDepthStencilState : public Main::RenderLayer::CEDepthStencilState, public CEDXDeviceElement {

	};

	class CEDXRasterizerState : public Main::RenderLayer::CERasterizerState, public CEDXDeviceElement {

	};

	class CEDXBlendState : public Main::RenderLayer::CEBlendState, public CEDXDeviceElement {

	};

	class CEDXGraphicsPipelineState : public Main::RenderLayer::CEGraphicsPipelineState, public CEDXDeviceElement {

	};

	class CEDXComputePipelineState : public Main::RenderLayer::CEComputePipelineState, public CEDXDeviceElement {
	public:
		CEDXComputePipelineState(CEDXDevice* device, const Core::Common::CERef<CEDXComputeShader>& shader);
		~CEDXComputePipelineState() = default;

		bool Create();
	};

	struct CERayTracingShaderIdentifier {
		char ShaderIdentifier[D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES];
	};

	class CEDXRayTracingPipelineState : public Main::RenderLayer::CERayTracingPipelineState, public CEDXDeviceElement {
	public:
		CEDXRayTracingPipelineState(CEDXDevice* device);
		~CEDXRayTracingPipelineState() = default;

		bool Create(const CERayTracingPipelineStateCreateInfo& createInfo);

		virtual void SetName(const std::string& name) override {
			CEResource::SetName(name);

			std::wstring wName = ConvertToWString(name);
			StateObject->SetName(wName.c_str());
		}

		virtual void* GetNativeResource() const override {
			return reinterpret_cast<void*>(StateObject.Get());
		}

		virtual bool IsValid() const override {
			return StateObject != nullptr;
		}

		void* GetShaderIdentifier(const std::string& exportName);

		ID3D12StateObject* GetStateObject() const {
			return StateObject.Get();
		}

		ID3D12StateObjectProperties* GetStateObjectProperties() const {
			return StateObjectProperties.Get();
		}

		CEDXRootSignature* GetGlobalRootSignature() const {
			return GlobalRootSignature.Get();
		}

		CEDXRootSignature* GetRayGenLocalRootSignature() const {
			return RayGenLocalRootSignature.Get();
		}

		CEDXRootSignature* GetMissLocalRootSignature() const {
			return MissLocalRootSignature.Get();
		}

		CEDXRootSignature* GetHitLocalRootSignature() const {
			return HitLocalRootSignature.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12StateObject> StateObject;
		Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> StateObjectProperties;

		Core::Common::CERef<CEDXRootSignature> GlobalRootSignature;
		Core::Common::CERef<CEDXRootSignature> RayGenLocalRootSignature;
		Core::Common::CERef<CEDXRootSignature> MissLocalRootSignature;
		Core::Common::CERef<CEDXRootSignature> HitLocalRootSignature;

		std::unordered_map<std::string, CERayTracingShaderIdentifier> ShaderIdentifiers;
	};
}
