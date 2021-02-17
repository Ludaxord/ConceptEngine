#pragma once
#include <exception>
#include <iomanip>
#include <spdlog/spdlog.h>
#include <sstream>
#include <d3d12.h>
#include "d3dx12.h"

namespace wrl = Microsoft::WRL;

class HrException : public std::runtime_error {
	inline std::string HrToString(HRESULT hr) {
		char s_string[64] = {};
		sprintf_s(s_string, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_string);
	}

public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {
		spdlog::error(HrToString(hr));
	}

	HRESULT Error() const { return m_hr; }
protected:
private:
	const HRESULT m_hr;
};

inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize) {
	if (path == nullptr) {
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, path, pathSize);
	if (size == 0 || size == pathSize) {
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash) {
		*(lastSlash + 1) = L'\0';
	}
};

inline void ThrowIfFailed(HRESULT hr, const wchar_t* message) {
	if (FAILED(hr)) {
		OutputDebugString(message);
		std::wstring wsMessage = message;
		const std::string sMessage(wsMessage.begin(), wsMessage.end());
		spdlog::error(sMessage);
		throw HrException(hr);
	}
}

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		throw HrException(hr);
	}
}

inline void ThrowIfFalse(bool value) {
	ThrowIfFailed(value ? S_OK : E_FAIL);
}

inline void ThrowIfFalse(bool value, const wchar_t* message) {
	ThrowIfFailed(value ? S_OK : E_FAIL, message);
}

inline DXGI_FORMAT NoSRGB(DXGI_FORMAT format) {
	switch (format) {
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8X8_UNORM;
	default: return format;
	}
}

inline void AllocateUploadBuffer(ID3D12Device* pDevice, void* pData, UINT64 datasize, ID3D12Resource** ppResource,
                                 const wchar_t* resourceName = nullptr) {
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(datasize);
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(ppResource)));
	if (resourceName) {
		(*ppResource)->SetName(resourceName);
	}
	void* pMappedData;
	(*ppResource)->Map(0, nullptr, &pMappedData);
	memcpy(pMappedData, pData, datasize);
	(*ppResource)->Unmap(0, nullptr);
}

inline void AllocateUAVBuffer(ID3D12Device* pDevice, UINT64 bufferSize, ID3D12Resource** ppResource,
                              D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON,
                              const wchar_t* resourceName = nullptr) {
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		initialResourceState,
		nullptr,
		IID_PPV_ARGS(ppResource)));
	if (resourceName) {
		(*ppResource)->SetName(resourceName);
	}
}

template <class T, size_t N>
void DefineExports(T* obj, LPCWSTR (&Exports)[N]) {
	for (UINT i = 0; i < N; i++) {
		obj->DefineExport(Exports[i]);
	}
}

template <class T, size_t N, size_t M>
void DefineExports(T* obj, LPCWSTR (&Exports)[N][M]) {
	for (UINT i = 0; i < N; i++)
		for (UINT j = 0; j < M; j++) {
			obj->DefineExport(Exports[i][j]);
		}
}

