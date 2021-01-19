#pragma once
#include <wrl.h>

#include "CEDirectXBuffer.h"

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	namespace wrl = Microsoft::WRL;

	class CEDirectXVertexBuffer : public CEDirectXBuffer {
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
		CEDirectXVertexBuffer(CEDirectXDevice& device, size_t numVertices, size_t vertexStride);
		CEDirectXVertexBuffer(CEDirectXDevice& device, wrl::ComPtr<ID3D12Resource> resource, size_t numVertices,
		                      size_t vertexStride);
		virtual ~CEDirectXVertexBuffer();

		void CreateVertexBufferView();

	private:
		size_t m_numVertices;
		size_t m_vertexStride;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	};
}
