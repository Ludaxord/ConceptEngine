#include "CECookingFactory.h"

//TODO: Implement
bool CECookingFactory::Create() {
	return false;
}

//TODO: Implement
CECookingResult CECookingFactory::CookMesh(CEColliderMeshComponent& Component, CEArray<CEMeshColliderData>& OutData,
                                           bool InvalidateOld) {
	return {};
}

//TODO: Implement
CECookingResult CECookingFactory::CookConvexMesh(const Mesh& Mesh, CEArray<CEMeshColliderData>& OutData) {
	return {};
}

//TODO: Implement
CECookingResult CECookingFactory::CookTriangleMesh(const Mesh& Mesh, CEArray<CEMeshColliderData>& OutData) {
	return {};
}

//TODO: Implement
void CECookingFactory::GenerateDebugMesh(CEColliderMeshComponent& Component, const CEMeshColliderData& ColliderData) {
}

//TODO: Implement
void CECookingFactory::Release() {
}
