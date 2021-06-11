#pragma once
#include <d3d12.h>
#include <string>
#include <wrl/client.h>

#include "CEDXDevice.h"
#include "CEDXDeviceElement.h"
#include "../../../Core/Common/CERefCountedObject.h"
#include "../../../Core/Common/CERef.h"
#include "../../../Utilities/CEStringUtilities.h"
#include "../../../Core/Containers/CEArray.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXDescriptorHeap : public CEDXDeviceElement, public CERefCountedObject {
	public:
		CEDXDescriptorHeap(CEDXDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 numDescriptors,
		                   D3D12_DESCRIPTOR_HEAP_FLAGS flags);
		~CEDXDescriptorHeap() = default;

		bool Create();

		void SetName(const std::string& name) {
			std::wstring wName = ConvertToWString(name);
			Heap->SetName(wName.c_str());
		}

		ID3D12DescriptorHeap* GetHeap() const {
			return Heap.Get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() const {
			return CpuStart;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() const {
			return GpuStart;
		}

		D3D12_DESCRIPTOR_HEAP_TYPE GetType() const {
			return Type;
		}

		uint32 GetNumDescriptors() const {
			return uint32(NumDescriptors);
		}

		uint32 GetDescriptorHandleIncrementSize() const {
			return DescriptorHandleIncrementSize;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap;
		D3D12_CPU_DESCRIPTOR_HANDLE CpuStart;
		D3D12_GPU_DESCRIPTOR_HANDLE GpuStart;
		D3D12_DESCRIPTOR_HEAP_TYPE Type;
		D3D12_DESCRIPTOR_HEAP_FLAGS Flags;
		uint32 NumDescriptors;
		uint32 DescriptorHandleIncrementSize;
	};

	class CEDXOfflineDescriptorHeap : public CEDXDeviceElement, public CERefCountedObject {
		struct CEDescriptorRange {
			CEDescriptorRange() = default;

			CEDescriptorRange(D3D12_CPU_DESCRIPTOR_HANDLE begin, D3D12_CPU_DESCRIPTOR_HANDLE end) : Begin(begin),
				End(end) {

			}

			bool IsValid() const {
				return Begin.ptr < End.ptr;
			}

			D3D12_CPU_DESCRIPTOR_HANDLE Begin = {0};
			D3D12_CPU_DESCRIPTOR_HANDLE End = {0};
		};

		struct CEDescriptorHeap {
			CEDescriptorHeap(const CERef<CEDXDescriptorHeap>& heap): FreeList(), Heap(heap) {
				CEDescriptorRange range;
				range.Begin = heap->GetCPUDescriptorHandleForHeapStart();
				range.End.ptr = range.Begin.ptr + (Heap->GetDescriptorHandleIncrementSize() * Heap->
					GetNumDescriptors());
				FreeList.EmplaceBack(range);
			}

			CEArray<CEDescriptorRange> FreeList;
			CERef<CEDXDescriptorHeap> Heap;
		};

	public:
		CEDXOfflineDescriptorHeap(CEDXDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type);
		~CEDXOfflineDescriptorHeap() = default;

		bool Create();

		D3D12_CPU_DESCRIPTOR_HANDLE Allocate(uint32& oHeapIndex);
		void Free(D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32 heapIndex);

		void SetName(const std::string& name);

		D3D12_DESCRIPTOR_HEAP_TYPE GetType() const {
			return Type;
		}

		uint32 GetDescriptorSize() const {
			return DescriptorSize;
		}

	private:
		bool AllocateHeap();
		D3D12_DESCRIPTOR_HEAP_TYPE Type;
		CEArray<CEDescriptorHeap> Heaps;
		std::string Name;
		uint32 DescriptorSize = 0;
	};

	class CEDXOnlineDescriptorHeap : public CEDXDeviceElement, public CERefCountedObject {
	public:
		CEDXOnlineDescriptorHeap(CEDXDevice* device, uint32 descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE type);
		~CEDXOnlineDescriptorHeap() = default;

		bool Create();

		uint32 AllocateHandles(uint32 numHandles);
		bool AllocateFreshHeap();

		bool HasSpace(uint32 numHandles) const;

		void Reset();

		void SetName(const std::string& name) {
			Heap->SetName(name);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleAt(uint32 index) const {
			return {
				Heap->GetCPUDescriptorHandleForHeapStart().ptr + (index * Heap->GetDescriptorHandleIncrementSize())
			};
		}

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleAt(uint32 index) const {
			return {
				Heap->GetGPUDescriptorHandleForHeapStart().ptr + (index * Heap->GetDescriptorHandleIncrementSize())
			};
		}

		uint32 GetDescriptorHandleIncrementSize() const {
			return Heap->GetDescriptorHandleIncrementSize();
		}

		ID3D12DescriptorHeap* GetNativeHeap() const {
			return Heap->GetHeap();
		}

		CEDXDescriptorHeap* GetHeap() const {
			return Heap.Get();
		}

	protected:
	private:
		CERef<CEDXDescriptorHeap> Heap;

		CEArray<CERef<CEDXDescriptorHeap>> HeapPool;
		CEArray<CERef<CEDXDescriptorHeap>> DiscardedHeaps;

		D3D12_DESCRIPTOR_HEAP_TYPE Type;
		uint32 CurrentHandle = 0;
		uint32 DescriptorCount = 0;
	};

}
