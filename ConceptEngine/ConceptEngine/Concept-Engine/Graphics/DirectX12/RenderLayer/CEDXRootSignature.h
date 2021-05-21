#pragma once
#include "CEDXDeviceElement.h"
#include "CEDXShader.h"
#include "../../../Core/Common/CERefCountedObject.h"
#include "../../../Core/Containers/CEArray.h"
#include "../../../Utilities/CEStringUtilities.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	enum class CERootSignatureType {
		Unknown = 0,
		Graphics = 1,
		Compute = 2,
		RayTracingGlobal = 3,
		RayTracingLocal = 4
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
		static void InitDescriptorRange();
		static void InitDescriptorTable();
		static void Init32BitConstantRange();

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
		Core::Containers::CEArray<Core::Common::CERef<CEDXRootSignatureResourceCount>> ResourceCounts;

		static CEDXRootSignatureCache* Instance;
	};
}
