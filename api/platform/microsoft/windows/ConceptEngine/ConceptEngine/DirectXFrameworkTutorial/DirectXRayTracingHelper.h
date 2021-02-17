#pragma once
#include "DirectXHelper.h"

#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

struct AccelerationStructureBuffers {
	wrl::ComPtr<ID3D12Resource> scratch;
	wrl::ComPtr<ID3D12Resource> accelerationStructure;
	wrl::ComPtr<ID3D12Resource> instanceDesc;
	UINT64 ResultDataMaxSizeInBytes;
};

class ShaderRecord {
public:
	ShaderRecord(
		void* pShaderIdentifier,
		UINT shaderIdentifierSize
	) :
		shaderIdentifier(pShaderIdentifier, shaderIdentifierSize) {

	}

	ShaderRecord(
		void* pShaderIdentifier,
		UINT shaderIdentifierSize,
		void* pLocalRootArguments,
		UINT localRootArgumentsSize
	):
		shaderIdentifier(pShaderIdentifier, shaderIdentifierSize),
		localRootArguments(pLocalRootArguments, localRootArgumentsSize) {

	}

	void CopyTo(void* dest) const {
		uint8_t* byteDest = static_cast<uint8_t*>(dest);
		memcpy(byteDest, shaderIdentifier.ptr, shaderIdentifier.size);
		if (localRootArguments.ptr) {
			memcpy(byteDest + shaderIdentifier.size, localRootArguments.ptr, localRootArguments.size);
		}
	}

	struct PointerWithSize {
		void* ptr;
		UINT size;

		PointerWithSize() : ptr(nullptr), size(0) {
		}

		PointerWithSize(void* _ptr, UINT _size) : ptr(_ptr), size(_size) {

		}

	};

	PointerWithSize shaderIdentifier;
	PointerWithSize localRootArguments;

protected:
private:
};

class ShaderTable : public GpuUploadBuffer {
	uint8_t* m_mappedShaderRecords;
	UINT m_shaderRecordSize;

	//Debug Support
	std::wstring m_name;
	std::vector<ShaderRecord> m_shaderRecords;

	ShaderTable() {
	}

public:
	ShaderTable(ID3D12Device* device,
	            UINT numShaderRecords,
	            UINT shaderRecordSize,
	            LPCWSTR resourceName = nullptr) : m_name(resourceName) {
		m_shaderRecordSize = Align(shaderRecordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
		m_shaderRecords.reserve(numShaderRecords);
		UINT bufferSize = numShaderRecords * m_shaderRecordSize;
		Allocate(device, bufferSize, resourceName);
		m_mappedShaderRecords = MapCpuWriteOnly();
	}

	void push_back(const ShaderRecord& shaderRecord) {
		ThrowIfFalse(m_shaderRecords.size() < m_shaderRecords.capacity());
		m_shaderRecords.push_back(shaderRecord);
		shaderRecord.CopyTo(m_mappedShaderRecords);
		m_mappedShaderRecords += m_shaderRecordSize;
	}

	UINT GetShaderRecordSize() {
		return m_shaderRecordSize;
	}

	void DebugPrint(std::unordered_map<void*, std::wstring> shaderIdToStringMap) {
		std::wstringstream wstr;
		wstr << L"|--------------------------------------------------------------------\n";
		wstr << L"|Shader table - " << m_name.c_str() << L": "
			<< m_shaderRecordSize << L" | "
			<< m_shaderRecords.size() * m_shaderRecordSize << L" bytes\n";

		for (UINT i = 0; i < m_shaderRecords.size(); i++)
		{
			wstr << L"| [" << i << L"]: ";
			wstr << shaderIdToStringMap[m_shaderRecords[i].shaderIdentifier.ptr] << L", ";
			wstr << m_shaderRecords[i].shaderIdentifier.size << L" + " << m_shaderRecords[i].localRootArguments.size << L" bytes \n";
		}
		wstr << L"|--------------------------------------------------------------------\n";
		wstr << L"\n";
		OutputDebugStringW(wstr.str().c_str());
	}

protected:
private:
};
