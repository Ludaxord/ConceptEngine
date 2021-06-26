#include "CEDXShaderCompiler.h"

#include "../../../Utilities/CEStringUtilities.h"

#include "../../../Core/Platform/CEPlatformActions.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

ConceptEngine::Graphics::Main::RenderLayer::CEIShaderCompiler* ShaderCompiler = nullptr;

DxcCreateInstanceProc DxcCreateInstanceFunc = nullptr;

static LPCWSTR GetTargetProfile(CEShaderStage stage, CEShaderModel model) {
	switch (stage) {
	case CEShaderStage::Compute: {
		switch (model) {
		case CEShaderModel::SM_5_0: return L"cs_5_0";
		case CEShaderModel::SM_5_1: return L"cs_5_1";
		case CEShaderModel::SM_6_0: return L"cs_6_0";
		case CEShaderModel::SM_6_1: return L"cs_6_1";
		case CEShaderModel::SM_6_2: return L"cs_6_2";
		case CEShaderModel::SM_6_3: return L"cs_6_3";
		case CEShaderModel::SM_6_4: return L"cs_6_4";
		case CEShaderModel::SM_6_5: return L"cs_6_5";
		default: break;
		}
	}
	case CEShaderStage::Vertex: {
		switch (model) {
		case CEShaderModel::SM_5_0: return L"vs_5_0";
		case CEShaderModel::SM_5_1: return L"vs_5_1";
		case CEShaderModel::SM_6_0: return L"vs_6_0";
		case CEShaderModel::SM_6_1: return L"vs_6_1";
		case CEShaderModel::SM_6_2: return L"vs_6_2";
		case CEShaderModel::SM_6_3: return L"vs_6_3";
		case CEShaderModel::SM_6_4: return L"vs_6_4";
		case CEShaderModel::SM_6_5: return L"vs_6_5";
		default: break;
		}
	}
	case CEShaderStage::Hull: {
		switch (model) {
		case CEShaderModel::SM_5_0: return L"hs_5_0";
		case CEShaderModel::SM_5_1: return L"hs_5_1";
		case CEShaderModel::SM_6_0: return L"hs_6_0";
		case CEShaderModel::SM_6_1: return L"hs_6_1";
		case CEShaderModel::SM_6_2: return L"hs_6_2";
		case CEShaderModel::SM_6_3: return L"hs_6_3";
		case CEShaderModel::SM_6_4: return L"hs_6_4";
		case CEShaderModel::SM_6_5: return L"hs_6_5";
		default: break;
		}
	}
	case CEShaderStage::Domain: {
		switch (model) {
		case CEShaderModel::SM_5_0: return L"ds_5_0";
		case CEShaderModel::SM_5_1: return L"ds_5_1";
		case CEShaderModel::SM_6_0: return L"ds_6_0";
		case CEShaderModel::SM_6_1: return L"ds_6_1";
		case CEShaderModel::SM_6_2: return L"ds_6_2";
		case CEShaderModel::SM_6_3: return L"ds_6_3";
		case CEShaderModel::SM_6_4: return L"ds_6_4";
		case CEShaderModel::SM_6_5: return L"ds_6_5";
		default: break;
		}
	}
	case CEShaderStage::Geometry: {
		switch (model) {
		case CEShaderModel::SM_5_0: return L"gs_5_0";
		case CEShaderModel::SM_5_1: return L"gs_5_1";
		case CEShaderModel::SM_6_0: return L"gs_6_0";
		case CEShaderModel::SM_6_1: return L"gs_6_1";
		case CEShaderModel::SM_6_2: return L"gs_6_2";
		case CEShaderModel::SM_6_3: return L"gs_6_3";
		case CEShaderModel::SM_6_4: return L"gs_6_4";
		case CEShaderModel::SM_6_5: return L"gs_6_5";
		default: break;
		}
	}
	case CEShaderStage::Mesh: {
		switch (model) {
		case CEShaderModel::SM_5_0: return L"ms_5_0";
		case CEShaderModel::SM_5_1: return L"ms_5_1";
		case CEShaderModel::SM_6_0: return L"ms_6_0";
		case CEShaderModel::SM_6_1: return L"ms_6_1";
		case CEShaderModel::SM_6_2: return L"ms_6_2";
		case CEShaderModel::SM_6_3: return L"ms_6_3";
		case CEShaderModel::SM_6_4: return L"ms_6_4";
		case CEShaderModel::SM_6_5: return L"ms_6_5";
		default: break;
		}
	}
	case CEShaderStage::Pixel: {
		switch (model) {
		case CEShaderModel::SM_5_0: return L"ps_5_0";
		case CEShaderModel::SM_5_1: return L"ps_5_1";
		case CEShaderModel::SM_6_0: return L"ps_6_0";
		case CEShaderModel::SM_6_1: return L"ps_6_1";
		case CEShaderModel::SM_6_2: return L"ps_6_2";
		case CEShaderModel::SM_6_3: return L"ps_6_3";
		case CEShaderModel::SM_6_4: return L"ps_6_4";
		case CEShaderModel::SM_6_5: return L"ps_6_5";
		default: break;
		}
	}
	case CEShaderStage::Amplification: {
		switch (model) {
		case CEShaderModel::SM_5_0: return L"as_5_0";
		case CEShaderModel::SM_5_1: return L"as_5_1";
		case CEShaderModel::SM_6_0: return L"as_6_0";
		case CEShaderModel::SM_6_1: return L"as_6_1";
		case CEShaderModel::SM_6_2: return L"as_6_2";
		case CEShaderModel::SM_6_3: return L"as_6_3";
		case CEShaderModel::SM_6_4: return L"as_6_4";
		case CEShaderModel::SM_6_5: return L"as_6_5";
		default: break;
		}
	}
	case CEShaderStage::RayGen:
	case CEShaderStage::RayAnyHit:
	case CEShaderStage::RayClosestHit:
	case CEShaderStage::RayMiss: {
		switch (model) {
		case CEShaderModel::SM_6_3: return L"lib_6_3";
		case CEShaderModel::SM_6_4: return L"lib_6_4";
		case CEShaderModel::SM_6_5: return L"lib_6_5";
		default: break;
		}
	}

		return L"Unknown";
	}
}

