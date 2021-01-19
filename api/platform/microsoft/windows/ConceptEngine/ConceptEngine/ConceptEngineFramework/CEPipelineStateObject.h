#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D {
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	class CEPipelineStateObject {
	public:
		wrl::ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const {
			return m_pipelineState;
		}

	protected:
		CEPipelineStateObject(CEDevice& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc);
		virtual ~CEPipelineStateObject() = default;

	private:
		CEDevice& m_device;
		wrl::ComPtr<ID3D12PipelineState> m_pipelineState;
	};
}
