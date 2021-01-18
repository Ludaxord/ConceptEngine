#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "CEDirectXBuffer.h"

namespace ConceptEngine::GraphicsEngine::DirectX {
	namespace wrl = Microsoft::WRL;

	class CEDirectXIndexBuffer : public CEDirectXBuffer {
	public:
		/*
		 * Get index buffer view for biding to the Input Assembler stage.
		 */
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const {
			return m_indexBufferView;
		}

		size_t GetNumIndices() const {
			return m_numIndices;
		}

		DXGI_FORMAT GetIndexFormat() const {
			return m_indexFormat;
		}

	protected:
		CEDirectXIndexBuffer(CEDirectXDevice& device, size_t numIndices, DXGI_FORMAT indexFormat);
		CEDirectXIndexBuffer(CEDirectXDevice& device, wrl::ComPtr<ID3D12Resource> resource, DXGI_FORMAT indexFormat);
		virtual ~CEDirectXIndexBuffer() = default;

		void CreateIndexBufferView();

	private:
		size_t m_numIndices;
		DXGI_FORMAT m_indexFormat;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	};
}
