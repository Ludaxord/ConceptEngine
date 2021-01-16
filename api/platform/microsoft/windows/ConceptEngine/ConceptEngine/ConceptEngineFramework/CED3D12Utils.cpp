#include "CED3D12Utils.h"

#include <d3dcompiler.h>
#include <fstream>


#include "CEHelper.h"
#include "d3dx12.h"

ID3DBlob* CED3D12Utils::LoadBinary(const std::wstring& filename) {
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);

	ID3DBlob* blob;
	ThrowIfFailed(D3DCreateBlob(size, &blob));

	fin.read((char*)blob->GetBufferPointer(), size);
	fin.close();

	return blob;
}

ID3D12Resource* CED3D12Utils::CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
                                                  const void* initData, UINT64 byteSize,
                                                  Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer) {
	ID3D12Resource* defaultBuffer = nullptr;
	ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	                                              D3D12_HEAP_FLAG_NONE,
	                                              &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
	                                              D3D12_RESOURCE_STATE_COMMON,
	                                              nullptr,
	                                              IID_PPV_ARGS(&defaultBuffer)));
	// In order to copy CPU memory data into our default buffer, we need to create
	// an intermediate upload heap. 
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


	// Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to mBuffer.
	OutputDebugStringW(L"Closed CMD List 1\n");
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer,
	                                                                  D3D12_RESOURCE_STATE_COMMON,
	                                                                  D3D12_RESOURCE_STATE_COPY_DEST));
	OutputDebugStringW(L"Closed CMD List 2\n");
	UpdateSubresources<1>(cmdList, defaultBuffer, uploadBuffer.Get(), 0, 0, 1, &subResourceData);
	OutputDebugStringW(L"Closed CMD List 3\n");
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer,
	                                                                  D3D12_RESOURCE_STATE_COPY_DEST,
	                                                                  D3D12_RESOURCE_STATE_GENERIC_READ));
	OutputDebugStringW(L"Closed CMD List 4\n");
	// Note: uploadBuffer has to be kept alive after the above function calls because
	// the command list has not been executed yet that performs the actual copy.
	// The caller can Release the uploadBuffer after it knows the copy has been executed.


	return defaultBuffer;
}

ID3DBlob* CED3D12Utils::CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target) {
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
	                        entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);
	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());
	ThrowIfFailed(hr);

	return byteCode;
}
