#include "CEPipelineStateObject.h"

#include <d3dcompiler.h>

#include "CEDevice.h"
#include "CEHelper.h"
using namespace Concept::GraphicsEngine::Direct3D;

D3D12_SHADER_BYTECODE CEPipelineStateObject::CompileShaderFileToByteCode(std::wstring shaderPath) {
	wrl::ComPtr<ID3DBlob> shader;
	wrl::ComPtr<ID3DBlob> errorBuffer;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	HRESULT hr = D3DCompileFromFile(shaderPath.c_str(),
	                                nullptr,
	                                nullptr,
	                                "main",
	                                "vs_5_0",
	                                compileFlags,
	                                0,
	                                &shader,
	                                &errorBuffer);

	if (FAILED(hr)) {
		OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
	}

	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE shaderBytecode = {};
	shaderBytecode.BytecodeLength = shader->GetBufferSize();
	shaderBytecode.pShaderBytecode = shader->GetBufferPointer();
	return shaderBytecode;
}

CEPipelineStateObject::CEPipelineStateObject(CEDevice& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc): m_device(
	device) {
	auto d3d12Device = device.GetDevice();
	ThrowIfFailed(d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&m_pipelineState)));
}
