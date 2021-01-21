#pragma once
#include <cstdint>
#include <d3d12.h>
#include <functional>
#include <memory>
#include <queue>
#include <wrl.h>

#include "d3dx12.h"

namespace Concept::GraphicsEngine::Direct3D {
	class CERootSignature;
	class CECommandList;
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	class CEDynamicDescriptorHeap {
	public:
		CEDynamicDescriptorHeap(CEDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		                        uint32_t numDescriptorsPerHeap = 1024);
		virtual ~CEDynamicDescriptorHeap();

		/**
		 * Stages a contiguous range of CPU visible descriptors.
		 * Descriptors are not copied to GPU visible descriptor heap until
		 * CommitStagedDescriptors function is called. 
		 */
		void StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors,
		                      const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors);

		/**
		 * Stage inline CBV Descriptor.
		 */
		void StageInlineCBV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);

		/**
		 * Stage inline SRV descriptor.
		 */
		void StageInlineSRV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);

		/**
		 * Stage inline UAV descriptor
		 */
		void StageInlineUAV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);

		void CommitStagedDescriptorsForDraw(CECommandList& commandList);
		void CommitStagedDescriptorsForDispatch(CECommandList& commandList);

		/**
		 * Copies single CPU visible descriptor to GPU visible descriptor heap
		 * useful for:
		 *	* ID3D12GraphicsCommandList::ClearUnorderedAccessViewFloat
		 *	* ID3D12GraphicsCommandList::ClearUnorderedAccessViewUint
		 *	methods which require both CPU and GPU visible descriptors for UAV resource.
		 *
		 *	@param [commandList], command list required in case GPU visible descriptor heap
		 *	needs to be updated on command list
		 *	@param [cpuDescriptor], CPU descriptor to cpy into GPU visible descriptor heap.
		 *
		 *	@return GPU visible descriptor.l
		 */
		D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor(CECommandList& commandList,
		                                           D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor);

		/**
		 * Parse root signature to determine which root parameters contain descriptor tables and determine number of descriptors needed for each table.
		 */
		void ParseRootSignature(const std::shared_ptr<CERootSignature>& rootSignature);

		/**
		 * Reset used descriptors. Should only be used if any descriptors are being referenced by command list has finished executing on command queue.
		 */
		void Reset();

	protected:
	private:
		/**
		 * Request descriptor heap if one is available
		 */
		wrl::ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();

		/**
		 * Create new descriptor heap if no descriptor heap is available.
		 */
		wrl::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap();

		/**
		 * Compute number of stale descriptors that need to be copied to GPU visible descriptor heap.
		 */
		uint32_t ComputeStaleDescriptorCount() const;

		/**
		 * Copy all of staged descriptors to GPU visible descriptor heap and bind descriptor heap and descriptor tables to command list;
		 * Passed in function object is used to set GPU visible descriptors on command list. Two possible functions are:
		 *	* Before draw: ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable
		 *	* Before dispatch: ID3D12GraphicsCommandList::SetComputeRootDescriptorTable
		 * Since DynamicDescriptorHeap can not know which function will be used, it must be passed as an argument to function
		 */
		void CommitDescriptorTables(CECommandList& commandList,
		                            std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)>
		                            setFunction);
		void CommitInlineDescriptors(CECommandList& commandList,
		                             const D3D12_GPU_VIRTUAL_ADDRESS* bufferLocations, uint32_t& bitMask,
		                             std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_VIRTUAL_ADDRESS)>
		                             setFunction);
		/**
		 * Maximum number of descriptor tables per root signature.
		 * 32-bit mask is used to keep track of root parameter indices that are descriptor tables
		 */
		static const uint32_t MaxDescriptorTables = 32;

		/**
		 * Structure that represents descriptor table entry in root signature.
		 */
		struct DescriptorTableCache {
			DescriptorTableCache(): NumDescriptors(0), BaseDescriptor(nullptr) {

			}

			void Reset() {
				NumDescriptors = 0;
				BaseDescriptor = nullptr;
			}

			/**
			 * number of descriptor in descriptor table.
			 */
			uint32_t NumDescriptors;

			/**
			 * Pointer to descriptor in descriptor handle cache.
			 */
			D3D12_CPU_DESCRIPTOR_HANDLE* BaseDescriptor;
		};

		/**
		 * Device that is used to create descriptor heap
		 */
		CEDevice& m_device;

		/**
		 * Describes type of descriptor that can be staged using dynamic descriptor heap
		 * Valid values:
		 *	* D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		 *	* D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
		 *	This parameter also determines type of GPU visible descriptor heaps.
		 */
		D3D12_DESCRIPTOR_HEAP_TYPE m_descriptorHeapType;

		/**
		 * Number of descriptors to allocate in new GPU visible descriptor heaps.
		 */
		uint32_t m_numDescriptorsPerHeap;

		/**
		 * increment size of descriptor
		 */
		uint32_t m_descriptorHandleIncrementSize;

		/**
		 * descriptor handle cache
		 */
		std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> m_descriptorHandleCache;

		/**
		 * Descriptor handle per descriptor table
		 */
		DescriptorTableCache m_descriptorTableCache[MaxDescriptorTables];

		/**
		 * Inline CBV
		 */
		D3D12_GPU_VIRTUAL_ADDRESS m_inlineCBV[MaxDescriptorTables];

		/**
		 * Inline SRV
		 */
		D3D12_GPU_VIRTUAL_ADDRESS m_inlineSRV[MaxDescriptorTables];

		/**
		 * Inline UAV
		 */
		D3D12_GPU_VIRTUAL_ADDRESS m_inlineUAV[MaxDescriptorTables];

		/**
		 * Each bit in bit mask represents index in root signature
		 * that contains descriptor table.
		 */
		uint32_t m_descriptorTableBitMask;

		/**
		 * Each bit set in bit mask represents descriptor table
		 * in root signature that has changed since last time
		 * descriptor were copied.
		 */
		uint32_t m_staleDescriptorTableBitMask;
		uint32_t m_staleCBVBitMask;
		uint32_t m_staleSRVBitMask;
		uint32_t m_staleUAVBitMask;

		using DescriptorHeapPool = std::queue<wrl::ComPtr<ID3D12DescriptorHeap>>;

		DescriptorHeapPool m_descriptorHeapPool;
		DescriptorHeapPool m_availableDescriptorHeaps;

		wrl::ComPtr<ID3D12DescriptorHeap> m_currentDescriptorHeap;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_currentGPUDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_currentCPUDescriptorHandle;

		uint32_t m_numFreeHandles;
	};
}
