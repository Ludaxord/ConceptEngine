#include "CEDynamicDescriptorHeap.h"

#include <cassert>
#include <stdexcept>


#include "CECommandList.h"
#include "CEDevice.h"
#include "CEHelper.h"
#include "CERootSignature.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEDynamicDescriptorHeap::CEDynamicDescriptorHeap(CEDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType,
                                                 uint32_t numDescriptorsPerHeap): m_device(device),
	m_descriptorHeapType(heapType),
	m_numDescriptorsPerHeap(numDescriptorsPerHeap),
	m_descriptorTableBitMask(0),
	m_staleDescriptorTableBitMask(0),
	m_staleCBVBitMask(0),
	m_staleSRVBitMask(0),
	m_staleUAVBitMask(0),
	m_currentCPUDescriptorHandle(D3D12_DEFAULT),
	m_currentGPUDescriptorHandle(D3D12_DEFAULT),
	m_numFreeHandles(0) {
	m_descriptorHandleIncrementSize = m_device.GetDescriptorHandleIncrementSize(heapType);
	/*
	 * Allocate space for staging CPU visible descriptors
	 */
	m_descriptorHandleCache = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(m_numDescriptorsPerHeap);
}

CEDynamicDescriptorHeap::~CEDynamicDescriptorHeap() {
}

void CEDynamicDescriptorHeap::StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors,
                                               const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors) {
	/*
	 * Cannot stage more than maximum number of descriptors per heap.
	 * Cannot stage more than MaxDescriptorTables root parameters.
	 */
	if (numDescriptors > m_numDescriptorsPerHeap || rootParameterIndex >= MaxDescriptorTables) {
		throw std::bad_alloc();
	}

	DescriptorTableCache& descriptorTableCache = m_descriptorTableCache[rootParameterIndex];

	/*
	 * Check that number of descriptors to copy does not exceed number of descriptors expected in descriptor table.
	 */
	if ((offset + numDescriptors) > descriptorTableCache.NumDescriptors) {
		throw std::length_error("Number of descriptors exceeds the number of descriptors in the descriptor table.");
	}

	D3D12_CPU_DESCRIPTOR_HANDLE* dstDescriptor = (descriptorTableCache.BaseDescriptor + offset);
	for (uint32_t i = 0; i < numDescriptors; ++i) {
		dstDescriptor[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srcDescriptors, i, m_descriptorHandleIncrementSize);
	}

	/*
	 * Set root parameter index bit to make sure descriptor table
	 * at index is bound command list
	 */
	m_staleDescriptorTableBitMask |= (1 << rootParameterIndex);
}

void CEDynamicDescriptorHeap::StageInlineCBV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation) {
	assert(rootParameterIndex < MaxDescriptorTables);
	m_inlineCBV[rootParameterIndex] = bufferLocation;
	m_staleCBVBitMask |= (1 << rootParameterIndex);
}

void CEDynamicDescriptorHeap::StageInlineSRV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation) {
	assert(rootParameterIndex < MaxDescriptorTables);
	m_inlineSRV[rootParameterIndex] = bufferLocation;
	m_staleSRVBitMask |= (1 << rootParameterIndex);
}

void CEDynamicDescriptorHeap::StageInlineUAV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation) {
	assert(rootParameterIndex < MaxDescriptorTables);
	m_inlineUAV[rootParameterIndex] = bufferLocation;
	m_staleUAVBitMask |= (1 << rootParameterIndex);
}

void CEDynamicDescriptorHeap::CommitStagedDescriptorsForDraw(CECommandList& commandList) {
	CommitDescriptorTables(commandList, &ID3D12GraphicsCommandList5::SetGraphicsRootDescriptorTable);
	CommitInlineDescriptors(commandList, m_inlineCBV, m_staleCBVBitMask,
	                        &ID3D12GraphicsCommandList5::SetGraphicsRootConstantBufferView);
	CommitInlineDescriptors(commandList, m_inlineSRV, m_staleSRVBitMask,
	                        &ID3D12GraphicsCommandList5::SetGraphicsRootUnorderedAccessView);
	CommitInlineDescriptors(commandList, m_inlineUAV, m_staleUAVBitMask,
	                        &ID3D12GraphicsCommandList5::SetGraphicsRootUnorderedAccessView);
}

void CEDynamicDescriptorHeap::CommitStagedDescriptorsForDispatch(CECommandList& commandList) {
	CommitDescriptorTables(commandList, &ID3D12GraphicsCommandList5::SetComputeRootDescriptorTable);
	CommitInlineDescriptors(commandList, m_inlineCBV, m_staleCBVBitMask,
	                        &ID3D12GraphicsCommandList5::SetComputeRootConstantBufferView);
	CommitInlineDescriptors(commandList, m_inlineSRV, m_staleSRVBitMask,
	                        &ID3D12GraphicsCommandList5::SetComputeRootShaderResourceView);
	CommitInlineDescriptors(commandList, m_inlineUAV, m_staleUAVBitMask,
	                        &ID3D12GraphicsCommandList5::SetComputeRootUnorderedAccessView);
}

