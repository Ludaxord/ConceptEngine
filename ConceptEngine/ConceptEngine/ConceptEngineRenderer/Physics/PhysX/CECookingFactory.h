#pragma once
#include "CePhysXUtils.h"
#include "Scene/Components/CEColliderMeshComponent.h"

#include "Physics/CEPhysicsMaterial.h"
#include "Rendering/Resources/Mesh.h"

struct CEMeshColliderData {
	byte* Data;
	XMFLOAT4X4 Transform;
	uint32 Size;
};

class CECookingFactory {
public:
	bool Create();
	void Release();
	CECookingResult CookMesh(CEColliderMeshComponent& Component, CEArray<CEMeshColliderData>& OutData,
	                         bool InvalidateOld = false);
	CECookingResult CookConvexMesh(const Mesh& Mesh, CEArray<CEMeshColliderData>& OutData);
	CECookingResult CookTriangleMesh(const Mesh& Mesh, CEArray<CEMeshColliderData>& OutData);

private:
	void GenerateDebugMesh(CEColliderMeshComponent& Component, const CEMeshColliderData& ColliderData);
};