class CEExistingBlob : public IDxcBlob {
public:
	CEExistingBlob(LPVOID data, SIZE_T sizeInBytes) : Data(nullptr), SizeInBytes(sizeInBytes), References(0) {
		Data = ConceptEngine::Memory::CEMemory::Malloc(SizeInBytes);
		ConceptEngine::Memory::CEMemory::Memcpy(Data, data, SizeInBytes);
	}

	~CEExistingBlob() {
		ConceptEngine::Memory::CEMemory::Free(Data);
	}

	virtual LPVOID GetBufferPointer(void) override {
		return Data;
	}

	virtual SIZE_T GetBufferSize(void) override {
		return SizeInBytes;
	}

	virtual HRESULT QueryInterface(REFIID riid, LPVOID* ppvObject) {
		if (!ppvObject) {
			return E_INVALIDARG;
		}

		*ppvObject = NULL;

		if (riid == IID_IUnknown || riid == IID_ID3DBlob) {
			*ppvObject = (LPVOID)this;
			AddRef();
			return NO_ERROR;
		}

		return E_NOINTERFACE;
	}

	virtual ULONG AddRef() {
		InterlockedIncrement(&References);
		return References;
	}

	virtual ULONG Release() {
		ULONG numRefs = InterlockedDecrement(&References);
		if (numRefs == 0) {
			delete this;
		}

		return numRefs;
	}

protected:
private:
	LPVOID Data;
	SIZE_T SizeInBytes;
	ULONG References;
};

CEDXShaderCompiler::CEDXShaderCompiler(): CEIShaderCompiler(),
                                          DXCompiler(nullptr),
                                          DXLibrary(nullptr),
                                          DXLinker(nullptr),
                                          DXIncludeHandler(nullptr),
                                          DXReflection(nullptr),
                                          DXCompilerDLL() {
	ShaderCompiler = this;
}

CEDXShaderCompiler::~CEDXShaderCompiler() {
	ShaderCompiler = nullptr;
	DXCompiler.Reset();
	DXLibrary.Reset();
	DXLinker.Reset();
	DXIncludeHandler.Reset();
	DXReflection.Reset();
	::FreeLibrary(DXCompilerDLL);
}