D3D12_GPU_DESCRIPTOR_HANDLE CEDynamicDescriptorHeap::CopyDescriptor(CECommandList& commandList,
                                                                    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor) {
	if (!m_currentDescriptorHeap || m_numFreeHandles < 1) {
		m_currentDescriptorHeap = RequestDescriptorHeap();
		m_currentCPUDescriptorHandle = m_currentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_currentGPUDescriptorHandle = m_currentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		m_numFreeHandles = m_numDescriptorsPerHeap;

		commandList.SetDescriptorHeap(m_descriptorHeapType, m_currentDescriptorHeap.Get());

		/*
		 * When updating descriptor heap on command list, all descriptor
		 * tables must be (re)copied to new descriptor heap (not just stale descriptor tables).
		 */
		m_staleDescriptorTableBitMask = m_descriptorTableBitMask;
	}

	auto d3d12Device = m_device.GetDevice();

	D3D12_GPU_DESCRIPTOR_HANDLE hGPU = m_currentGPUDescriptorHandle;
	d3d12Device->CopyDescriptorsSimple(1, m_currentCPUDescriptorHandle, cpuDescriptor, m_descriptorHeapType);

	m_currentCPUDescriptorHandle.Offset(1, m_descriptorHandleIncrementSize);
	m_currentGPUDescriptorHandle.Offset(1, m_descriptorHandleIncrementSize);
	m_numFreeHandles -= 1;

	return hGPU;
}

void CEDynamicDescriptorHeap::ParseRootSignature(const std::shared_ptr<CERootSignature>& rootSignature) {
	assert(rootSignature);

	/*
	 * If root signature changes, all descriptors must be (re)bound to command list
	 */
	const auto& rootSignatureDesc = rootSignature->GetRootSignatureDesc();

	/*
	 * Get bit mask that represents root parameter indices that match descriptor heap type for this dynamic heap
	 */
	m_descriptorTableBitMask = rootSignature->GetDescriptorTableBitMask(m_descriptorHeapType);
	uint32_t descriptorTableBitMask = m_descriptorTableBitMask;

	uint32_t currentOffset = 0;
	DWORD rootIndex;
	while (_BitScanForward(&rootIndex, descriptorTableBitMask) && rootIndex < rootSignatureDesc.NumParameters) {
		uint32_t numDescriptors = rootSignature->GetNumDescriptors(rootIndex);

		DescriptorTableCache& descriptorTableCache = m_descriptorTableCache[rootIndex];
		descriptorTableCache.NumDescriptors = numDescriptors;
		descriptorTableCache.BaseDescriptor = m_descriptorHandleCache.get() + currentOffset;

		currentOffset += numDescriptors;

		/*
		 * Flip descriptor table bit so it is not scanned again for current index
		 */
		descriptorTableBitMask ^= (1 << rootIndex);
	}

	/*
	 * Make sure maximum number of descriptors per descriptor heap has not been exceeded
	 */
	assert(
		currentOffset <= m_numDescriptorsPerHeap &&
		"Root signature requires more than maximum number of descriptors per descriptor heap. Consider increasing the maximum number of descriptors per descriptor heap.");
}

void CEDynamicDescriptorHeap::Reset() {
	m_availableDescriptorHeaps = m_descriptorHeapPool;
	m_currentDescriptorHeap.Reset();
	m_currentCPUDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
	m_currentGPUDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
	m_numFreeHandles = 0;
	m_descriptorTableBitMask = 0;
	m_staleDescriptorTableBitMask = 0;
	m_staleCBVBitMask = 0;
	m_staleSRVBitMask = 0;
	m_staleUAVBitMask = 0;

	/*
	 * Reset descriptor cache 
	 */
	for (int i = 0; i < MaxDescriptorTables; ++i) {
		m_descriptorTableCache[i].Reset();
		m_inlineCBV[i] = 0ull;
		m_inlineSRV[i] = 0ull;
		m_inlineUAV[i] = 0ull;
	}
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CEDynamicDescriptorHeap::RequestDescriptorHeap() {
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	if (!m_availableDescriptorHeaps.empty()) {
		descriptorHeap = m_availableDescriptorHeaps.front();
		m_availableDescriptorHeaps.pop();
	}
	else {
		descriptorHeap = CreateDescriptorHeap();
		m_descriptorHeapPool.push(descriptorHeap);
	}

	return descriptorHeap;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CEDynamicDescriptorHeap::CreateDescriptorHeap() {
	auto d3d12Device = m_device.GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.Type = m_descriptorHeapType;
	descriptorHeapDesc.NumDescriptors = m_numDescriptorsPerHeap;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	ThrowIfFailed(d3d12Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap)));
	return descriptorHeap;
}

