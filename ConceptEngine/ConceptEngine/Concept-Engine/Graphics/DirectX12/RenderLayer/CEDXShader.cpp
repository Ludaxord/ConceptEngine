#include "CEDXShader.h"

#include "CEDXShaderCompiler.h"
#include "../../../Memory/CEMemory.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

CEDXBaseShader::CEDXBaseShader(CEDXDevice* device, const Core::Containers::CEArray<uint8>& code,
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

template <typename TCEDXReflectionInterface>
bool CEDXBaseShader::GetShaderResourceBindings(TCEDXReflectionInterface* reflection, CEDXBaseShader* shader,
                                               uint32 numBoundResources) {
	return true;
}
