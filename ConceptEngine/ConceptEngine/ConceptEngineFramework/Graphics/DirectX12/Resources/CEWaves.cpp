#include "CEWaves.h"
#include <ppl.h>
#include <algorithm>
#include <vector>
#include <cassert>

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CEWaves::CEWaves(int m, int n, float dx, float dt, float speed, float damping) {
	m_numRows = m;
	m_numCols = n;

	mVertexCount = m * n;
	mTriangleCount = (m - 1) * (n - 1) * 2;

	mTimeStep = dt;
	mSpatialStep = dx;

	float d = damping * dt + 2.0f;
	float e = (speed * speed) * (dt * dt) / (dx * dx);
	mK1 = (damping * dt - 2.0f) / d;
	mK2 = (4.0f - 8.0f * e) / d;
	mK3 = (2.0f * e) / d;

	mPrevSolution.resize(m * n);
	mCurrSolution.resize(m * n);
	mNormals.resize(m * n);
	mTangentX.resize(m * n);

	//Generate grid vertices in system memory
	float halfWidth = (n - 1) * dx * 0.5f;
	float halfDepth = (m - 1) * dx * 0.5f;

	for (int i = 0; i < m; ++i) {
		float z = halfDepth - i * dx;
		for (int j = 0; j < n; ++j) {
			float x = halfWidth + j * dx;

			mPrevSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
			mCurrSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
			mNormals[i * n + j] = XMFLOAT3(0.0f, 1.0f, 0.0f);
			mTangentX[i * n + j] = XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
	}
}

CEWaves::~CEWaves() {
}

const XMFLOAT3& CEWaves::Position(int i) const {
	return CEObject::Position(i);
}

const XMFLOAT3& CEWaves::Normal(int i) const {
	return CEObject::Normal(i);
}

const XMFLOAT3& CEWaves::TangentX(int i) const {
	return CEObject::TangentX(i);
}

int CEWaves::RowCount() const {
	return m_numRows;
}

int CEWaves::ColumnCount() const {
	return m_numCols;
}

int CEWaves::VertexCount() const {
	return mVertexCount;
}

int CEWaves::TriangleCount() const {
	return mTriangleCount;
}

float CEWaves::Width() const {
	return m_numCols * mSpatialStep;
}

float CEWaves::Depth() const {
	return m_numRows * mSpatialStep;
}

void CEWaves::Update(float dt) {
	static float t = 0;

	//Accumulate time;
	t += dt;

	//Only update simulation at specified time step
	if (t >= mTimeStep) {
		//Only update interior points; we use zero boundary conditions.
		//Reference: https://docs.microsoft.com/en-us/cpp/parallel/concrt/parallel-algorithms?view=msvc-160#parallel_for
		concurrency::parallel_for(1, m_numRows - 1, [this](int i) {
			for (int j = 1; j < m_numCols - 1; ++j) {
				/*
				 * After update we will be discarding previous buffer,
				 * so overwrite that buffer with new update.
				 * NOTE: how we can do this inplace (read/write to same element)
				 * because we will not need previous buffer again and assignment happens last
				 *
				 * Note: j indexes x and i indexes z: h(x_j, z_i, t_k)
				 * Moreover, our +z axis goes "down";
				 * this is just to keep consistent with our row indices going down
				 */
				mPrevSolution[i * m_numCols + j].y =
					mK1 * mPrevSolution[i * m_numCols + j].y +
					mK2 * mCurrSolution[i * m_numCols + j].y +
					mK3 * (
						mCurrSolution[(i + 1) * m_numCols + j].y +
						mCurrSolution[(i - 1) * m_numCols + j].y +
						mCurrSolution[i * m_numCols + j + 1].y +
						mCurrSolution[i * m_numCols + j - 1].y
					);
			}
		});

		/*
		 * We just overwrote previous buffer with new data,
		 * so this data needs to become current solution and old current solution becomes new previous solution
		 */
		std::swap(mPrevSolution, mCurrSolution);

		t = 0.0f;

		/*
		 * Compute normals using finite difference scheme
		 */
		concurrency::parallel_for(1, m_numRows - 1, [this](int i) {
			for (int j = 1; j < m_numCols - 1; ++j) {
				float l = mCurrSolution[i * m_numCols + j - 1].y;
				float r = mCurrSolution[i * m_numCols + j + 1].y;
				float t = mCurrSolution[(i - 1) * m_numCols + j].y;
				float b = mCurrSolution[(i + 1) * m_numCols + j].y;

				mNormals[i * m_numCols + j].x = -r + l;
				mNormals[i * m_numCols + j].y = 2.0f * mSpatialStep;
				mNormals[i * m_numCols + j].z = b - t;

				XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mNormals[i * m_numCols + j]));
				XMStoreFloat3(&mNormals[i * m_numCols + j], n);

				mTangentX[i * m_numCols + j] = XMFLOAT3(2.0f * mSpatialStep, r - l, 0.0f);
				XMVECTOR T = XMVector3Normalize(XMLoadFloat3(&mTangentX[i * m_numCols + j]));
				XMStoreFloat3(&mTangentX[i * m_numCols + j], T);
			}
		});
	}
}

void CEWaves::Disturb(int i, int j, float magnitude) {

	//Do not disturb boundaries
	assert(j > 1 && j < m_numRows - 2);
	assert(j > 1 && j < m_numCols - 2);

	const float halfMagnitude = 0.5f * magnitude;

	//Disturb vertex height and its neighbors
	mCurrSolution[m_numCols * i + j].y += magnitude;
	mCurrSolution[m_numCols * i + j + 1].y += halfMagnitude;
	mCurrSolution[m_numCols * i + j - 1].y += halfMagnitude;
	mCurrSolution[m_numCols * (i + 1) + j].y += halfMagnitude;
	mCurrSolution[m_numCols * (i - 1) + j].y += halfMagnitude;
}