uint32_t CEDynamicDescriptorHeap::ComputeStaleDescriptorCount() const {
	uint32_t numStaleDescriptors = 0;
	DWORD i;
	DWORD staleDescriptorsBitMask = m_staleDescriptorTableBitMask;

	while (_BitScanForward(&i, staleDescriptorsBitMask)) {
		numStaleDescriptors += m_descriptorTableCache[i].NumDescriptors;
		staleDescriptorsBitMask ^= (1 << i);
	}

	return numStaleDescriptors;
}

void CEDynamicDescriptorHeap::CommitDescriptorTables(CECommandList& commandList,
                                                     std::function<void(
	                                                     ID3D12GraphicsCommandList*, UINT,
	                                                     D3D12_GPU_DESCRIPTOR_HANDLE)> setFunction) {
	/*
	 * Compute number of descriptors that need to be copied
	 */
	uint32_t numDescriptorsToCommit = ComputeStaleDescriptorCount();

	if (numDescriptorsToCommit > 0) {
		auto d3d12Device = m_device.GetDevice();
		auto d3d12GraphicsCommandList = commandList.GetCommandList().Get();
		assert(d3d12GraphicsCommandList != nullptr);

		if (!m_currentDescriptorHeap || m_numFreeHandles < numDescriptorsToCommit) {
			m_currentDescriptorHeap = RequestDescriptorHeap();
			m_currentCPUDescriptorHandle = m_currentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_currentGPUDescriptorHandle = m_currentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			m_numFreeHandles = m_numDescriptorsPerHeap;

			commandList.SetDescriptorHeap(m_descriptorHeapType, m_currentDescriptorHeap.Get());

			/*
			 * When updating descriptor heap on command list, all descriptor
			 * tables must be (re)copied to new descriptor heap (not just stale descriptor tables)
			 */
			m_staleDescriptorTableBitMask = m_descriptorTableBitMask;
		}

		DWORD rootIndex;
		/*
		 * Scan from LSB to MSB for bit set in staleDescriptorsBitMask
		 */
		while (_BitScanForward(&rootIndex, m_staleDescriptorTableBitMask)) {
			UINT numSrcDescriptors = m_descriptorTableCache[rootIndex].NumDescriptors;
			D3D12_CPU_DESCRIPTOR_HANDLE* pointerSrcDescriptorHandles = m_descriptorTableCache[rootIndex].BaseDescriptor;

			D3D12_CPU_DESCRIPTOR_HANDLE pointerDestinationDescriptorRangeStarts[] = {m_currentCPUDescriptorHandle};
			UINT pointerDestinationDescriptorRangeSizes[] = {numSrcDescriptors};

			/*
			 * Copy staged CPU visible descriptors to GPU visible descriptor heap.
			 */
			d3d12Device->CopyDescriptors(1, pointerDestinationDescriptorRangeStarts,
			                             pointerDestinationDescriptorRangeSizes, numSrcDescriptors,
			                             pointerSrcDescriptorHandles, nullptr, m_descriptorHeapType);

			/*
			 * set descriptors on command list using passed in setter function
			 */
			setFunction(d3d12GraphicsCommandList, rootIndex, m_currentGPUDescriptorHandle);

			/*
			 * Offset current CPU and GPU descriptor handles.
			 */
			m_currentCPUDescriptorHandle.Offset(numSrcDescriptors, m_descriptorHandleIncrementSize);
			m_currentGPUDescriptorHandle.Offset(numSrcDescriptors, m_descriptorHandleIncrementSize);
			m_numFreeHandles -= numSrcDescriptors;

			/*
			 * Flip stale bit so descriptor table is not (re)copied again unless it is updated with a new descriptor.
			 */
			m_staleDescriptorTableBitMask ^= (1 << rootIndex);
		}

	}
}

void CEDynamicDescriptorHeap::CommitInlineDescriptors(CECommandList& commandList,
                                                      const D3D12_GPU_VIRTUAL_ADDRESS* bufferLocations,
                                                      uint32_t& bitMask,
                                                      std::function<void(
	                                                      ID3D12GraphicsCommandList*, UINT,
	                                                      D3D12_GPU_VIRTUAL_ADDRESS)> setFunction) {
	if (bitMask != 0) {
		auto d3d12GraphicsCommandList = commandList.GetCommandList().Get();
		DWORD rootIndex;
		while (_BitScanForward(&rootIndex, bitMask)) {
			setFunction(d3d12GraphicsCommandList, rootIndex, bufferLocations[rootIndex]);
			/*
			 * Flip stale bit so descriptor is not recopied again unless it is updated with a new descriptor
			 */
			bitMask ^= (1 << rootIndex);
		}
	}
}
