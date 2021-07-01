#include "CEDXMeshManager.h"

using namespace ConceptEngine::Graphics::DirectX12::Managers;

using namespace DirectX;

CEDXMeshManager::CEDXMeshManager() = default;

CEDXMeshManager::~CEDXMeshManager() {
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreateFromFile(const std::string& filename, bool mergeMeshes,
                                                                          bool leftHanded) noexcept {
	return {};
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreateCube(float width, float height, float depth) noexcept {

	const float HalfWidth = width * 0.5f;
	const float HalfHeight = height * 0.5f;
	const float HalfDepth = depth * 0.5f;

	ConceptEngine::Graphics::Main::CEMeshData Cube;
	Cube.Vertices =
	{
		// FRONT FACE
		{
			XMFLOAT3(-HalfWidth, HalfHeight, -HalfDepth),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 0.0f)
		},
		{
			XMFLOAT3(HalfWidth, HalfHeight, -HalfDepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 0.0f)
		},
		{
			XMFLOAT3(-HalfWidth, -HalfHeight, -HalfDepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 1.0f)
		},
		{
			XMFLOAT3(HalfWidth, -HalfHeight, -HalfDepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 1.0f)
		},

		// BACK FACE
		{
			XMFLOAT3(HalfWidth, HalfHeight, HalfDepth), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 0.0f)
		},
		{
			XMFLOAT3(-HalfWidth, HalfHeight, HalfDepth), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 0.0f)
		},
		{
			XMFLOAT3(HalfWidth, -HalfHeight, HalfDepth), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 1.0f)
		},
		{
			XMFLOAT3(-HalfWidth, -HalfHeight, HalfDepth), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 1.0f)
		},

		// RIGHT FACE
		{
			XMFLOAT3(HalfWidth, HalfHeight, -HalfDepth), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
			XMFLOAT2(0.0f, 0.0f)
		},
		{
			XMFLOAT3(HalfWidth, HalfHeight, HalfDepth), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
			XMFLOAT2(1.0f, 0.0f)
		},
		{
			XMFLOAT3(HalfWidth, -HalfHeight, -HalfDepth), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
			XMFLOAT2(0.0f, 1.0f)
		},
		{
			XMFLOAT3(HalfWidth, -HalfHeight, HalfDepth), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
			XMFLOAT2(1.0f, 1.0f)
		},

		// LEFT FACE
		{
			XMFLOAT3(-HalfWidth, HalfHeight, -HalfDepth), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
			XMFLOAT2(0.0f, 1.0f)
		},
		{
			XMFLOAT3(-HalfWidth, HalfHeight, HalfDepth), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
			XMFLOAT2(1.0f, 1.0f)
		},
		{
			XMFLOAT3(-HalfWidth, -HalfHeight, -HalfDepth), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
			XMFLOAT2(0.0f, 0.0f)
		},
		{
			XMFLOAT3(-HalfWidth, -HalfHeight, HalfDepth), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
			XMFLOAT2(1.0f, 0.0f)
		},

		// TOP FACE
		{
			XMFLOAT3(-HalfWidth, HalfHeight, HalfDepth), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 0.0f)
		},
		{
			XMFLOAT3(HalfWidth, HalfHeight, HalfDepth), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 0.0f)
		},
		{
			XMFLOAT3(-HalfWidth, HalfHeight, -HalfDepth), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 1.0f)
		},
		{
			XMFLOAT3(HalfWidth, HalfHeight, -HalfDepth), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 1.0f)
		},

		// BOTTOM FACE
		{
			XMFLOAT3(-HalfWidth, -HalfHeight, -HalfDepth), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 0.0f)
		},
		{
			XMFLOAT3(HalfWidth, -HalfHeight, -HalfDepth), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 0.0f)
		},
		{
			XMFLOAT3(-HalfWidth, -HalfHeight, HalfDepth), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 1.0f)
		},
		{
			XMFLOAT3(HalfWidth, -HalfHeight, HalfDepth), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 1.0f)
		},
	};

	Cube.Indices =
	{
		// FRONT FACE
		0, 1, 2,
		1, 3, 2,

		// BACK FACE
		4, 5, 6,
		5, 7, 6,

		// RIGHT FACE
		8, 9, 10,
		9, 11, 10,

		// LEFT FACE
		14, 13, 12,
		14, 15, 13,

		// TOP FACE
		16, 17, 18,
		17, 19, 18,

		// BOTTOM FACE
		20, 21, 22,
		21, 23, 22
	};

	return Cube;
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreatePlane(uint32 width, uint32 height) noexcept {

	Main::CEMeshData data;
	if (width < 1) {
		width = 1;
	}
	if (height < 1) {
		height = 1;
	}

	data.Vertices.Resize((width + 1) * (height + 1));
	data.Indices.Resize((width * height) * 6);

	// Size of each quad, size of the plane will always be between -0.5 and 0.5
	XMFLOAT2 quadSize = XMFLOAT2(1.0f / float(width), 1.0f / float(height));
	XMFLOAT2 uvQuadSize = XMFLOAT2(1.0f / float(width), 1.0f / float(height));

	for (uint32 x = 0; x <= width; x++) {
		for (uint32 y = 0; y <= height; y++) {
			int32 v = ((1 + height) * x) + y;
			data.Vertices[v].Position = XMFLOAT3(0.5f - (quadSize.x * x), 0.5f - (quadSize.y * y), 0.0f);
			// TODO: Fix vertices so normal is positive
			data.Vertices[v].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			data.Vertices[v].Tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
			data.Vertices[v].TexCoord = XMFLOAT2(0.0f + (uvQuadSize.x * x), 0.0f + (uvQuadSize.y * y));
		}
	}

	for (uint8 x = 0; x < width; x++) {
		for (uint8 y = 0; y < height; y++) {
			int32 quad = (height * x) + y;
			data.Indices[(quad * 6) + 0] = (x * (1 + height)) + y + 1;
			data.Indices[(quad * 6) + 1] = (data.Indices[quad * 6] + 2 + (height - 1));
			data.Indices[(quad * 6) + 2] = data.Indices[quad * 6] - 1;
			data.Indices[(quad * 6) + 3] = data.Indices[(quad * 6) + 1];
			data.Indices[(quad * 6) + 4] = data.Indices[(quad * 6) + 1] - 1;
			data.Indices[(quad * 6) + 5] = data.Indices[(quad * 6) + 2];
		}
	}

	data.Vertices.ShrinkToFit();
	data.Indices.ShrinkToFit();

	return data;
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreateSphere(uint32 subdivisions, float radius) noexcept {

	Main::CEMeshData Sphere;
	Sphere.Vertices.Resize(12);

	float T = (1.0f + sqrt(5.0f)) / 2.0f;
	Sphere.Vertices[0].Position = XMFLOAT3(-1.0f, T, 0.0f);
	Sphere.Vertices[1].Position = XMFLOAT3(1.0f, T, 0.0f);
	Sphere.Vertices[2].Position = XMFLOAT3(-1.0f, -T, 0.0f);
	Sphere.Vertices[3].Position = XMFLOAT3(1.0f, -T, 0.0f);
	Sphere.Vertices[4].Position = XMFLOAT3(0.0f, -1.0f, T);
	Sphere.Vertices[5].Position = XMFLOAT3(0.0f, 1.0f, T);
	Sphere.Vertices[6].Position = XMFLOAT3(0.0f, -1.0f, -T);
	Sphere.Vertices[7].Position = XMFLOAT3(0.0f, 1.0f, -T);
	Sphere.Vertices[8].Position = XMFLOAT3(T, 0.0f, -1.0f);
	Sphere.Vertices[9].Position = XMFLOAT3(T, 0.0f, 1.0f);
	Sphere.Vertices[10].Position = XMFLOAT3(-T, 0.0f, -1.0f);
	Sphere.Vertices[11].Position = XMFLOAT3(-T, 0.0f, 1.0f);

	Sphere.Indices =
	{
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,

		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,

		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,

		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1,
	};

	if (subdivisions > 0) {
		Subdivide(Sphere, subdivisions);
	}

	for (uint32 i = 0; i < static_cast<uint32>(Sphere.Vertices.Size()); i++) {
		// Calculate the new position, normal and tangent
		XMVECTOR Position = XMLoadFloat3(&Sphere.Vertices[i].Position);
		Position = XMVector3Normalize(Position);
		XMStoreFloat3(&Sphere.Vertices[i].Normal, Position);

		Position = XMVectorScale(Position, radius);
		XMStoreFloat3(&Sphere.Vertices[i].Position, Position);

		// Calculate uvs
		Sphere.Vertices[i].TexCoord.y = (asin(Sphere.Vertices[i].Position.y) / XM_PI) + 0.5f;
		Sphere.Vertices[i].TexCoord.x = (atan2f(Sphere.Vertices[i].Position.z, Sphere.Vertices[i].Position.x) + XM_PI) /
			(2.0f * XM_PI);
	}

	Sphere.Indices.ShrinkToFit();
	Sphere.Vertices.ShrinkToFit();

	CalculateTangent(Sphere);

	return Sphere;
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::
CreateCone(uint32 sides, float radius, float height) noexcept {
	return {};
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreatePyramid() noexcept {
	return {};
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreateCylinder(uint32 sides, float radius,
                                                                          float height) noexcept {
	return {};
}

void CEDXMeshManager::Subdivide(Main::CEMeshData& data, uint32 subdivisions) noexcept {
	if (subdivisions < 1) {
		return;
	}

	Main::CEVertex TempVertices[3];
	uint32 IndexCount = 0;
	uint32 VertexCount = 0;
	uint32 OldVertexCount = 0;
	data.Vertices.Reserve((data.Vertices.Size() * static_cast<uint32>(pow(2, subdivisions))));
	data.Indices.Reserve((data.Indices.Size() * static_cast<uint32>(pow(4, subdivisions))));

	for (uint32 i = 0; i < subdivisions; i++) {
		OldVertexCount = uint32(data.Vertices.Size());
		IndexCount = uint32(data.Indices.Size());
		for (uint32 j = 0; j < IndexCount; j += 3) {
			// Calculate Position
			XMVECTOR Position0 = XMLoadFloat3(&data.Vertices[data.Indices[j]].Position);
			XMVECTOR Position1 = XMLoadFloat3(&data.Vertices[data.Indices[j + 1]].Position);
			XMVECTOR Position2 = XMLoadFloat3(&data.Vertices[data.Indices[j + 2]].Position);

			XMVECTOR Position = XMVectorAdd(Position0, Position1);
			Position = XMVectorScale(Position, 0.5f);
			XMStoreFloat3(&TempVertices[0].Position, Position);

			Position = XMVectorAdd(Position0, Position2);
			Position = XMVectorScale(Position, 0.5f);
			XMStoreFloat3(&TempVertices[1].Position, Position);

			Position = XMVectorAdd(Position1, Position2);
			Position = XMVectorScale(Position, 0.5f);
			XMStoreFloat3(&TempVertices[2].Position, Position);

			// Calculate TexCoord
			XMVECTOR TexCoord0 = XMLoadFloat2(&data.Vertices[data.Indices[j]].TexCoord);
			XMVECTOR TexCoord1 = XMLoadFloat2(&data.Vertices[data.Indices[j + 1]].TexCoord);
			XMVECTOR TexCoord2 = XMLoadFloat2(&data.Vertices[data.Indices[j + 2]].TexCoord);

			XMVECTOR TexCoord = XMVectorAdd(TexCoord0, TexCoord1);
			TexCoord = XMVectorScale(TexCoord, 0.5f);
			XMStoreFloat2(&TempVertices[0].TexCoord, TexCoord);

			TexCoord = XMVectorAdd(TexCoord0, TexCoord2);
			TexCoord = XMVectorScale(TexCoord, 0.5f);
			XMStoreFloat2(&TempVertices[1].TexCoord, TexCoord);

			TexCoord = XMVectorAdd(TexCoord1, TexCoord2);
			TexCoord = XMVectorScale(TexCoord, 0.5f);
			XMStoreFloat2(&TempVertices[2].TexCoord, TexCoord);

			// Calculate Normal
			XMVECTOR Normal0 = XMLoadFloat3(&data.Vertices[data.Indices[j]].Normal);
			XMVECTOR Normal1 = XMLoadFloat3(&data.Vertices[data.Indices[j + 1]].Normal);
			XMVECTOR Normal2 = XMLoadFloat3(&data.Vertices[data.Indices[j + 2]].Normal);

			XMVECTOR Normal = XMVectorAdd(Normal0, Normal1);
			Normal = XMVectorScale(Normal, 0.5f);
			Normal = XMVector3Normalize(Normal);
			XMStoreFloat3(&TempVertices[0].Normal, Normal);

			Normal = XMVectorAdd(Normal0, Normal2);
			Normal = XMVectorScale(Normal, 0.5f);
			Normal = XMVector3Normalize(Normal);
			XMStoreFloat3(&TempVertices[1].Normal, Normal);

			Normal = XMVectorAdd(Normal1, Normal2);
			Normal = XMVectorScale(Normal, 0.5f);
			Normal = XMVector3Normalize(Normal);
			XMStoreFloat3(&TempVertices[2].Normal, Normal);

			// Calculate Tangent
			XMVECTOR Tangent0 = XMLoadFloat3(&data.Vertices[data.Indices[j]].Tangent);
			XMVECTOR Tangent1 = XMLoadFloat3(&data.Vertices[data.Indices[j + 1]].Tangent);
			XMVECTOR Tangent2 = XMLoadFloat3(&data.Vertices[data.Indices[j + 2]].Tangent);

			XMVECTOR Tangent = XMVectorAdd(Tangent0, Tangent1);
			Tangent = XMVectorScale(Tangent, 0.5f);
			Tangent = XMVector3Normalize(Tangent);
			XMStoreFloat3(&TempVertices[0].Tangent, Tangent);

			Tangent = XMVectorAdd(Tangent0, Tangent2);
			Tangent = XMVectorScale(Tangent, 0.5f);
			Tangent = XMVector3Normalize(Tangent);
			XMStoreFloat3(&TempVertices[1].Tangent, Tangent);

			Tangent = XMVectorAdd(Tangent1, Tangent2);
			Tangent = XMVectorScale(Tangent, 0.5f);
			Tangent = XMVector3Normalize(Tangent);
			XMStoreFloat3(&TempVertices[2].Tangent, Tangent);

			// Push the new Vertices
			data.Vertices.EmplaceBack(TempVertices[0]);
			data.Vertices.EmplaceBack(TempVertices[1]);
			data.Vertices.EmplaceBack(TempVertices[2]);

			// Push index of the new triangles
			VertexCount = uint32(data.Vertices.Size());
			data.Indices.EmplaceBack(VertexCount - 3);
			data.Indices.EmplaceBack(VertexCount - 1);
			data.Indices.EmplaceBack(VertexCount - 2);

			data.Indices.EmplaceBack(VertexCount - 3);
			data.Indices.EmplaceBack(data.Indices[j + 1]);
			data.Indices.EmplaceBack(VertexCount - 1);

			data.Indices.EmplaceBack(VertexCount - 2);
			data.Indices.EmplaceBack(VertexCount - 1);
			data.Indices.EmplaceBack(data.Indices[j + 2]);

			// Reassign the old indexes
			data.Indices[j + 1] = VertexCount - 3;
			data.Indices[j + 2] = VertexCount - 2;
		}

		Optimize(data, OldVertexCount);
	}

	data.Vertices.ShrinkToFit();
	data.Indices.ShrinkToFit();
}

void CEDXMeshManager::Optimize(Main::CEMeshData& data, uint32 startVertex) noexcept {
	uint32 VertexCount = static_cast<uint32>(data.Vertices.Size());
	uint32 IndexCount = static_cast<uint32>(data.Indices.Size());

	uint32 k = 0;
	uint32 j = 0;
	for (uint32 i = startVertex; i < VertexCount; i++) {
		for (j = 0; j < VertexCount; j++) {
			if (data.Vertices[i] == data.Vertices[j]) {
				if (i != j) {
					data.Vertices.Erase(data.Vertices.Begin() + i);
					VertexCount--;
					j--;

					for (k = 0; k < IndexCount; k++) {
						if (data.Indices[k] == i) {
							data.Indices[k] = j;
						}
						else if (data.Indices[k] > i) {
							data.Indices[k]--;
						}
					}

					i--;
					break;
				}
			}
		}
	}
}

void CEDXMeshManager::CalculateHardNormals(Main::CEMeshData& data) noexcept {
}

void CEDXMeshManager::CalculateTangent(Main::CEMeshData& data) noexcept {
	auto CalculateTangentFromVectors = [](Main::CEVertex& Vertex1,
	                                      const Main::CEVertex& Vertex2,
	                                      const Main::CEVertex& Vertex3) {
		XMFLOAT3 Edge1;
		Edge1.x = Vertex2.Position.x - Vertex1.Position.x;
		Edge1.y = Vertex2.Position.y - Vertex1.Position.y;
		Edge1.z = Vertex2.Position.z - Vertex1.Position.z;

		XMFLOAT3 Edge2;
		Edge2.x = Vertex3.Position.x - Vertex1.Position.x;
		Edge2.y = Vertex3.Position.y - Vertex1.Position.y;
		Edge2.z = Vertex3.Position.z - Vertex1.Position.z;

		XMFLOAT2 UVEdge1;
		UVEdge1.x = Vertex2.TexCoord.x - Vertex1.TexCoord.x;
		UVEdge1.y = Vertex2.TexCoord.y - Vertex1.TexCoord.y;

		XMFLOAT3 UVEdge2;
		UVEdge2.x = Vertex3.TexCoord.x - Vertex1.TexCoord.x;
		UVEdge2.y = Vertex3.TexCoord.y - Vertex1.TexCoord.y;

		float Denominator = 1.0f / (UVEdge1.x * UVEdge2.y - UVEdge2.x * UVEdge1.y);

		XMFLOAT3 Tangent;
		Tangent.x = Denominator * (UVEdge2.y * Edge1.x - UVEdge1.y * Edge2.x);
		Tangent.y = Denominator * (UVEdge2.y * Edge1.y - UVEdge1.y * Edge2.y);
		Tangent.z = Denominator * (UVEdge2.y * Edge1.z - UVEdge1.y * Edge2.z);

		float Length = std::sqrt((Tangent.x * Tangent.x) + (Tangent.y * Tangent.y) + (Tangent.z * Tangent.z));
		if (Length != 0.0f) {
			Tangent.x /= Length;
			Tangent.y /= Length;
			Tangent.z /= Length;
		}

		Vertex1.Tangent = Tangent;
	};

	for (uint32 i = 0; i < data.Indices.Size(); i += 3) {
		Main::CEVertex& Vertex1 = data.Vertices[data.Indices[i + 0]];
		Main::CEVertex& Vertex2 = data.Vertices[data.Indices[i + 1]];
		Main::CEVertex& Vertex3 = data.Vertices[data.Indices[i + 2]];

		CalculateTangentFromVectors(Vertex1, Vertex2, Vertex3);
		CalculateTangentFromVectors(Vertex2, Vertex3, Vertex1);
		CalculateTangentFromVectors(Vertex3, Vertex1, Vertex2);
	}
}