bool CEDXShaderCompiler::Create(bool useLib) {
	UseLib = useLib;
	if (useLib) {
		if (!CreateLib()) {
			return false;
		}
	}
	else {
		if (!CreateDLL()) {
			return false;
		}
	}
	return true;
}

bool CEDXShaderCompiler::CreateDLL() {
	DXCompilerDLL = ::LoadLibrary(L"dxcompiler.dll");
	if (!DXCompilerDLL) {
		CEPlatformActions::MessageBox("Error", "Failed to load dxCompiler.dll");
		return false;
	}

	DxcCreateInstanceFunc = CEPlatformActions::GetTypedProcAddress<DxcCreateInstanceProc>(
		DXCompilerDLL, "DxcCreateInstance");
	if (!DxcCreateInstanceFunc) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to load DxcCreateInstance");
		return false;
	}

	HRESULT hResult = DxcCreateInstanceFunc(CLSID_DxcCompiler, IID_PPV_ARGS(&DXCompiler));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxCompiler");
		return false;
	}

	hResult = DxcCreateInstanceFunc(CLSID_DxcLibrary, IID_PPV_ARGS(&DXLibrary));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxLibrary");
		return false;
	}

	hResult = DXLibrary->CreateIncludeHandler(&DXIncludeHandler);
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxIncludeHandler");
		return false;
	}

	hResult = DxcCreateInstanceFunc(CLSID_DxcLinker, IID_PPV_ARGS(&DXLinker));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxLinker");
		return false;
	}

	hResult = DxcCreateInstanceFunc(CLSID_DxcContainerReflection, IID_PPV_ARGS(&DXReflection));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxReflection");
		return false;
	}

	hResult = DxcCreateInstanceFunc(CLSID_DxcValidator, IID_PPV_ARGS(&DXValidator));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DXValidator");
		return false;
	}

	return true;
}

bool CEDXShaderCompiler::CreateLib() {
	HRESULT hResult = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&DXCompiler));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxCompiler");
		return false;
	}

	hResult = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&DXLibrary));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxLibrary");
		return false;
	}

	hResult = DXLibrary->CreateIncludeHandler(&DXIncludeHandler);
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxIncludeHandler");
		return false;
	}

	hResult = DxcCreateInstance(CLSID_DxcLinker, IID_PPV_ARGS(&DXLinker));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxLinker");
		return false;
	}

	hResult = DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&DXReflection));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxReflection");
		return false;
	}

	return true;
}

bool CEDXShaderCompiler::CompileFromFile(const std::string& filePath, const std::string& entryPoint,
                                         const CEArray<CEShaderDefine>* defines,
                                         CEShaderStage shaderStage, CEShaderModel shaderModel,
                                         CEArray<uint8>& code) {
	std::wstring wFilePath = ConvertToWString(filePath);
	std::wstring wEntryPoint = ConvertToWString(entryPoint);

	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
	HRESULT hResult = DXLibrary->CreateBlobFromFile(wFilePath.c_str(), nullptr, &sourceBlob);
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create Source data.");

		return false;
	}

	return InternalCompileFromSource(sourceBlob.Get(), wFilePath.c_str(), wEntryPoint.c_str(), shaderStage, shaderModel,
	                                 defines, code);
}

bool CEDXShaderCompiler::CompileShader(const std::string& shaderSource, const std::string& entryPoint,
                                       const CEArray<CEShaderDefine>* defines,
                                       CEShaderStage shaderStage, CEShaderModel shaderModel,
                                       CEArray<uint8>& code) {

	std::wstring wEntrypoint = ConvertToWString(entryPoint);

	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
	HRESULT result = DXLibrary->CreateBlobWithEncodingOnHeapCopy(shaderSource.c_str(),
	                                                             sizeof(char) * static_cast<uint32>(shaderSource.
		                                                             size()),
	                                                             CP_UTF8,
	                                                             &sourceBlob);

	if (FAILED(result)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create Source Data");
		return false;
	}

	return InternalCompileFromSource(sourceBlob.Get(), nullptr, wEntrypoint.c_str(), shaderStage, shaderModel, defines,
	                                 code);
}

