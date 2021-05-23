#pragma once
#include "CEDXDeviceElement.h"
#include "../../Main/RenderLayer/CEShader.h"

#include "../../../Math/CEMathTypes.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {

	//TODO: create static sampler class right not put enums here:
	enum CEStaticSampler {

	};

	enum CEShaderVisibility {
		ShaderVisibility_All = 0,
		ShaderVisibility_Vertex = 1,
		ShaderVisibility_Hull = 2,
		ShaderVisibility_Domain = 3,
		ShaderVisibility_Geometry = 4,
		ShaderVisibility_Pixel = 5,
		ShaderVisibility_Count = ShaderVisibility_Pixel + 1,
	};

	enum CEResourceType {
		ResourceType_CBV = 0,
		ResourceType_SRV = 1,
		ResourceType_UAV = 2,
		ResourceType_Sampler = 3,
		ResourceType_Count = ResourceType_Sampler + 1,
		ResourceType_Unknown = 5,
	};

	struct CEShaderResourceRange {
		uint32 NumCBVs = 0;
		uint32 NumSRVs = 0;
		uint32 NumUAVs = 0;
		uint32 NumSamplers = 0;
	};

	struct CEShaderResourceCount {
		void Combine(const CEShaderResourceCount& another);
		bool IsCompatible(const CEShaderResourceCount& another) const;

		CEShaderResourceRange Ranges;
		uint32 Num32BitConstants = 0;
	};


	struct CEDXShaderParameter {
		CEDXShaderParameter() = default;

		CEDXShaderParameter(const std::string& name, uint32 shaderRegister, uint32 space, uint32 numDescriptors,
		                    uint32 sizeInBytes) : Name(name), Register(shaderRegister), Space(space),
		                                          NumDescriptors(numDescriptors), SizeInBytes(sizeInBytes) {

		}

		std::string Name;
		uint32 Register = 0;
		uint32 Space = 0;
		uint32 NumDescriptors = 0;
		uint32 SizeInBytes = 0;
	};

	class CEDXBaseShader : public CEDXDeviceElement {
	public:
		CEDXBaseShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code,
		               CEShaderVisibility shaderVisibility);
		~CEDXBaseShader();

		D3D12_SHADER_BYTECODE GetByteCode() const {
			return ByteCode;
		}

		const void* GetCode() const {
			return ByteCode.pShaderBytecode;
		}

		uint64 GetCodeSize() const {
			return static_cast<uint64>(ByteCode.BytecodeLength);
		}

		CEDXShaderParameter GetConstantBufferParameter(uint32 parameterIndex) {
			return ConstantBufferParameters[parameterIndex];
		}

		uint32 GetNumConstantBufferParameters() {
			return ConstantBufferParameters.Size();
		}

		CEDXShaderParameter GetShaderResourceParameter(uint32 parameterIndex) {
			return ShaderResourceParameters[parameterIndex];
		}

		uint32 GetNumShaderResourceParameters() {
			return ShaderResourceParameters.Size();
		}

		CEDXShaderParameter GetUnorderedAccessParameter(uint32 parameterIndex) {
			return UnorderedAccessParameters[parameterIndex];
		}

		uint32 GetNumUnorderedAccessParameters() {
			return UnorderedAccessParameters.Size();
		}

		CEDXShaderParameter GetSamplerStateParameter(uint32 parameterIndex) {
			return SamplerParameters[parameterIndex];
		}

		uint32 GetNumSamplerStateParameters() {
			return SamplerParameters.Size();
		}

		bool HasRootSignature() const {
			return ContainsRootSignature;
		}

		CEShaderVisibility GetShaderVisibility() const {
			return Visibility;
		}

		const CEShaderResourceCount& GetResourceCount() const {
			return ResourceCount;
		}

		const CEShaderResourceCount& GetRTLocalResourceCount() const {
			return RTLocalResourceCount;
		}

		static bool GetShaderReflection(class CEDXBaseShader* shader);

	protected:
		template <typename TCEDXReflectionInterface>
		static bool GetShaderResourceBindings(TCEDXReflectionInterface* reflection, CEDXBaseShader* shader,
		                                      uint32 numBoundResources);
		D3D12_SHADER_BYTECODE ByteCode;
		Core::Containers::CEArray<CEDXShaderParameter> ConstantBufferParameters;
		Core::Containers::CEArray<CEDXShaderParameter> ShaderResourceParameters;
		Core::Containers::CEArray<CEDXShaderParameter> UnorderedAccessParameters;
		Core::Containers::CEArray<CEDXShaderParameter> SamplerParameters;
		CEShaderVisibility Visibility;
		CEShaderResourceCount ResourceCount;
		CEShaderResourceCount RTLocalResourceCount;

		bool ContainsRootSignature = false;
	private:
	};

	template <typename TBaseShader>
	class TCEDXShader : public TBaseShader {
	public:
		TCEDXShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code): TBaseShader(device, code) {

		}

		virtual void GetShaderParameterInfo(CEShaderParameterInfo& shaderParameterInfo) const override;

		virtual bool GetShaderResourceViewIndexByName(const std::string& name, uint32& index) const override;

		virtual bool GetSamplerIndexByName(const std::string& name, uint32& index) const override;

		virtual bool GetUnorderedAccessViewIndexByName(const std::string& name, uint32& index) const override;

		virtual bool GetConstantBufferIndexByName(const std::string& name, uint32& index) const override;

		virtual bool IsValid() const override;

	protected:
	private:
		bool InternalFindParameterIndexByName(const Core::Containers::CEArray<CEDXShaderParameter>& parameters,
		                                      const std::string& name, uint32& index) const {
			for (uint32 i = 0; i < parameters.Size(); i++) {
				if (parameters[i].Name == name) {
					index = i;
					return true;
				}
			}
			return false;
		}
	};

	class CEDXBaseVertexShader : public CEVertexShader, public CEDXBaseShader {
	public:
		CEDXBaseVertexShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code) : CEVertexShader(),
			CEDXBaseShader(device, code, ShaderVisibility_Vertex) {

		}
	};

	class CEDXBasePixelShader : public CEPixelShader, public CEDXBaseShader {
	public:
		CEDXBasePixelShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code) : CEPixelShader(),
			CEDXBaseShader(device, code, ShaderVisibility_Pixel) {

		}
	};

	class CEDXBaseComputeShader : public CEComputeShader, public CEDXBaseShader {
	public:
		CEDXBaseComputeShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code) : CEComputeShader(),
			CEDXBaseShader(device, code, ShaderVisibility_All), ThreadGroupXYZ(0, 0, 0) {

		}

		bool Create();

		virtual CEVectorUint3 GetThreadGroupXYZ() const override {
			return ThreadGroupXYZ;
		}


	protected:
		CEVectorUint3 ThreadGroupXYZ;
	};

	class CEDXBaseRayTracingShader : public CEDXBaseShader {
	public:
		CEDXBaseRayTracingShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code): CEDXBaseShader(
			device, code, ShaderVisibility_All) {

		}

		const std::string& GetIdentifier() const {
			return Identifier;
		}

		static bool GetRayTracingShaderReflection(class CEDXRayTracingShader* shader);

	protected:
		std::string Identifier;
	};

	class CEDXBaseRayGenShader : public CERayGenShader, public CEDXBaseRayTracingShader {
	public:
		CEDXBaseRayGenShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code) : CERayGenShader(),
			CEDXBaseRayTracingShader(device, code) {

		}
	};

	class CEDXBaseRayAnyHitShader : public CERayAnyHitShader, public CEDXBaseRayTracingShader {
	public:
		CEDXBaseRayAnyHitShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code) : CERayAnyHitShader(),
			CEDXBaseRayTracingShader(device, code) {

		}
	};

	class CEDXBaseRayClosestHitShader : public CERayAnyHitShader, public CEDXBaseRayTracingShader {
	public:
		CEDXBaseRayClosestHitShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code) :
			CERayAnyHitShader(), CEDXBaseRayTracingShader(device, code) {

		}
	};

	class CEDXBaseRayMissShader : public CERayMissShader, public CEDXBaseRayTracingShader {
		CEDXBaseRayMissShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code) : CERayMissShader(),
			CEDXBaseRayTracingShader(device, code) {

		}
	};

	using CEDXVertexShader = TCEDXShader<CEDXBaseVertexShader>;
	using CEDXPixelShader = TCEDXShader<CEDXBasePixelShader>;
	using CEDXComputeShader = TCEDXShader<CEDXBaseComputeShader>;
	using CEDXRayGenShader = TCEDXShader<CEDXBaseRayGenShader>;
	using CEDXRayAnyHitShader = TCEDXShader<CEDXBaseRayAnyHitShader>;
	using CEDXRayClosestHitShader = TCEDXShader<CEDXBaseRayClosestHitShader>;
	using CEDXRayMissShader = TCEDXShader<CEDXBaseRayMissShader>;

	inline CEDXBaseShader* ShaderCast(CEShader* shader) {
		if (shader->AsVertexShader()) {
			return static_cast<CEDXVertexShader*>(shader);
		}
		else if (shader->AsPixelShader()) {
			return static_cast<CEDXPixelShader*>(shader);
		}
		else if (shader->AsComputeShader()) {
			return static_cast<CEDXComputeShader*>(shader);
		}
		else if (shader->AsRayGenShader()) {
			return static_cast<CEDXRayGenShader*>(shader);
		}
		else if (shader->AsRayAnyHitShader()) {
			return static_cast<CEDXRayAnyHitShader*>(shader);
		}
		else if (shader->AsRayClosestHitShader()) {
			return static_cast<CEDXRayClosestHitShader*>(shader);
		}
		else if (shader->AsRayMissShader()) {
			return static_cast<CEDXRayMissShader*>(shader);
		}
		else {
			return nullptr;
		}
	}

}
