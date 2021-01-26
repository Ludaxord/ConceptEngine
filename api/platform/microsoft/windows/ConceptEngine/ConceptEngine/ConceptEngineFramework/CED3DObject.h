#pragma once
#include <DirectXMath.h>
#include <intsafe.h>


#include "CEObject.h"

namespace Concept::GraphicsEngine::Direct3D::Objects {
	struct CED3DVertexPosColor {
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Color;
	};

	class CED3DObject : public CEObject<CED3DVertexPosColor, WORD> {
	public:
		CED3DObject(VertexList& vertices, IndexList& indices, FieldOfView fieldOfView)
			: CEObject<CED3DVertexPosColor, WORD>(vertices, indices, fieldOfView) {
		}

		MainVertexList GetVertexPosColor() override;
		MainIndexList GetVertexIndices() override;

	protected:
	private:

	};

	inline CEObject<CED3DVertexPosColor, unsigned short>::MainVertexList CED3DObject::GetVertexPosColor() {
		MainVertexList mainVertexList;
		auto vertexList = GetVertices();
		for (auto& vertex : vertexList) {
			mainVertexList.push_back({
				{vertex.Position.x, vertex.Position.y, vertex.Position.z},
				{vertex.Color.x, vertex.Color.y, vertex.Color.z}
			});
		}

		return mainVertexList;
	}

	inline CEObject<CED3DVertexPosColor, unsigned short>::MainIndexList CED3DObject::GetVertexIndices() {
		MainIndexList mainIndexList;
		auto indexList = GetIndices();
		for (auto& index : indexList) {
			mainIndexList.push_back(static_cast<uint32_t>(index));
		}
		return mainIndexList;
	}
}