bool CEDXShaderCompiler::GetReflection(CEDXBaseShader* shader, ID3D12ShaderReflection** reflection) {
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = new CEExistingBlob((LPVOID)shader->GetCode(), shader->GetCodeSize());
	return InternalGetReflection(shaderBlob, IID_PPV_ARGS(reflection));
}

bool CEDXShaderCompiler::GetLibraryReflection(CEDXBaseShader* shader, ID3D12LibraryReflection** reflection) {
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = new CEExistingBlob((LPVOID)shader->GetCode(), shader->GetCodeSize());
	return InternalGetReflection(shaderBlob, IID_PPV_ARGS(reflection));
}

bool CEDXShaderCompiler::HasRootSignature(CEDXBaseShader* shader) {
	Microsoft::WRL::ComPtr<IDxcContainerReflection> reflection;
	HRESULT hResult = UseLib
		                  ? DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&reflection))
		                  : DxcCreateInstanceFunc(CLSID_DxcContainerReflection, IID_PPV_ARGS(&reflection));

	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to create DxcCreateInstance");
		return false;
	}

	uint32 partIndex;
	hResult = reflection->FindFirstPartKind(DFCC_RootSignature, &partIndex);
	if (FAILED(hResult)) {
		return false;
	}

	return false;
}

bool CEDXShaderCompiler::ValidateShader(Microsoft::WRL::ComPtr<IDxcBlobEncoding> Blob) {
	ComPtr<IDxcOperationResult> Result;
	DXValidator->Validate(Blob.Get(), DxcValidatorFlags_InPlaceEdit, &Result);
	HRESULT Status;
	Result->GetStatus(&Status);

	Microsoft::WRL::ComPtr<IDxcBlobEncoding> printBlob;
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> printBlobUtf8;
	if (SUCCEEDED(Result->GetErrorBuffer(&printBlob))) {
		DXLibrary->GetBlobAsUtf8(printBlob.Get(), &printBlobUtf8);
	}

	if (FAILED(Status)) {
		if (printBlobUtf8 && printBlobUtf8->GetBufferSize() > 0) {
			CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to Validate Shader");
			CE_LOG_ERROR(reinterpret_cast<LPCSTR>(printBlobUtf8->GetBufferPointer()));
		}
		else {
			CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to validate with Unknown error");
		}

		return false;
	}

	return true;
}

bool CEDXShaderCompiler::InternalCompileFromSource(IDxcBlob* sourceBlob, LPCWSTR filePath, LPCWSTR entryPoint,
                                                   CEShaderStage shaderStage, CEShaderModel shaderModel,
                                                   const CEArray<CEShaderDefine>* defines,
                                                   CEArray<uint8>& code) {
	CEArray<LPCWSTR> args = {L"-O3"};

	if (ShaderStageIsRayTracing(shaderStage)) {
		args.EmplaceBack(L"-exports");
		args.EmplaceBack(entryPoint);
	}

	CEArray<DxcDefine> dxDefines;
	CEArray<std::wstring> strBuff;
	if (defines) {
		strBuff.Reserve(defines->Size() * 2);
		dxDefines.Reserve(defines->Size());

		for (const CEShaderDefine& define : *defines) {
			const std::wstring& wDefine = strBuff.EmplaceBack(ConvertToWString(define.Define));
			const std::wstring& wValue = strBuff.EmplaceBack(ConvertToWString(define.Value));
			dxDefines.PushBack({wDefine.c_str(), wValue.c_str()});
		}
	}

	LPCWSTR targetProfile = GetTargetProfile(shaderStage, shaderModel);

	Microsoft::WRL::ComPtr<IDxcOperationResult> result;
	HRESULT hResult = DXCompiler->Compile(sourceBlob,
	                                      filePath,
	                                      entryPoint, targetProfile,
	                                      args.Data(),
	                                      args.Size(),
	                                      dxDefines.Data(),
	                                      dxDefines.Size(),
	                                      DXIncludeHandler.Get(),
	                                      &result);
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to compile");
		return false;
	}

	if (FAILED(result->GetStatus(&hResult))) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to retrive result. Unknown error");
	}

	Microsoft::WRL::ComPtr<IDxcBlobEncoding> printBlob;
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> printBlobUtf8;
	if (SUCCEEDED(result->GetErrorBuffer(&printBlob))) {
		DXLibrary->GetBlobAsUtf8(printBlob.Get(), &printBlobUtf8);
	}

	if (FAILED(hResult)) {
		if (printBlobUtf8 && printBlobUtf8->GetBufferSize() > 0) {
			CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to compile with following error");
			CE_LOG_ERROR(reinterpret_cast<LPCSTR>(printBlobUtf8->GetBufferPointer()));
		}
		else {
			CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to compile with Unknown error");
		}

		return false;
	}

	std::string asciiFilePath = filePath != nullptr ? ConvertToString(filePath) : "";
	if (printBlobUtf8 && printBlobUtf8->GetBufferSize() > 0) {
		CE_LOG_INFO(
			"[CEDXShaderCompiler]: Successfully compiled shader '" + asciiFilePath + "' with following output: ");
		CE_LOG_INFO(reinterpret_cast<LPCSTR>(printBlobUtf8->GetBufferPointer()));
	}
	else {
		CE_LOG_INFO("[CEDXShaderCompiler]: Successfully compiled shader: '" + asciiFilePath + "'.");
	}

	Microsoft::WRL::ComPtr<IDxcBlob> compiledBlob;
	if (FAILED(result->GetResult(&compiledBlob))) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Failed to retrive result");
		return false;
	}

	const uint32 blobSize = uint32(compiledBlob->GetBufferSize());
	code.Resize(blobSize);

	CE_LOG_INFO("[CEDXShaderCompiler]: Compiled size: " + std::to_string(blobSize) + " bytes");
	Memory::CEMemory::Memcpy(code.Data(), compiledBlob->GetBufferPointer(), blobSize);

	if (ShaderStageIsRayTracing(shaderStage)) {
		return ValidateRayTracingShader(compiledBlob, entryPoint);
	}

	return true;
}

