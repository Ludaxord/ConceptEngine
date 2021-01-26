#pragma once
#include <utility>
#include <vector>

namespace Concept::GraphicsEngine {

	struct CEVertexPosColor {
		struct VertexPosition {
			float x;
			float y;
			float z;
		} Position;

		struct VertexColor {
			float r;
			float g;
			float b;
		} Color;
	};

	template <typename Vertices, typename Indices>
	class CEObject {
	public:
		using VertexList = std::vector<Vertices>;
		using IndexList = std::vector<Indices>;
		using FieldOfView = float;

		using MainVertexList = std::vector<CEVertexPosColor>;
		using MainIndexList = std::vector<uint32_t>;

		virtual ~CEObject() = default;
		virtual MainVertexList GetVertexPosColor() = 0;
		virtual MainIndexList GetVertexIndices() = 0;

		VertexList GetVertices() {
			return g_vertices;
		};

		IndexList GetIndices() {
			return g_indices;
		};

		FieldOfView GetFieldOfView() const {
			return g_fieldOfView;
		};
	protected:

		CEObject(VertexList& vertices, IndexList& indices, FieldOfView fieldOfView):
			g_vertices(vertices),
			g_indices(indices),
			g_fieldOfView(fieldOfView) {
		}

	private:
		VertexList g_vertices;
		IndexList g_indices;
		FieldOfView g_fieldOfView;
	};
}
