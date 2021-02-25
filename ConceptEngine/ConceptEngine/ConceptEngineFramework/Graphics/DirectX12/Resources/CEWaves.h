#pragma once
#include "CETypes.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {

	class CEWaves final : CEObject {

	public:
		CEWaves(int m, int n, float dx, float dt, float speed, float damping);
		CEWaves(const CEWaves& rhs) = delete;
		CEWaves& operator=(const CEWaves& rhs) = delete;
		~CEWaves();

		int RowCount() const override;
		int ColumnCount() const override;
		int VertexCount() const override;
		int TriangleCount() const override;
		float Width() const override;
		float Depth() const override;
		void Update(float dt) override;
		void Disturb(int i, int j, float magnitude) override;
	};

}
