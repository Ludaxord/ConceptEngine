#pragma once
#include <memory>

#include "CESubObject.h"

namespace ConceptFramework::GraphicsEngine::Direct3D {
	class CEDevice;

	class CEExportAssociation : public CESubObject {
	public:
	protected:
		friend class CEDevice;
		friend class std::default_delete<CEExportAssociation>;

		CEExportAssociation(const WCHAR* exportNames[], uint32_t exportCount,
		                    const D3D12_STATE_SUBOBJECT* pSubObjectToAssociate);

	private:
		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION association = {};
	};
}
