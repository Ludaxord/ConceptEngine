#include "CEMath.h"

using namespace DirectX;
const float CEMath::Infinity = FLT_MAX;
const float CEMath::Pi = 3.1415926535f;

float CEMath::AngleFromXY(float x, float y) {
	float theta = 0.0f;
	if (x >= 0.0f) {
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]
		if (theta < 0.0f)
			theta += 2.0f * Pi;
	}
	else {
		theta = atanf(y / x) + Pi;
	}
	return theta;
}

XMVECTOR CEMath::RandUnitVec3() {
	XMVECTOR Ones = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	auto Zeroes = XMVectorZero();

	while (true) {
		XMVECTOR v = XMVectorSet(CEMath::RandF(-1.0f, 1.0f), CEMath::RandF(-1.0f, 1.0f), CEMath::RandF(-1.0f, 1.0f),
		                         0.0f);
		if (XMVector3Greater(XMVector3LengthSq(v), Ones)) {
			continue;
		}
		return XMVector3Normalize(v);
	}
}

XMVECTOR CEMath::RandHemisphereUnitVec3(DirectX::XMVECTOR n) {
	auto Ones = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	auto Zeroes = XMVectorZero();

	while (true) {
		XMVECTOR v = XMVectorSet(CEMath::RandF(-1.0f, 1.0f), CEMath::RandF(-1.0f, 1.0f), CEMath::RandF(-1.0f, 1.0f),
		                         0.0f);
		if (XMVector3Greater(XMVector3LengthSq(v), Ones)) {
			continue;
		}
		if (XMVector3Less(XMVector3Dot(n, v), Zeroes)) {
			continue;
		}

		return XMVector3Normalize(v);
	}
}
