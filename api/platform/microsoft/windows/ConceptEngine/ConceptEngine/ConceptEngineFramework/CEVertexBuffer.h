#pragma once
#include <wrl.h>

#include "CEBuffer.h"

namespace ConceptEngine::GraphicsEngine::Direct3D {
	namespace wrl = Microsoft::WRL;

	class CEVertexBuffer : public CEBuffer {
	public:
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const {
			return m_vertexBufferView;
		}

		size_t GetNumVertices() const {
			return m_numVertices;
		}

		size_t GetVertexStride() const {
			return m_vertexStride;
		}

	protected:
		CEVertexBuffer(CEDevice& device, size_t numVertices, size_t vertexStride);
		CEVertexBuffer(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource, size_t numVertices,
		                      size_t vertexStride);
		virtual ~CEVertexBuffer();

		void CreateVertexBufferView();

	private:
		size_t m_numVertices;
		size_t m_vertexStride;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	};
}
