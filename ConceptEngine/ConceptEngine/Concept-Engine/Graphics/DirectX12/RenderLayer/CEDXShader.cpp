#include "CEDXShader.h"

#include "CEDXShaderCompiler.h"
#include "../../../Memory/CEMemory.h"
#include "../../../Math/CEMath.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

void CEShaderResourceCount::Combine(const CEShaderResourceCount& another) {
	Ranges.NumCBVs = Math::CEMath::Max(Ranges.NumCBVs, another.Ranges.NumCBVs);
	Ranges.NumSRVs = Math::CEMath::Max(Ranges.NumSRVs, another.Ranges.NumSRVs);
	Ranges.NumUAVs = Math::CEMath::Max(Ranges.NumUAVs, another.Ranges.NumUAVs);
	Ranges.NumSamplers = Math::CEMath::Max(Ranges.NumSamplers, another.Ranges.NumSamplers);
	Num32BitConstants = Math::CEMath::Max(Num32BitConstants, another.Num32BitConstants);
}

bool CEShaderResourceCount::IsCompatible(const CEShaderResourceCount& another) const {
	if (Num32BitConstants > another.Num32BitConstants) {
		return false;
	}

	if (Ranges.NumCBVs > another.Ranges.NumCBVs) {
		return false;
	}

	if (Ranges.NumSRVs > another.Ranges.NumSRVs) {
		return false;
	}

	if (Ranges.NumUAVs > another.Ranges.NumUAVs) {
		return false;
	}

	if (Ranges.NumSamplers > another.Ranges.NumSamplers) {
		return false;
	}

	return true;
}

CEDXBaseShader::CEDXBaseShader(CEDXDevice* device, const CEArray<uint8>& code,
                               CEShaderVisibility shaderVisibility) : CEDXDeviceElement(device), ByteCode(),
                                                                      Visibility(shaderVisibility) {
	ByteCode.BytecodeLength = code.SizeInBytes();
	ByteCode.pShaderBytecode = Memory::CEMemory::Malloc(ByteCode.BytecodeLength);

	Memory::CEMemory::Memcpy((void*)ByteCode.pShaderBytecode, code.Data(), ByteCode.BytecodeLength);
}

CEDXBaseShader::~CEDXBaseShader() {
	Memory::CEMemory::Free((void*)ByteCode.pShaderBytecode);

	ByteCode.pShaderBytecode = nullptr;
	ByteCode.BytecodeLength = 0;
}

static bool IsShaderResourceView(D3D_SHADER_INPUT_TYPE type) {
	return type == D3D_SIT_TEXTURE || type == D3D_SIT_BYTEADDRESS || type == D3D_SIT_STRUCTURED || type ==
		D3D_SIT_RTACCELERATIONSTRUCTURE;
}

static bool IsUnorderedAccessView(D3D_SHADER_INPUT_TYPE type) {
	return type == D3D_SIT_UAV_RWTYPED || type == D3D_SIT_UAV_RWBYTEADDRESS || type == D3D_SIT_UAV_RWSTRUCTURED;
}

static bool IsRayTracingLocalSpace(uint32 registerSpace) {
	return registerSpace == D3D12_SHADER_REGISTER_SPACE_RT_LOCAL;
}

static bool IsLegalRegisterSpace(const D3D12_SHADER_INPUT_BIND_DESC& shaderBindDesc) {
	if (shaderBindDesc.Space == D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS && shaderBindDesc.Type == D3D_SIT_CBUFFER) {
		return true;
	}

	if (IsRayTracingLocalSpace(shaderBindDesc.Space)) {
		return true;
	}

	if (shaderBindDesc.Space == 0) {
		return true;
	}

	return false;
}

bool CEDXBaseShader::GetShaderReflection(CEDXBaseShader* shader) {
	Assert(shader != nullptr);

	Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflection;
	if (!static_cast<CEDXShaderCompiler*>(ShaderCompiler)->GetReflection(shader, &reflection)) {
		return false;
	}

	D3D12_SHADER_DESC shaderDesc;
	if (FAILED(reflection->GetDesc(&shaderDesc))) {
		return false;
	}

	if (!GetShaderResourceBindings(reflection.Get(), shader, shaderDesc.BoundResources)) {
		CE_LOG_ERROR("[CEDXShader]: Error when analysing shader parameters");
		return false;
	}

	if (static_cast<CEDXShaderCompiler*>(ShaderCompiler)->HasRootSignature(shader)) {
		shader->ContainsRootSignature = true;
	}

	return true;
}

