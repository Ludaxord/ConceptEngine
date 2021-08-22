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
	//TODO: Implement
	bool Create();

	//TODO: Implement
	void Release();

	//TODO: Implement
	CECookingResult CookMesh(CEColliderMeshComponent& Component, CEArray<CEMeshColliderData>& OutData,
	                         bool InvalidateOld = false);

	//TODO: Implement
	CECookingResult CookConvexMesh(const Mesh& Mesh, CEArray<CEMeshColliderData>& OutData);
	//TODO: Implement
	CECookingResult CookTriangleMesh(const Mesh& Mesh, CEArray<CEMeshColliderData>& OutData);

private:
	//TODO: Implement
	void GenerateDebugMesh(CEColliderMeshComponent& Component, const CEMeshColliderData& ColliderData);
};
