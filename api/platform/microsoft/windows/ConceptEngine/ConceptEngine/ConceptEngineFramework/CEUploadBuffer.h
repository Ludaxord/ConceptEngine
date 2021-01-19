#pragma once
#include <d3d12.h>
#include <memory>
#include <deque>
#include <wrl.h>

#include "CEDefinitions.h"

namespace ConceptEngine::GraphicsEngine::Direct3D {
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	class CEDirectXUploadBuffer {
	public:
		/**
		 * Use to upload data to GPU
		 */
		struct Allocation {
			void* CPU;
			D3D12_GPU_VIRTUAL_ADDRESS GPU;
		};

		/**
		 * maximum size of allocation is size of single page
		 */
		size_t GetPageSize() const {
			return m_pageSize;
		}

		/**
		 * Allocate memory is upload heap.
		 * allocation must not exceed size of a page.
		 * use memcpy or similar method to copy buffer data
		 * to CPU pointer in allocation structure returned from this function.
		 */
		Allocation Allocate(size_t sizeInBytes, size_t alignment);

		/**
		 * Release all allocated pages. This should only be done when command list finished executing on command queue.
		 */
		void Reset();

	protected:
		friend class std::default_delete<CEDirectXUploadBuffer>;

		/**
		 * @param pageSize, size to use to allocate new pages in GPU memory.
		 */
		explicit CEDirectXUploadBuffer(CEDevice& device, size_t pageSize = _2MB);

	private:
		/**
		 * single page for allocator
		 */
		struct Page {
			Page(CEDevice& device, size_t sizeInBytes);
			~Page();

			/**
			 * Check to see if page has room to satisfy requested allocation.
			 */
			bool HasSpace(size_t sizeInBytes, size_t alignment) const;

			/**
			 * Allocate memory from page.
			 * Throws std::bad_alloc if allocation size is larger than page size
			 * or size of allocation exceeds remaining space in page;
			 */
			Allocation Allocate(size_t sizeInBytes, size_t alignment);

			/**
			 * Reset page for reuse
			 */
			void Reset();

		private:
			CEDevice& m_device;
			wrl::ComPtr<ID3D12Resource> m_resource;

			/**
			 * Base Pointer
			 */
			void* m_CPUPointer;
			D3D12_GPU_VIRTUAL_ADDRESS m_GPUPointer;

			/**
			 * Allocated page size.
			 */
			size_t m_pageSize;

			/**
			 * Current allocation offset in bytes.
			 */
			size_t m_offset;
		};

		/**
		 * pool of memory pages
		 */
		using PagePool = std::deque<std::shared_ptr<Page>>;

		/**
		 * Device that was used to create buffer
		 */
		CEDevice& m_device;

		/**
		 * Request page from pool of available pages
		 * or create new page if there are no available pages
		 */
		std::shared_ptr<Page> RequestPage();

		PagePool m_pagePool;
		PagePool m_availablePages;

		std::shared_ptr<Page> m_currentPage;

		/**
		 * Size of each page of memory.
		 */
		size_t m_pageSize;
	};
}
