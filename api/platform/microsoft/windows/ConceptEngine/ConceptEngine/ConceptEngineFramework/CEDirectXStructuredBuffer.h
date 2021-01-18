#pragma once
#include <wrl.h>

#include "CEDirectXByteAddressBuffer.h"

namespace ConceptEngine::GraphicsEngine::DirectX {
	namespace wrl = Microsoft::WRL;

	class CEDirectXStructuredBuffer : public CEDirectXBuffer {
	public:
		/*
		 * Get number of elements contained in buffer;
		 */
		virtual size_t GetNumElements() const {
			return m_numElements;
		}

		/*
		 * Get size in bytes of each element in buffer;
		 */
		virtual size_t GetElementSize() const {
			return m_elementSize;
		}

		std::shared_ptr<CEDirectXByteAddressBuffer> GetCounterBuffer() const {
			return m_counterBuffer;
		}

	protected:
		CEDirectXStructuredBuffer(CEDirectXDevice& device, size_t numElements, size_t elementSize);
		CEDirectXStructuredBuffer(CEDirectXDevice& device, wrl::ComPtr<ID3D12Resource> resource, size_t numElements,
		                          size_t elementSize);
		virtual ~CEDirectXStructuredBuffer() = default;

	private:
		size_t m_numElements;
		size_t m_elementSize;

		/*
		 * Buffer store internal counter for structured buffer
		 */
		std::shared_ptr<CEDirectXByteAddressBuffer> m_counterBuffer;
	};
}
