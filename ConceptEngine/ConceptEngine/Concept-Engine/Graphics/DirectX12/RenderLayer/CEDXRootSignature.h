#pragma once

#include "../Libraries/d3dx12.h"

#include "CEDXDeviceElement.h"
#include "CEDXShader.h"
#include "../../../Core/Common/CERefCountedObject.h"
#include "../../../Core/Containers/CEArray.h"
#include "../../../Utilities/CEStringUtilities.h"
#include "../../../Math/CEMath.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	enum class CERootSignatureType {
		Unknown = 0,
		Graphics = 1,
		Compute = 2,
		RayTracingGlobal = 3,
		RayTracingLocal = 4
	};

	enum CEStaticSamplers : uint8 {
		PointWrap = 0,
		PointClamp = 1,
		LinearWrap = 2,
		LinearClamp = 3,
		AnisotropicWrap = 4,
		AnisotropicClamp = 5,
		Shadow = 6,
		All = 7,
	};

	struct CEDXRootSignatureResourceCount {
		bool IsCompatible(const CEDXRootSignatureResourceCount& another) const;
		CERootSignatureType Type = CERootSignatureType::Unknown;
		CEShaderResourceCount ResourceCounts[ShaderVisibility_Count];
		bool AllowInputAssembler = false;
	};

	struct CEDXRootSignatureDescHelper {
	public:
		CEDXRootSignatureDescHelper(const CEDXRootSignatureResourceCount& rootSignatureInfo);
		~CEDXRootSignatureDescHelper() = default;

		const D3D12_ROOT_SIGNATURE_DESC& GetDesc() const {
			return Desc;
		}

	private:
		static void InitDescriptorRange(
			D3D12_DESCRIPTOR_RANGE& range,
			D3D12_DESCRIPTOR_RANGE_TYPE type,
			uint32 numDescriptors,
			uint32 baseShaderRegister,
			uint32 registerSpace
		);
		static void InitDescriptorTable(
			D3D12_ROOT_PARAMETER& parameter,
			D3D12_SHADER_VISIBILITY shaderVisibility,
			const D3D12_DESCRIPTOR_RANGE* descriptorRanges,
			uint32 numDescriptorRanges
		);

		static void Init32BitConstantRange(
			D3D12_ROOT_PARAMETER& parameter,
			D3D12_SHADER_VISIBILITY shaderVisibility,
			uint32 num32BitConstants,
			uint32 shaderRegister,
			uint32 registerSpace
		);

		D3D12_ROOT_SIGNATURE_DESC Desc;
		D3D12_ROOT_PARAMETER Parameters[D3D12_MAX_ROOT_PARAMETERS];
		D3D12_DESCRIPTOR_RANGE DescriptorRanges[D3D12_MAX_DESCRIPTOR_RANGES];
		uint32 NumDescriptorRanges = 0;
	};

	class CEDXRootSignature : public CEDXDeviceElement, public Core::Common::CERefCountedObject {
	public:
		CEDXRootSignature(CEDXDevice* device);
		~CEDXRootSignature() = default;

		bool Create(const CEDXRootSignatureResourceCount& rootSignatureInfo);
		bool Create(const D3D12_ROOT_SIGNATURE_DESC& desc);
		bool Create(const void* blobWithRootSignature, uint64 blobLengthInBytes);

		int32 GetRootParameterIndex(CEShaderVisibility visibility, CEResourceType type) const {
			return RootParameterMap[visibility][type];
		}

		int32 Get32BitContantsIndex() const {
			return ConstantRootParameterIndex;
		}

		void SetName(const std::string& name) {
			std::wstring wName = ConvertToWString(name);
			RootSignature->SetName(wName.c_str());
		}

		ID3D12RootSignature* GetRootSignature() const {
			return RootSignature.Get();
		}

		ID3D12RootSignature* const* GetAddressOfRootSignature() const {
			return RootSignature.GetAddressOf();
		}

		static bool Serialize(const D3D12_ROOT_SIGNATURE_DESC& desc, ID3DBlob** blob);

	private:
		void CreateRootParameterMap(const D3D12_ROOT_SIGNATURE_DESC& desc);
		bool InternalCreate(const void* blobWithRootSignature, uint64 blobLengthInBytes);

		Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature;
		int32 RootParameterMap[ShaderVisibility_Count][ResourceType_Count];

		int32 ConstantRootParameterIndex;
	};

	class CEDXRootSignatureCache : public CEDXDeviceElement {
	public:
		CEDXRootSignatureCache(CEDXDevice* device);
		~CEDXRootSignatureCache();

		bool Create();
		void ReleaseAll();

		CEDXRootSignature* GetRootSignature(const CEDXRootSignatureResourceCount& resourceCount);

		static CEDXRootSignatureCache& Get();

	private:
		CEDXRootSignature* CreateRootSignature(const CEDXRootSignatureResourceCount& resourceCount);

		Core::Containers::CEArray<Core::Common::CERef<CEDXRootSignature>> RootSignatures;
		Core::Containers::CEArray<CEDXRootSignatureResourceCount> ResourceCounts;

		static CEDXRootSignatureCache* Instance;
	};
}
