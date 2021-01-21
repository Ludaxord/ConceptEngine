#pragma once
#include <d3d12.h>
#include <string>
#include <wrl.h>

namespace Concept::GraphicsEngine::Direct3D {
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	class CEPipelineStateObject {
	public:
		wrl::ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const {
			return m_pipelineState;
		}

		/**
		 * TODO: Now leave function here, in future move it to more convenient class
		 */
		static D3D12_SHADER_BYTECODE CompileShaderFileToByteCode(std::wstring shaderPath);

	protected:
		CEPipelineStateObject(CEDevice& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc);
		virtual ~CEPipelineStateObject() = default;

	private:
		CEDevice& m_device;
		wrl::ComPtr<ID3D12PipelineState> m_pipelineState;
	};
}
