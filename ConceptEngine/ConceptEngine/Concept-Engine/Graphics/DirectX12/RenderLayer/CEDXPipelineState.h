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
	public:
		CEDXInputLayoutState(CEDXDevice* device, const CEInputLayoutStateCreateInfo& createInfo) : CEInputLayoutState(),
			CEDXDeviceElement(device), SemanticNames(), ElementDesc(), Desc() {
			SemanticNames.Reserve(createInfo.Elements.Size());
			for (const CEInputElement& element : createInfo.Elements) {
				D3D12_INPUT_ELEMENT_DESC dxElement;
				dxElement.SemanticName = SemanticNames.EmplaceBack(element.Semantic).c_str();
				dxElement.SemanticIndex = element.SemanticIndex;
				dxElement.Format = ConvertFormat(element.Format);
				dxElement.InputSlot = element.InputSlot;
				dxElement.InputSlotClass = ConvertInputClassification(element.InputClassification);
				dxElement.InstanceDataStepRate = element.InstanceStepRate;
				ElementDesc.EmplaceBack(dxElement);
			}

			Desc.NumElements = GetElementCount();
			Desc.pInputElementDescs = GetElementData();
		}

		virtual bool IsValid() const override {
			return true;
		}

		const D3D12_INPUT_ELEMENT_DESC* GetElementData() const {
			return ElementDesc.Data();
		}

		uint32 GetElementCount() const {
			return ElementDesc.Size();
		}

		const D3D12_INPUT_LAYOUT_DESC& GetDesc() const {
			return Desc;
		}

	private:
		CEArray<std::string> SemanticNames;
		CEArray<D3D12_INPUT_ELEMENT_DESC> ElementDesc;
		D3D12_INPUT_LAYOUT_DESC Desc;
	};

	class CEDXDepthStencilState : public Main::RenderLayer::CEDepthStencilState, public CEDXDeviceElement {
	public:
		CEDXDepthStencilState(CEDXDevice* device, const D3D12_DEPTH_STENCIL_DESC& desc) : CEDepthStencilState(),
			CEDXDeviceElement(device), Desc(desc) {

		}

		virtual bool IsValid() const override {
			return true;
		}

		const D3D12_DEPTH_STENCIL_DESC& GetDesc() const {
			return Desc;
		}

	private:
		D3D12_DEPTH_STENCIL_DESC Desc;
	};

	class CEDXRasterizerState : public Main::RenderLayer::CERasterizerState, public CEDXDeviceElement {
	public:
		CEDXRasterizerState(CEDXDevice* device, const D3D12_RASTERIZER_DESC& desc): CERasterizerState(),
			CEDXDeviceElement(device), Desc(desc) {

		}

		virtual bool IsValid() const override {
			return true;
		}

		const D3D12_RASTERIZER_DESC& GetDesc() const {
			return Desc;
		}

	private:
		D3D12_RASTERIZER_DESC Desc;
	};

	class CEDXBlendState : public Main::RenderLayer::CEBlendState, public CEDXDeviceElement {
	public:
		CEDXBlendState(CEDXDevice* device, const D3D12_BLEND_DESC& desc): CEBlendState(), CEDXDeviceElement(device),
		                                                                  Desc(desc) {

		}

		virtual bool IsValid() const override {
			return true;
		}

		const D3D12_BLEND_DESC& GetDesc() const {
			return Desc;
		}

	private:
		D3D12_BLEND_DESC Desc;
	};

	class CEDXGraphicsPipelineState : public Main::RenderLayer::CEGraphicsPipelineState, public CEDXDeviceElement {
	public:
		CEDXGraphicsPipelineState(CEDXDevice* device);
		~CEDXGraphicsPipelineState() = default;

		bool Create(const CEGraphicsPipelineStateCreateInfo& createInfo);

		virtual void SetName(const std::string& name) override {
			CEResource::SetName(name);

			std::wstring wName = ConvertToWString(name);
			PipelineState->SetName(wName.c_str());
		}

		virtual void* GetNativeResource() const override {
			return reinterpret_cast<void*>(PipelineState.Get());
		}

		virtual bool IsValid() const override {
			return PipelineState != nullptr && RootSignature != nullptr;
		}

		ID3D12PipelineState* GetPipeline() const {
			return PipelineState.Get();
		}

		CEDXRootSignature* GetRootSignature() const {
			return RootSignature.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;
		CERef<CEDXRootSignature> RootSignature;
	};

	class CEDXComputePipelineState : public Main::RenderLayer::CEComputePipelineState, public CEDXDeviceElement {
	public:
		CEDXComputePipelineState(CEDXDevice* device, const CERef<CEDXComputeShader>& shader);
		~CEDXComputePipelineState() = default;

		bool Create();

		virtual void SetName(const std::string& name) override {
			CEResource::SetName(name);

			std::wstring wName = ConvertToWString(name);
			PipelineState->SetName(wName.c_str());
		}

		virtual void* GetNativeResource() const override {
			return reinterpret_cast<void*>(PipelineState.Get());
		}

		virtual bool IsValid() const override {
			return PipelineState != nullptr && RootSignature != nullptr;
		}

		ID3D12PipelineState* GetPipeline() const {
			return PipelineState.Get();
		}

		CEDXRootSignature* GetRootSignature() const {
			return RootSignature.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;
		CERef<CEDXComputeShader> Shader;
		CERef<CEDXRootSignature> RootSignature;
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

		CERef<CEDXRootSignature> GlobalRootSignature;
		CERef<CEDXRootSignature> RayGenLocalRootSignature;
		CERef<CEDXRootSignature> MissLocalRootSignature;
		CERef<CEDXRootSignature> HitLocalRootSignature;

		std::unordered_map<std::string, CERayTracingShaderIdentifier> ShaderIdentifiers;
	};
}
