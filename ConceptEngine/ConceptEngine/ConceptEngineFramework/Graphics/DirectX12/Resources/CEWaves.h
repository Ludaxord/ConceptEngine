#pragma once
#include "CETypes.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {

	class CEWaves final : CEObject {

	public:
		CEWaves(int m, int n, float dx, float dt, float speed, float damping);
		CEWaves(const CEWaves& rhs) = delete;
		CEWaves& operator=(const CEWaves& rhs) = delete;
		~CEWaves();
		
		const DirectX::XMFLOAT3& Position(int i) const override;
		const DirectX::XMFLOAT3& Normal(int i) const override;
		const DirectX::XMFLOAT3& TangentX(int i) const override;

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
