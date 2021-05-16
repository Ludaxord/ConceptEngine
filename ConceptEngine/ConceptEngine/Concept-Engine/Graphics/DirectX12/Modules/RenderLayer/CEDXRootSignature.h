#pragma once
#include "CEDXDeviceElement.h"
#include "../../../../Core/Common/CERefCountedObject.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::RenderLayer {
	enum class CERootSignatureType {
		Unknown = 0,
		Graphics = 1,
		Compute = 2,
		RayTracingGlobal = 3,
		RayTracingLocal = 4
	};

	struct CEDXRootSignatureResourceCount {

	};

	struct CEDXRootSignatureDescHelper {

	};

	class CEDXRootSignature : public CEDXDeviceElement, public Core::Common::CERefCountedObject {

	};

	class CEDXRootSignatureCache : public CEDXDeviceElement {
	public:
		CEDXRootSignatureCache(CEDXDevice* device);
		~CEDXRootSignatureCache();

		bool Create();
		void ReleaseAll();
	private:
		static CEDXRootSignatureCache* Instance;
	};
}
