#include "CECookingFactory.h"

#include <PxPhysics.h>
#include <PxPhysicsVersion.h>
#include <cooking/PxCooking.h>

#include "CEPhysX.h"
#include "Utilities/DirectoryUtilities.h"

struct CECookingData {
	CECookingData(const physx::PxTolerancesScale& Scale) : CookingSDK(nullptr), CookingParameters(Scale) {

	}

	physx::PxCooking* CookingSDK;
	physx::PxCookingParams CookingParameters;
};

inline static CECookingData* CookingData = nullptr;

//TODO: Implement
bool CECookingFactory::Create() {
	CookingData = new CECookingData(CEPhysX::GetPhysXSDK().getTolerancesScale());
	CookingData->CookingParameters.meshWeldTolerance = 0.1f;
	CookingData->CookingParameters.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
	CookingData->CookingParameters.midphaseDesc = physx::PxMeshMidPhase::eBVH34;

	CookingData->CookingSDK = PxCreateCooking(PX_PHYSICS_VERSION, CEPhysX::GetFoundation(),
	                                          CookingData->CookingParameters);
	Assert(CookingData->CookingSDK);

	return true;
}

//TODO: Implement
CECookingResult CECookingFactory::CookMesh(CEColliderMeshComponent& Component, CEArray<CEMeshColliderData>& OutData,
                                           bool InvalidateOld) {
	CreateCacheDirectory("Colliders");

	//TODO: Create Meta Data Creator;
	std::filesystem::path FilePath = GetCacheDirectory() / "Colliders" / std::to_string(Component.CollisionMesh->ID) / (
		+ Component.IsConvex ? "_convex.pxm" : "_tri.pxm");
	CECookingResult Result = CECookingResult::Failure;

	if (InvalidateOld) {
		Component.ProcessedMeshes.Clear();
		bool RemovedCached = std::filesystem::remove(FilePath);
		if (!RemovedCached)
			CE_LOG_WARNING("[CECookingFactory]: Could not remove cached collider data for " + FilePath.string());
	}

	if (!std::filesystem::exists(FilePath)) {
		Result = Component.IsConvex
			         ? CookConvexMesh(*Component.CollisionMesh.Get(), OutData)
			         : CookTriangleMesh(*Component.CollisionMesh.Get(), OutData);

		if (Result == CECookingResult::Success) {

		}
	}
	else {

	}

	if (Result == CECookingResult::Success && Component.ProcessedMeshes.Size() == 0) {
		for (const auto& ColliderData : OutData)
			GenerateDebugMesh(Component, ColliderData);
	}

	return Result;
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

void CECookingFactory::Release() {
	CookingData->CookingSDK->release();
	CookingData->CookingSDK = nullptr;

	delete CookingData;
}