bool CEDXShaderCompiler::InternalGetReflection(const ComPtr<IDxcBlob>& shaderBlob, REFIID IID, void** ppvObject) {
	HRESULT result = DXReflection->Load(shaderBlob.Get());
	if (FAILED(result)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Were not able to validate Ray Tracing shader");
		return false;
	}

	uint32 partIndex;
	result = DXReflection->FindFirstPartKind(DFCC_DXIL, &partIndex);
	if (FAILED(result)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Were not able to validate Ray Tracing shader");
		return false;
	}

	result = DXReflection->GetPartReflection(partIndex, IID, ppvObject);
	if (FAILED(result)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Were not able to validate Ray Tracing shader");
		return false;
	}

	return true;
}

bool CEDXShaderCompiler::ValidateRayTracingShader(const ComPtr<IDxcBlob>& shaderBlob, LPCWSTR entryPoint) {
	Microsoft::WRL::ComPtr<ID3D12LibraryReflection> libraryReflection;
	if (!InternalGetReflection(shaderBlob, IID_PPV_ARGS(&libraryReflection))) {
		return false;
	}

	D3D12_LIBRARY_DESC libraryDesc;
	Memory::CEMemory::Memzero(&libraryDesc);

	HRESULT hResult = libraryReflection->GetDesc(&libraryDesc);
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Were not able to validate Ray Tracing shader");
		return false;
	}

	Assert(libraryDesc.FunctionCount > 0);

	ID3D12FunctionReflection* function = libraryReflection->GetFunctionByIndex(0);

	D3D12_FUNCTION_DESC funcDesc;
	Memory::CEMemory::Memzero(&funcDesc);

	hResult = function->GetDesc(&funcDesc);
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXShaderCompiler]: Were not able to validate Ray Tracing shader");
		return false;
	}

	char buffer[256];
	Memory::CEMemory::Memzero(buffer, sizeof(buffer));

	size_t convertedChars;
	wcstombs_s(&convertedChars, buffer, 256, entryPoint, _TRUNCATE);

	std::string funcName(funcDesc.Name);
	auto result = funcName.find(buffer);
	if (result == std::string::npos) {
		CE_LOG_ERROR(
			"[CEDXShaderCompiler]: first exported function does not have correct entrypoint '" + std::string(buffer) +
			"'. Name=" + funcName);
		return false;
	}

	return false;
}
