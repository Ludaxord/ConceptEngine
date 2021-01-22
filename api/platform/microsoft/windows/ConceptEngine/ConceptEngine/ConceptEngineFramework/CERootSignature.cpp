#include "CERootSignature.h"

#include <cassert>


#include "CEDevice.h"
#include "CEHelper.h"
#include "d3dx12.h"
using namespace Concept::GraphicsEngine::Direct3D;

uint32_t CERootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const {
	uint32_t descriptorTableBitMask = 0;
	switch (descriptorHeapType) {
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		descriptorTableBitMask = m_descriptorTableBitMask;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		descriptorTableBitMask = m_samplerTableBitMask;
		break;
	}

	return descriptorTableBitMask;
}

uint32_t CERootSignature::GetNumDescriptors(uint32_t rootIndex) const {
	assert(rootIndex > 0);
	return m_numDescriptorsPerTable[rootIndex];
}

CERootSignature::CERootSignature(CEDevice& device,
                                 const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc) :
	m_device(device),
	m_rootSignatureDesc{},
	m_numDescriptorsPerTable{0},
	m_samplerTableBitMask(0),
	m_descriptorTableBitMask(0) {
	SetRootSignatureDesc(rootSignatureDesc);
}

CERootSignature::~CERootSignature() {
	Destroy();
}

void CERootSignature::Destroy() {
	for (UINT i = 0; i < m_rootSignatureDesc.NumParameters; ++i) {
		const D3D12_ROOT_PARAMETER1& rootParameter = m_rootSignatureDesc.pParameters[i];
		if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			delete[] rootParameter.DescriptorTable.pDescriptorRanges;
		}
	}

	delete[] m_rootSignatureDesc.pParameters;
	m_rootSignatureDesc.pParameters = nullptr;
	m_rootSignatureDesc.NumParameters = 0;

	delete[] m_rootSignatureDesc.pStaticSamplers;
	m_rootSignatureDesc.pStaticSamplers = nullptr;
	m_rootSignatureDesc.NumStaticSamplers = 0;

	m_descriptorTableBitMask = 0;
	m_samplerTableBitMask = 0;

	memset(m_numDescriptorsPerTable, 0, sizeof(m_numDescriptorsPerTable));
}

void CERootSignature::SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc) {
	/*
	 * Make sure any previously allocated root signature description is cleaned
	 * up first.
	 */
	Destroy();

	UINT numParameter = rootSignatureDesc.NumParameters;
	D3D12_ROOT_PARAMETER1* pParameters = numParameter > 0 ? new D3D12_ROOT_PARAMETER1[numParameter] : nullptr;

	for (UINT i = 0; i < numParameter; ++i) {
		const D3D12_ROOT_PARAMETER1& rootParameter = rootSignatureDesc.pParameters[i];
		pParameters[i] = rootParameter;

		if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			UINT numDescriptorRanges = rootParameter.DescriptorTable.NumDescriptorRanges;
			D3D12_DESCRIPTOR_RANGE1* pDescriptorRanges = numDescriptorRanges > 0
				                                             ? new D3D12_DESCRIPTOR_RANGE1[numDescriptorRanges]
				                                             : nullptr;
			memcpy(pDescriptorRanges, rootParameter.DescriptorTable.pDescriptorRanges,
			       sizeof(D3D12_DESCRIPTOR_RANGE1) * numDescriptorRanges);

			pParameters[i].DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
			pParameters[i].DescriptorTable.pDescriptorRanges = pDescriptorRanges;

			/*
			 * Set bit mask depending on type of descriptor table 
			 */
			if (numDescriptorRanges > 0) {
				switch (pDescriptorRanges[0].RangeType) {
				case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
				case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
				case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
					m_descriptorTableBitMask |= (1 << i);
					break;
				case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
					m_samplerTableBitMask |= (1 << i);
					break;
				}
			}

			/*
			 * Count number of descriptors in descriptor table.
			 */
			for (UINT j = 0; j < numDescriptorRanges; ++j) {
				m_numDescriptorsPerTable[i] += pDescriptorRanges[j].NumDescriptors;
			}
		}
	}

	m_rootSignatureDesc.NumParameters = numParameter;
	m_rootSignatureDesc.pParameters = pParameters;

	UINT numStaticSamplers = rootSignatureDesc.NumStaticSamplers;
	D3D12_STATIC_SAMPLER_DESC* pStaticSampler = numStaticSamplers > 0
		                                            ? new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers]
		                                            : nullptr;
	if (pStaticSampler) {
		memcpy(pStaticSampler, rootSignatureDesc.pStaticSamplers,
		       sizeof(D3D12_STATIC_SAMPLER_DESC) * numStaticSamplers);
	}

	m_rootSignatureDesc.NumStaticSamplers = numStaticSamplers;
	m_rootSignatureDesc.pStaticSamplers = pStaticSampler;

	D3D12_ROOT_SIGNATURE_FLAGS flags = rootSignatureDesc.Flags;
	m_rootSignatureDesc.Flags = flags;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionRootSignatureDesc;
	versionRootSignatureDesc.Init_1_1(numParameter, pParameters, numStaticSamplers, pStaticSampler, flags);

	D3D_ROOT_SIGNATURE_VERSION highestVersion = m_device.GetHighestRootSignatureVersion();

	/*
	 * Serialize root signature
	 */
	wrl::ComPtr<ID3DBlob> rootSignatureBlob;
	wrl::ComPtr<ID3DBlob> errorBlob;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&versionRootSignatureDesc, highestVersion, &rootSignatureBlob,
	                                                    &errorBlob));

	auto d3d12Device = m_device.GetDevice();
	/*
	 * Create root signature
	 */
	ThrowIfFailed(d3d12Device->CreateRootSignature(0,
	                                               rootSignatureBlob->GetBufferPointer(),
	                                               rootSignatureBlob->GetBufferSize(),
	                                               IID_PPV_ARGS(&m_rootSignature)));
}
