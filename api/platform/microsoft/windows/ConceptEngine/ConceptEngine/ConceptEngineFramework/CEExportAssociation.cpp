#include "CEExportAssociation.h"

using namespace ConceptFramework::GraphicsEngine::Direct3D;

CEExportAssociation::CEExportAssociation(const WCHAR* exportNames[], uint32_t exportCount,
                                         const D3D12_STATE_SUBOBJECT* pSubObjectToAssociate) {
	association.NumExports = exportCount;
	association.pExports = exportNames;
	association.pSubobjectToAssociate = pSubObjectToAssociate;

	subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
	subObject.pDesc = &association;
}
