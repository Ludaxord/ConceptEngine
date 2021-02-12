#include "CEHitGroup.h"

using namespace ConceptFramework::GraphicsEngine::Direct3D;

CEHitGroup::CEHitGroup(LPCWSTR ahsExport, LPCWSTR chsExport,
                       const std::wstring& name): exportName(name) {
	desc = {};
	desc.AnyHitShaderImport = ahsExport;
	desc.ClosestHitShaderImport = chsExport;
	desc.HitGroupExport = exportName.c_str();

	subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
	subObject.pDesc = &desc;
}