// Pretty-print a state object tree.
inline void PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc) {
	std::wstringstream wstr;
	wstr << L"\n";
	wstr << L"--------------------------------------------------------------------\n";
	wstr << L"| D3D12 State Object 0x" << static_cast<const void*>(desc) << L": ";
	if (desc->Type == D3D12_STATE_OBJECT_TYPE_COLLECTION) wstr << L"Collection\n";
	if (desc->Type == D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE) wstr << L"Raytracing Pipeline\n";

	auto ExportTree = [](UINT depth, UINT numExports, const D3D12_EXPORT_DESC* exports) {
		std::wostringstream woss;
		for (UINT i = 0; i < numExports; i++) {
			woss << L"|";
			if (depth > 0) {
				for (UINT j = 0; j < 2 * depth - 1; j++) woss << L" ";
			}
			woss << L" [" << i << L"]: ";
			if (exports[i].ExportToRename) woss << exports[i].ExportToRename << L" --> ";
			woss << exports[i].Name << L"\n";
		}
		return woss.str();
	};

	for (UINT i = 0; i < desc->NumSubobjects; i++) {
		wstr << L"| [" << i << L"]: ";
		switch (desc->pSubobjects[i].Type) {
		case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
			wstr << L"Global Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
			break;
		case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
			wstr << L"Local Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
			break;
		case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
			wstr << L"Node Mask: 0x" << std::hex << std::setfill(L'0') << std::setw(8) << *static_cast<const UINT*>(desc
				->pSubobjects[i].pDesc) << std::setw(0) << std::dec << L"\n";
			break;
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY: {
			wstr << L"DXIL Library 0x";
			auto lib = static_cast<const D3D12_DXIL_LIBRARY_DESC*>(desc->pSubobjects[i].pDesc);
			wstr << lib->DXILLibrary.pShaderBytecode << L", " << lib->DXILLibrary.BytecodeLength << L" bytes\n";
			wstr << ExportTree(1, lib->NumExports, lib->pExports);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION: {
			wstr << L"Existing Library 0x";
			auto collection = static_cast<const D3D12_EXISTING_COLLECTION_DESC*>(desc->pSubobjects[i].pDesc);
			wstr << collection->pExistingCollection << L"\n";
			wstr << ExportTree(1, collection->NumExports, collection->pExports);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION: {
			wstr << L"Subobject to Exports Association (Subobject [";
			auto association = static_cast<const D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
			UINT index = static_cast<UINT>(association->pSubobjectToAssociate - desc->pSubobjects);
			wstr << index << L"])\n";
			for (UINT j = 0; j < association->NumExports; j++) {
				wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION: {
			wstr << L"DXIL Subobjects to Exports Association (";
			auto association = static_cast<const D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].
				pDesc);
			wstr << association->SubobjectToAssociate << L")\n";
			for (UINT j = 0; j < association->NumExports; j++) {
				wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG: {
			wstr << L"Raytracing Shader Config\n";
			auto config = static_cast<const D3D12_RAYTRACING_SHADER_CONFIG*>(desc->pSubobjects[i].pDesc);
			wstr << L"|  [0]: Max Payload Size: " << config->MaxPayloadSizeInBytes << L" bytes\n";
			wstr << L"|  [1]: Max Attribute Size: " << config->MaxAttributeSizeInBytes << L" bytes\n";
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG: {
			wstr << L"Raytracing Pipeline Config\n";
			auto config = static_cast<const D3D12_RAYTRACING_PIPELINE_CONFIG*>(desc->pSubobjects[i].pDesc);
			wstr << L"|  [0]: Max Recursion Depth: " << config->MaxTraceRecursionDepth << L"\n";
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP: {
			wstr << L"Hit Group (";
			auto hitGroup = static_cast<const D3D12_HIT_GROUP_DESC*>(desc->pSubobjects[i].pDesc);
			wstr << (hitGroup->HitGroupExport ? hitGroup->HitGroupExport : L"[none]") << L")\n";
			wstr << L"|  [0]: Any Hit Import: " << (hitGroup->AnyHitShaderImport
				                                        ? hitGroup->AnyHitShaderImport
				                                        : L"[none]") << L"\n";
			wstr << L"|  [1]: Closest Hit Import: " << (hitGroup->ClosestHitShaderImport
				                                            ? hitGroup->ClosestHitShaderImport
				                                            : L"[none]") << L"\n";
			wstr << L"|  [2]: Intersection Import: " << (hitGroup->IntersectionShaderImport
				                                             ? hitGroup->IntersectionShaderImport
				                                             : L"[none]") << L"\n";
			break;
		}
		}
		wstr << L"|--------------------------------------------------------------------\n";
	}
	wstr << L"\n";
	OutputDebugStringW(wstr.str().c_str());
}

inline HRESULT ReadDataFromFile(LPCWSTR filename, BYTE** data, UINT* size)
{
	using namespace Microsoft::WRL;

	CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
	extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
	extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
	extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
	extendedParams.lpSecurityAttributes = nullptr;
	extendedParams.hTemplateFile = nullptr;

	Wrappers::FileHandle file(CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));
	if (file.Get() == INVALID_HANDLE_VALUE)
	{
		throw std::exception();
	}

	FILE_STANDARD_INFO fileInfo = {};
	if (!GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
	{
		throw std::exception();
	}

	if (fileInfo.EndOfFile.HighPart != 0)
	{
		throw std::exception();
	}

	*data = reinterpret_cast<BYTE*>(malloc(fileInfo.EndOfFile.LowPart));
	*size = fileInfo.EndOfFile.LowPart;

	if (!ReadFile(file.Get(), *data, fileInfo.EndOfFile.LowPart, nullptr, nullptr))
	{
		throw std::exception();
	}

	return S_OK;
}

// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
	pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
	WCHAR fullName[50];
	if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
	{
		pObject->SetName(fullName);
	}
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

inline UINT Align(UINT size, UINT alignment)
{
	return (size + (alignment - 1)) & ~(alignment - 1);
}

inline UINT CalculateConstantBufferByteSize(UINT byteSize)
{
	// Constant buffer size is required to be aligned.
	return Align(byteSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
}

#ifdef D3D_COMPILE_STANDARD_FILE_INCLUDE
inline Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(_DEBUG) || defined(DBG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	return byteCode;
}
#endif

// Resets all elements in a ComPtr array.
template<class T>
void ResetComPtrArray(T* comPtrArray)
{
	for (auto& i : *comPtrArray)
	{
		i.Reset();
	}
}


// Resets all elements in a unique_ptr array.
template<class T>
void ResetUniquePtrArray(T* uniquePtrArray)
{
	for (auto& i : *uniquePtrArray)
	{
		i.reset();
	}
}

class GpuUploadBuffer
{
public:
	wrl::ComPtr<ID3D12Resource> GetResource() { return m_resource; }

protected:
	wrl::ComPtr<ID3D12Resource> m_resource;

	GpuUploadBuffer() {}
	~GpuUploadBuffer()
	{
		if (m_resource.Get())
		{
			m_resource->Unmap(0, nullptr);
		}
	}

	void Allocate(ID3D12Device* device, UINT bufferSize, LPCWSTR resourceName = nullptr)
	{
		auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_resource)));
		m_resource->SetName(resourceName);
	}

	uint8_t* MapCpuWriteOnly()
	{
		uint8_t* mappedData;
		// We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)));
		return mappedData;
	}
};

struct D3DBuffer
{
	wrl::ComPtr<ID3D12Resource> resource;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
};

// Helper class to create and update a constant buffer with proper constant buffer alignments.
// Usage: ToDo
//    ConstantBuffer<...> cb;
//    cb.Create(...);
//    cb.staging.var = ...; | cb->var = ... ; 
//    cb.CopyStagingToGPU(...);
template <class T>
class ConstantBuffer : public GpuUploadBuffer
{
	uint8_t* m_mappedConstantData;
	UINT m_alignedInstanceSize;
	UINT m_numInstances;

public:
	ConstantBuffer() : m_alignedInstanceSize(0), m_numInstances(0), m_mappedConstantData(nullptr) {}

	void Create(ID3D12Device* device, UINT numInstances = 1, LPCWSTR resourceName = nullptr)
	{
		m_numInstances = numInstances;
		UINT alignedSize = Align(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		UINT bufferSize = numInstances * alignedSize;
		Allocate(device, bufferSize, resourceName);
		m_mappedConstantData = MapCpuWriteOnly();
	}

	void CopyStagingToGpu(UINT instanceIndex = 0)
	{
		memcpy(m_mappedConstantData + instanceIndex * m_alignedInstanceSize, &staging, sizeof(T));
	}

	// Accessors
	T staging;
	T* operator->() { return &staging; }
	UINT NumInstances() { return m_numInstances; }
	D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress(UINT instanceIndex = 0)
	{
		return m_resource->GetGPUVirtualAddress() + instanceIndex * m_alignedInstanceSize;
	}
};


// Helper class to create and update a structured buffer.
// Usage: ToDo
//    ConstantBuffer<...> cb;
//    cb.Create(...);
//    cb.staging.var = ...; | cb->var = ... ; 
//    cb.CopyStagingToGPU(...);
template <class T>
class StructuredBuffer : public GpuUploadBuffer
{
	T* m_mappedBuffers;
	std::vector<T> m_staging;
	UINT m_numInstances;

public:
	// Performance tip: Align structures on sizeof(float4) boundary.
	// Ref: https://developer.nvidia.com/content/understanding-structured-buffer-performance
	static_assert(sizeof(T) % 16 == 0, L"Align structure buffers on 16 byte boundary for performance reasons.");

	StructuredBuffer() : m_mappedBuffers(nullptr), m_numInstances(0) {}

	void Create(ID3D12Device* device, UINT numElements, UINT numInstances = 1, LPCWSTR resourceName = nullptr)
	{
		m_staging.resize(numElements);
		UINT bufferSize = numInstances * numElements * sizeof(T);
		Allocate(device, bufferSize, resourceName);
		m_mappedBuffers = reinterpret_cast<T*>(MapCpuWriteOnly());
	}

	void CopyStagingToGpu(UINT instanceIndex = 0)
	{
		memcpy(m_mappedBuffers + instanceIndex, &m_staging[0], InstanceSize());
	}

	// Accessors
	T& operator[](UINT elementIndex) { return m_staging[elementIndex]; }
	size_t NumElementsPerInstance() { return m_staging.size(); }
	UINT NumInstances() { return m_staging.size(); }
	size_t InstanceSize() { return NumElementsPerInstance() * sizeof(T); }
	D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress(UINT instanceIndex = 0)
	{
		return m_resource->GetGPUVirtualAddress() + instanceIndex * InstanceSize();
	}
};