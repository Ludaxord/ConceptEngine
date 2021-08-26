#include "CECookingFactory.h"

#include <PxPhysics.h>
#include <PxPhysicsVersion.h>
#include <cooking/PxCooking.h>

#include "CEPhysX.h"
#include "Utilities/DirectoryUtilities.h"

#include <extensions/PxDefaultStreams.h>

struct CECookingData {
	CECookingData(const physx::PxTolerancesScale& Scale) : CookingSDK(nullptr), CookingParameters(Scale) {

	}

	physx::PxCooking* CookingSDK;
	physx::PxCookingParams CookingParameters;
};

inline static CECookingData* CookingData = nullptr;

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
			uint32 BufferSize = 0;
			uint32 Offset = 0;

			for (const auto& ColliderData : OutData) {
				BufferSize += sizeof(uint32);
				BufferSize += ColliderData.Size;
			}

			//TODO: Create File Writer..
			CEFileBuffer ColliderBuffer;
			ColliderBuffer.Allocate(BufferSize);

			for (auto& ColliderData : OutData) {
				ColliderBuffer.Write((void*)&ColliderData.Size, sizeof(uint32), Offset);
				Offset += sizeof(uint32);
				ColliderBuffer.Write(ColliderData.Data, ColliderData.Size, Offset);
				Offset += ColliderData.Size;
			}

			if (!WriteBytes(FilePath, ColliderBuffer)) {
				CE_LOG_ERROR("[CECookingFactory]: Failed to Write Collider to " + FilePath.string());
				return CECookingResult::Failure;
			}

			ColliderBuffer.Release();
		}
	}
	else {
		CEFileBuffer ColliderBuffer = ReadBytes(FilePath);

		if (ColliderBuffer.Size > 0) {
			uint32 Offset = 0;

			//TODO: Add Submeshes... NOTE: To add submeshes, need to add loading object from files not creating base objects...
			const auto& Submeshes = Component.CollisionMesh->MainMeshData;

			for (const auto& Submesh : Submeshes.Vertices) {
				CEMeshColliderData& Data = OutData.EmplaceBack();

				Data.Size = ColliderBuffer.Read<uint32>(Offset);
				Offset += sizeof(uint32);
				Data.Data = ColliderBuffer.ReadBytes(Data.Size, Offset);
				Offset += Data.Size;
				Data.Transform = Submeshes.Transform;
			}

			ColliderBuffer.Release();

			Result = CECookingResult::Success;
		}
	}

	if (Result == CECookingResult::Success && Component.ProcessedMeshes.Size() == 0) {
		for (const auto& ColliderData : OutData)
			GenerateDebugMesh(Component, ColliderData);
	}

	return Result;
}

CECookingResult CECookingFactory::CookConvexMesh(const Mesh& Mesh, CEArray<CEMeshColliderData>& OutData) {
	const auto& Vertices = Mesh.MainMeshData.Vertices;
	const auto& Indices = Mesh.MainMeshData.Indices;

	for (int i = 0; i < Vertices.Size(); i++) {
		physx::PxConvexMeshDesc ConvexDesc;
		ConvexDesc.points.count = Mesh.VertexCount;
		ConvexDesc.points.stride = sizeof(Vertex);
		ConvexDesc.points.data = &Vertices[i];
		ConvexDesc.indices.count = Mesh.IndexCount / 3;
		ConvexDesc.indices.data = &Indices[i / 3];
		ConvexDesc.indices.stride = sizeof(uint32);
		ConvexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX | physx::PxConvexFlag::eSHIFT_VERTICES;

		physx::PxDefaultMemoryOutputStream Buffer;
		physx::PxConvexMeshCookingResult::Enum Result;
		if (!CookingData->CookingSDK->cookConvexMesh(ConvexDesc, Buffer, &Result)) {
			CE_LOG_ERROR("[CECookingFactory]: Failed to Cook ConvexMesh " + Mesh.ID)
			OutData.Clear();
			return FromPhysXCookingResult(Result);
		}

		CEMeshColliderData& Data = OutData.EmplaceBack();
		Data.Size = Buffer.getSize();
		Data.Data = new byte[Data.Size];
		Data.Transform = Mesh.MainMeshData.Transform;
		memcpy(Data.Data, Buffer.getData(), Data.Size);
	}

	return CECookingResult::Success;
}

//TODO: Implement
CECookingResult CECookingFactory::CookTriangleMesh(const Mesh& Mesh, CEArray<CEMeshColliderData>& OutData) {


	return CECookingResult::Success;
}

//TODO: Implement
void CECookingFactory::GenerateDebugMesh(CEColliderMeshComponent& Component, const CEMeshColliderData& ColliderData) {
}

void CECookingFactory::Release() {
	CookingData->CookingSDK->release();
	CookingData->CookingSDK = nullptr;

	delete CookingData;
}
