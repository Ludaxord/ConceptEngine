#include "CEDXShaderCompiler.h"

using namespace ConceptEngine::Graphics::DirectX12::Modules::RenderLayer;

CEDXShaderCompiler* CED3DShaderCompiler = nullptr;

CEDXShaderCompiler::CEDXShaderCompiler(): CEIShaderCompiler(),
                                          DXCompiler(nullptr),
                                          DXLibrary(nullptr),
                                          DXLinker(nullptr),
                                          DXIncludeHandler(nullptr),
                                          DXReflection(nullptr),
                                          DXCompilerDLL() {
	CED3DShaderCompiler = this;
}

CEDXShaderCompiler::~CEDXShaderCompiler() {
	CED3DShaderCompiler = nullptr;
	DXCompiler.Reset();
	DXLibrary.Reset();
	DXLinker.Reset();
	DXIncludeHandler.Reset();
	DXReflection.Reset();
	::FreeLibrary(DXCompilerDLL);
}

bool CEDXShaderCompiler::Create() {
	return false;
}

bool CEDXShaderCompiler::CompileFromFile(const std::string& filePath, const std::string& entryPoint,
                                         const Core::Containers::CEArray<CEShaderDefine>* defines,
                                         CEShaderStage shaderStage, CEShaderModel shaderModel,
                                         Core::Containers::CEArray<uint8>& code) {
	return false;
}

bool CEDXShaderCompiler::CompileShader(const std::string& shaderSource, const std::string& entryPoint,
                                       const Core::Containers::CEArray<CEShaderDefine>* defines,
                                       CEShaderStage shaderStage, CEShaderModel shaderModel,
                                       Core::Containers::CEArray<uint8>& code) {
	return false;
}

bool CEDXShaderCompiler::GetReflection(CEDXBaseShader* shader, ID3D12ShaderReflection** reflection) {
	return false;
}

bool CEDXShaderCompiler::GetLibraryReflection(CEDXBaseShader* shader, ID3D12LibraryReflection** reflection) {
	return false;
}

bool CEDXShaderCompiler::HasRootSignature(CEDXBaseShader* shader) {
	return false;
}

bool CEDXShaderCompiler::InternalCompileFromSource(IDxcBlob* sourceBlob, LPCWSTR filePath, LPCWSTR entryPoint,
                                                   CEShaderStage shaderStage, CEShaderModel shaderModel,
                                                   const Core::Containers::CEArray<CEShaderDefine>* defines,
                                                   Core::Containers::CEArray<uint8>& code) {
	return false;
}

bool CEDXShaderCompiler::InternalGetReflection(const ComPtr<IDxcBlob>& shaderBlob, const IID& IID, void** ppvObject) {
	return false;
}

bool CEDXShaderCompiler::ValidateRayTracingShader(const ComPtr<IDxcBlob>& shaderBlob, LPCWSTR entryPoint) {
	return false;
}
