#include "CEStateObject.h"

#include "CEHelper.h"

using namespace ConceptFramework::GraphicsEngine::Direct3D;

CEStateObject::CEStateObject(CEDevice& device, const D3D12_STATE_OBJECT_DESC& desc): m_device(device) {
	auto rtDevice = device.GetDevice();

	ThrowIfFailed(rtDevice->CreateStateObject(&desc, IID_PPV_ARGS(&m_rtPipeline)));
	m_rtPipeline->QueryInterface(IID_PPV_ARGS(&sop));
}
