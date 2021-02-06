#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace Concept::GraphicsEngine::Direct3D {
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	class CEStateObject {
	public:
		wrl::ComPtr<ID3D12StateObject> GetD3D12StateObject() const {
			return m_rtPipeline;
		}

	protected:
		CEStateObject(CEDevice& device, const D3D12_STATE_OBJECT_DESC& desc);
		virtual ~CEStateObject() = default;
	private:
		CEDevice& m_device;
		wrl::ComPtr<ID3D12StateObject> m_rtPipeline;
		wrl::ComPtr<ID3D12StateObjectProperties> sop;
	};
}