//TODO: Implement
bool CEDXBaseHullShader::Create() {
	return false;
}

//TODO: Implement
bool CEDXBaseDomainShader::Create() {
	return false;
}

//TODO: Implement
bool CEDXBaseGeometryShader::Create() {
	return false;
}

//TODO: Implement
bool CEDXBaseMeshShader::Create() {
	return false;
}

//TODO: Implement
bool CEDXBaseAmplificationShader::Create() {
	return false;
}

bool CEDXBaseComputeShader::Create() {
	Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflection;
	if (!static_cast<CEDXShaderCompiler*>(ShaderCompiler)->GetReflection(this, &reflection)) {
		CE_LOG_ERROR("[CEDXBaseComputeShader]: Error in GetReflection");
		return false;
	}

	D3D12_SHADER_DESC shaderDesc;
	if (FAILED(reflection->GetDesc(&shaderDesc))) {
		return false;
	}

	if (!GetShaderResourceBindings(reflection.Get(), this, shaderDesc.BoundResources)) {
		CE_LOG_ERROR("[CEDXBaseComputeShader]: Error when analysing shader parameters");
		return false;
	}

	if (static_cast<CEDXShaderCompiler*>(ShaderCompiler)->HasRootSignature(this)) {
		ContainsRootSignature = true;
	}

	reflection->GetThreadGroupSize(&ThreadGroupXYZ.Native.x, &ThreadGroupXYZ.Native.y, &ThreadGroupXYZ.Native.z);

	return true;
}

bool CEDXBaseRayTracingShader::GetRayTracingShaderReflection(CEDXBaseRayTracingShader* shader) {
	Assert(Shader != nullptr);
	Microsoft::WRL::ComPtr<ID3D12LibraryReflection> reflection;
	if (!static_cast<CEDXShaderCompiler*>(ShaderCompiler)->GetLibraryReflection(shader, &reflection)) {
		return false;
	}

	D3D12_LIBRARY_DESC libDesc;
	Memory::CEMemory::Memzero(&libDesc);

	HRESULT result = reflection->GetDesc(&libDesc);
	if (FAILED(result)) {
		return false;
	}

	Assert(libDesc.FunctionCount > 0);

	ID3D12FunctionReflection* function = reflection->GetFunctionByIndex(0);

	D3D12_FUNCTION_DESC funDesc;
	Memory::CEMemory::Memzero(&funDesc);

	function->GetDesc(&funDesc);
	if (FAILED(result)) {
		return false;
	}

	if (!GetShaderResourceBindings(function, shader, funDesc.BoundResources)) {
		CE_LOG_ERROR("[CEDXBaseRayTracingShader]: Error when analysing ");
		return false;
	}

	std::string identifier = funDesc.Name;

	auto nameStart = identifier.find_last_of("\x1?");
	if (nameStart != std::string::npos) {
		nameStart++;
	}

	auto nameEnd = identifier.find_first_of("@");

	shader->Identifier = identifier.substr(nameStart, nameEnd - nameStart);
	return true;
}

