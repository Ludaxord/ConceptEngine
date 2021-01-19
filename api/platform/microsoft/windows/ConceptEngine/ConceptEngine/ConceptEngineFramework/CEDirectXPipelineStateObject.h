#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	class CEDirectXDevice;
	namespace wrl = Microsoft::WRL;

	class CEDirectXPipelineStateObject {
	public:
		wrl::ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const {
			return m_pipelineState;
		}

	protected:
		CEDirectXPipelineStateObject(CEDirectXDevice& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc);
		virtual ~CEDirectXPipelineStateObject() = default;

	private:
		CEDirectXDevice& m_device;
		wrl::ComPtr<ID3D12PipelineState> m_pipelineState;
	};
}
