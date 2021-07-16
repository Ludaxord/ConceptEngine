#include "CEMesh.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

using namespace ConceptEngine::Graphics::Main::Common;

bool CEMesh::Create(const CEMeshData& data) {
	VertexCount = static_cast<uint32>(data.Vertices.Size());
	IndexCount = static_cast<uint32>(data.Indices.Size());

	const bool RayTracingSupported = CastGraphicsManager()->IsRayTracingSupported();

	const uint32 BufferFlags = RayTracingSupported
		                           ? RenderLayer::BufferFlag_SRV | RenderLayer::BufferFlag_Default
		                           : RenderLayer::BufferFlag_Default;

	RenderLayer::CEResourceData InitialData(data.Vertices.Data(), data.Vertices.SizeInBytes());
	VertexBuffer = CastGraphicsManager()->CreateVertexBuffer<CEVertex>(VertexCount,
	                                                                   BufferFlags,
	                                                                   RenderLayer::CEResourceState::VertexAndConstantBuffer,
	                                                                   &InitialData);
	if (!VertexBuffer) {
		return false;
	}

	VertexBuffer->SetName("VertexBuffer");

	InitialData = RenderLayer::CEResourceData(data.Indices.Data(), data.Indices.SizeInBytes());

	IndexBuffer = CastGraphicsManager()->CreateIndexBuffer(RenderLayer::CEIndexFormat::uint32,
	                                                       IndexCount,
	                                                       BufferFlags,
	                                                       RenderLayer::CEResourceState::IndexBuffer,
	                                                       &InitialData);
	if (!IndexBuffer) {
		return false;
	}

	IndexBuffer->SetName("IndexBuffer");

	if (RayTracingSupported) {
		RTGeometry = CastGraphicsManager()->CreateRayTracingGeometry(RenderLayer::RayTracingStructureBuildFlag_None,
		                                                             VertexBuffer.Get(),
		                                                             IndexBuffer.Get()
		);
		if (!RTGeometry) {
			return false;
		}

		RTGeometry->SetName("RayTracing Geometry");

		VertexBufferSRV = CastGraphicsManager()->CreateShaderResourceViewForVertexBuffer(
			VertexBuffer.Get(), 0, VertexCount);
		if (!VertexBufferSRV) {
			return false;
		}

		IndexBufferSRV = CastGraphicsManager()->
			CreateShaderResourceViewForIndexBuffer(IndexBuffer.Get(), 0, IndexCount);

		if (!IndexBufferSRV) {
			return false;
		}
	}

	CreateBoundingBox(data);

	return true;
}

bool CEMesh::BuildAccelerationStructure(RenderLayer::CECommandList& commandList) {
	commandList.BuildRayTracingGeometry(RTGeometry.Get(), VertexBuffer.Get(), IndexBuffer.Get(), true);
	return true;
}

CESharedPtr<CEMesh> CEMesh::Make(const CEMeshData& data) {
	CESharedPtr<CEMesh> Result = MakeShared<CEMesh>();
	if (Result->Create(data)) {
		return Result;
	}

	return CESharedPtr<CEMesh>(nullptr);
}

void CEMesh::CreateBoundingBox(const CEMeshData& data) {
	constexpr float Inf = std::numeric_limits<float>::infinity();
	CEVectorFloat3 Min = CEVectorFloat3(Inf, Inf, Inf);
	CEVectorFloat3 Max = CEVectorFloat3(-Inf, -Inf, -Inf);

	for (const CEVertex& Vertex : data.Vertices) {
		Min.x = Math::CEMath::Min<float>(Min.x, Vertex.Position.x);
		Max.x = Math::CEMath::Max<float>(Max.x, Vertex.Position.x);
		
		Min.y = Math::CEMath::Min<float>(Min.y, Vertex.Position.y);
		Max.y = Math::CEMath::Max<float>(Max.y, Vertex.Position.y);
		
		Min.z = Math::CEMath::Min<float>(Min.z, Vertex.Position.z);
		Max.z = Math::CEMath::Max<float>(Max.z, Vertex.Position.z);
	}

	BoundingBox.Top = Max;
	BoundingBox.Bottom = Min;
}
