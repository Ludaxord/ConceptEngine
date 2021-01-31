#pragma once
#include <wrl.h>

#include "CEByteAddressBuffer.h"

namespace Concept::GraphicsEngine::Direct3D {
	namespace wrl = Microsoft::WRL;
	class CEDevice;

	class CEStructuredBuffer : public CEBuffer {
	public:
		/**
		 * Get number of elements contained in buffer;
		 */
		virtual size_t GetNumElements() const {
			return m_numElements;
		}

		/**
		 * Get size in bytes of each element in buffer;
		 */
		virtual size_t GetElementSize() const {
			return m_elementSize;
		}

		std::shared_ptr<CEByteAddressBuffer> GetCounterBuffer() const {
			return m_counterBuffer;
		}

	protected:
		CEStructuredBuffer(CEDevice& device, size_t numElements, size_t elementSize);
		CEStructuredBuffer(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource, size_t numElements,
		                          size_t elementSize);
		virtual ~CEStructuredBuffer() = default;

	private:
		size_t m_numElements;
		size_t m_elementSize;

		/**
		 * Buffer store internal counter for structured buffer
		 */
		std::shared_ptr<CEByteAddressBuffer> m_counterBuffer;
	};
}