template <typename TCEDXReflectionInterface>
bool CEDXBaseShader::GetShaderResourceBindings(TCEDXReflectionInterface* reflection,
                                               CEDXBaseShader* shader,
                                               uint32 numBoundResources) {
	CEShaderResourceCount resourceCount;
	CEShaderResourceCount RTLocalResourceCount;
	CEArray<CEDXShaderParameter> constantBufferParameters;
	CEArray<CEDXShaderParameter> samplerParameters;
	CEArray<CEDXShaderParameter> shaderResourceParameters;
	CEArray<CEDXShaderParameter> unorderedAccessParameters;

	D3D12_SHADER_INPUT_BIND_DESC shaderBindDesc;
	for (uint32 i = 0; i < numBoundResources; i++) {
		Memory::CEMemory::Memzero(&shaderBindDesc);

		if (FAILED(reflection->GetResourceBindingDesc(i, &shaderBindDesc))) {
			continue;
		}

		if (!IsLegalRegisterSpace(shaderBindDesc)) {
			CE_LOG_ERROR(
				"Shader Parameter '" + std::string(shaderBindDesc.Name) + "' has register space '" + std::to_string(
					shaderBindDesc.Space) + "' spacified, which is invalid.");
			return false;
		}

		if (shaderBindDesc.Type == D3D_SIT_CBUFFER) {
			uint32 sizeInBytes = 0;

			ID3D12ShaderReflectionConstantBuffer* bufferVar = reflection->GetConstantBufferByName(shaderBindDesc.Name);
			if (bufferVar) {
				D3D12_SHADER_BUFFER_DESC bufferDesc;
				if (SUCCEEDED(bufferVar->GetDesc(&bufferDesc))) {
					sizeInBytes = bufferDesc.Size;
				}
			}

			uint32 num32BitConstants = sizeInBytes / 4;

			if (shaderBindDesc.Space == D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
				if (shaderBindDesc.BindCount > 1 || num32BitConstants > D3D12_MAX_32BIT_SHADER_CONSTANTS_COUNT ||
					resourceCount.Num32BitConstants != 0) {
					return false;
				}

				resourceCount.Num32BitConstants = num32BitConstants;
			}
			else {
				samplerParameters.EmplaceBack(shaderBindDesc.Name, shaderBindDesc.BindPoint, shaderBindDesc.Space,
				                              shaderBindDesc.BindCount, 0);
				if (shaderBindDesc.Space == 0) {
					resourceCount.Ranges.NumCBVs = Math::CEMath::Max(resourceCount.Ranges.NumCBVs,
					                                                 shaderBindDesc.BindPoint + shaderBindDesc.
					                                                 BindCount);
				}
				else {
					RTLocalResourceCount.Ranges.NumCBVs = Math::CEMath::Max(RTLocalResourceCount.Ranges.NumCBVs,
					                                                        shaderBindDesc.BindPoint + shaderBindDesc.
					                                                        BindCount);
				}
			}
		}
		else if (shaderBindDesc.Type == D3D_SIT_SAMPLER) {
			samplerParameters.EmplaceBack(shaderBindDesc.Name, shaderBindDesc.BindPoint, shaderBindDesc.Space,
			                              shaderBindDesc.BindCount, 0);
			if (shaderBindDesc.Space == 0) {
				resourceCount.Ranges.NumSamplers == Math::CEMath::Max(resourceCount.Ranges.NumSamplers,
				                                                      shaderBindDesc.BindPoint + shaderBindDesc.
				                                                      BindCount);
			}
			else {
				RTLocalResourceCount.Ranges.NumSamplers = Math::CEMath::Max(
					RTLocalResourceCount.Ranges.NumSamplers, shaderBindDesc.BindPoint + shaderBindDesc.BindCount);
			}

		}
		else if (IsShaderResourceView(shaderBindDesc.Type)) {

			const uint32 numDescriptors = shaderBindDesc.BindCount != 0 ? shaderBindDesc.BindCount : UINT_MAX;

			shaderResourceParameters.EmplaceBack(shaderBindDesc.Name, shaderBindDesc.BindPoint, shaderBindDesc.Space,
			                                     numDescriptors, 0);

			if (shaderBindDesc.Space == 0) {
				resourceCount.Ranges.NumSRVs == Math::CEMath::Max(resourceCount.Ranges.NumSRVs,
				                                                  shaderBindDesc.BindPoint + shaderBindDesc.
				                                                  BindCount);
			}
			else {
				RTLocalResourceCount.Ranges.NumSRVs = Math::CEMath::Max(
					RTLocalResourceCount.Ranges.NumSRVs, shaderBindDesc.BindPoint + shaderBindDesc.BindCount);
			}
		}
		else if (IsUnorderedAccessView(shaderBindDesc.Type)) {

			unorderedAccessParameters.EmplaceBack(shaderBindDesc.Name, shaderBindDesc.BindPoint, shaderBindDesc.Space,
			                                      shaderBindDesc.BindCount, 0);

			if (shaderBindDesc.Space == 0) {
				resourceCount.Ranges.NumUAVs == Math::CEMath::Max(resourceCount.Ranges.NumUAVs,
				                                                  shaderBindDesc.BindPoint + shaderBindDesc.
				                                                  BindCount);
			}
			else {
				RTLocalResourceCount.Ranges.NumUAVs = Math::CEMath::Max(
					RTLocalResourceCount.Ranges.NumUAVs, shaderBindDesc.BindPoint + shaderBindDesc.BindCount);
			}
		}
	}

	shader->ConstantBufferParameters = Move(constantBufferParameters);
	shader->SamplerParameters = Move(samplerParameters);
	shader->ShaderResourceParameters = Move(shaderResourceParameters);
	shader->UnorderedAccessParameters = Move(unorderedAccessParameters);
	shader->ResourceCount = resourceCount;
	shader->RTLocalResourceCount = RTLocalResourceCount;

	return true;
}
