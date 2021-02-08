#include "CEDXILLibrary.h"

Microsoft::WRL::ComPtr<ID3DBlob> CEDXIlLibrary::CreateLibrary(std::wstring fileName, std::wstring target) {
	ThrowIfFailed(gDxcDllHelper.Initialize());

	IDxcCompiler* pCompiler;
	IDxcLibrary* pLibrary;

	ThrowIfFailed(gDxcDllHelper.CreateInstance(CLSID_DxcCompiler, &pCompiler));
	ThrowIfFailed(gDxcDllHelper.CreateInstance(CLSID_DxcLibrary, &pLibrary));

	// Open and read the file
	std::ifstream shaderFile(fileName);
	if (shaderFile.good() == false) {
		std::string s(fileName.begin(), fileName.end());
		spdlog::error("Cannot open file {}", s);
		return nullptr;
	}

	std::stringstream ss;
	ss << shaderFile.rdbuf();
	std::string shader = ss.str();

	//Create blob for string
	wrl::ComPtr<IDxcBlobEncoding> pTextBlob;
	ThrowIfFailed(
		pLibrary->CreateBlobWithEncodingFromPinned((LPBYTE)shader.c_str(), (uint32_t)shader.size(), 0,
		                                           &pTextBlob));

	//Compile
	wrl::ComPtr<IDxcOperationResult> pResult;
	ThrowIfFailed(pCompiler->Compile(pTextBlob.Get(), fileName.c_str(), L"", target.c_str(), nullptr, 0,
	                                 nullptr, 0, nullptr, &pResult));

	//Verify result
	HRESULT resCode;
	ThrowIfFailed(pResult->GetStatus(&resCode));
	if (FAILED(resCode)) {
		wrl::ComPtr<IDxcBlobEncoding> pError;
		ThrowIfFailed(pResult->GetErrorBuffer(&pError));
		std::string log = convertBlobToString(pError.Get());
		spdlog::error("Compiler error: {}", log);
		return nullptr;
	}

	wrl::ComPtr<IDxcBlob> pBlob;
	wrl::ComPtr<ID3DBlob> blob;
	ThrowIfFailed(pResult->GetResult(&pBlob));

	pBlob.As(&blob);
	return blob;
}

CEDXIlLibrary::CEDXIlLibrary(Microsoft::WRL::ComPtr<ID3DBlob> pBlob,
                             const WCHAR* entryPoint[],
                             uint32_t entryPointCount) : pShaderBlob(pBlob) {
	stateSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	stateSubObject.pDesc = &dxilLibDesc;

	dxilLibDesc = {};
	exportDesc.resize(entryPointCount);
	exportName.resize(entryPointCount);

	if (pBlob) {
		dxilLibDesc.DXILLibrary.pShaderBytecode = pBlob->GetBufferPointer();
		dxilLibDesc.DXILLibrary.BytecodeLength = pBlob->GetBufferSize();
		dxilLibDesc.NumExports = entryPointCount;
		dxilLibDesc.pExports = exportDesc.data();
		for (uint32_t i = 0; i < entryPointCount; i++) {
			exportName[i] = entryPoint[i];
			exportDesc[i].Name = exportName[i].c_str();
			exportDesc[i].Flags = D3D12_EXPORT_FLAG_NONE;
			exportDesc[i].ExportToRename = nullptr;
		}
	}
}
