#include "CEFrustum.h"
#include "../../Core/Common/CETypes.h"

using namespace ConceptEngine::Render::Scene;

CEFrustum::CEFrustum(float screenDepth,
                     const CEMatrixFloat4X4& view,
                     const CEMatrixFloat4X4& projection): Planes() {
	Create(screenDepth, view, projection);
}

void CEFrustum::Create(float farPlane,
                       const CEMatrixFloat4X4& view,
                       const CEMatrixFloat4X4& projection) {

#if WINDOWS_PLATFORM
	XMFLOAT4X4 tempProjection = projection.Native;
	float MinimumZ = -tempProjection._43 / tempProjection._33;
	float R = farPlane / (farPlane - MinimumZ);
	tempProjection._33 = R;
	tempProjection._43 = -R * MinimumZ;

	XMMATRIX XmView = XMMatrixTranspose(XMLoadFloat4x4(&view.Native));
	XMMATRIX XmProjection = XMLoadFloat4x4(&tempProjection);
	XMMATRIX XmMatrix = XMMatrixMultiply(XmView, XmProjection);
	XMFLOAT4X4 Matrix;
	XMStoreFloat4x4(&Matrix, XmMatrix);

	XMVECTOR XmPlanes[6];
	Planes[0].x = Matrix._14 + Matrix._13;
	Planes[0].y = Matrix._24 + Matrix._23;
	Planes[0].z = Matrix._34 + Matrix._33;
	Planes[0].w = Matrix._44 + Matrix._43;
	XmPlanes[0] = XMLoadFloat4(&Planes[0].Native);
	XmPlanes[0] = XMPlaneNormalize(XmPlanes[0]);
	XMStoreFloat4(&Planes[0].Native, XmPlanes[0]);

	// Calculate far plane of frustum.
	Planes[1].x = Matrix._14 - Matrix._13;
	Planes[1].y = Matrix._24 - Matrix._23;
	Planes[1].z = Matrix._34 - Matrix._33;
	Planes[1].w = Matrix._44 - Matrix._43;
	XmPlanes[1] = XMLoadFloat4(&Planes[1].Native);
	XmPlanes[1] = XMPlaneNormalize(XmPlanes[1]);
	XMStoreFloat4(&Planes[1].Native, XmPlanes[1]);

	// Calculate left plane of frustum.
	Planes[2].x = Matrix._14 + Matrix._11;
	Planes[2].y = Matrix._24 + Matrix._21;
	Planes[2].z = Matrix._34 + Matrix._31;
	Planes[2].w = Matrix._44 + Matrix._41;
	XmPlanes[2] = XMLoadFloat4(&Planes[2].Native);
	XmPlanes[2] = XMPlaneNormalize(XmPlanes[2]);
	XMStoreFloat4(&Planes[2].Native, XmPlanes[2]);

	// Calculate right plane of frustum.
	Planes[3].x = Matrix._14 - Matrix._11;
	Planes[3].y = Matrix._24 - Matrix._21;
	Planes[3].z = Matrix._34 - Matrix._31;
	Planes[3].w = Matrix._44 - Matrix._41;
	XmPlanes[3] = XMLoadFloat4(&Planes[3].Native);
	XmPlanes[3] = XMPlaneNormalize(XmPlanes[3]);
	XMStoreFloat4(&Planes[3].Native, XmPlanes[3]);

	// Calculate top plane of frustum.
	Planes[4].x = Matrix._14 - Matrix._12;
	Planes[4].y = Matrix._24 - Matrix._22;
	Planes[4].z = Matrix._34 - Matrix._32;
	Planes[4].w = Matrix._44 - Matrix._42;
	XmPlanes[4] = XMLoadFloat4(&Planes[4].Native);
	XmPlanes[4] = XMPlaneNormalize(XmPlanes[4]);
	XMStoreFloat4(&Planes[4].Native, XmPlanes[4]);

	// Calculate bottom plane of frustum.
	Planes[5].x = Matrix._14 + Matrix._12;
	Planes[5].y = Matrix._24 + Matrix._22;
	Planes[5].z = Matrix._34 + Matrix._32;
	Planes[5].w = Matrix._44 + Matrix._42;
	XmPlanes[5] = XMLoadFloat4(&Planes[5].Native);
	XmPlanes[5] = XMPlaneNormalize(XmPlanes[5]);
	XMStoreFloat4(&Planes[5].Native, XmPlanes[5]);
#endif


}

CEFrustum::CEFrustum(float FarPlane, const DirectX::XMFLOAT4X4& View, const DirectX::XMFLOAT4X4& Projection) {
	Create(FarPlane, View, Projection);
}

void CEFrustum::Create(float FarPlane, const DirectX::XMFLOAT4X4& View, const DirectX::XMFLOAT4X4& Projection) {
	XMFLOAT4X4 TempProjection = Projection;
	// Calculate the minimum Z distance in the frustum.
	float MinimumZ = -TempProjection._43 / TempProjection._33;
	float R = FarPlane / (FarPlane - MinimumZ);
	TempProjection._33 = R;
	TempProjection._43 = -R * MinimumZ;

	// Create the frustum Matrix from the view Matrix and updated projection Matrix.
	XMMATRIX XmView = XMMatrixTranspose(XMLoadFloat4x4(&View));
	XMMATRIX XmProjection = XMLoadFloat4x4(&TempProjection);
	XMMATRIX XmMatrix = XMMatrixMultiply(XmView, XmProjection);
	XMFLOAT4X4 Matrix;
	XMStoreFloat4x4(&Matrix, XmMatrix);

	// Calculate near plane of frustum.
	XMVECTOR XmPlanes[6];
	Planes[0].x = Matrix._14 + Matrix._13;
	Planes[0].y = Matrix._24 + Matrix._23;
	Planes[0].z = Matrix._34 + Matrix._33;
	Planes[0].w = Matrix._44 + Matrix._43;
	XmPlanes[0] = XMLoadFloat4(&Planes[0].Native);
	XmPlanes[0] = XMPlaneNormalize(XmPlanes[0]);
	XMStoreFloat4(&Planes[0].Native, XmPlanes[0]);

	// Calculate far plane of frustum.
	Planes[1].x = Matrix._14 - Matrix._13;
	Planes[1].y = Matrix._24 - Matrix._23;
	Planes[1].z = Matrix._34 - Matrix._33;
	Planes[1].w = Matrix._44 - Matrix._43;
	XmPlanes[1] = XMLoadFloat4(&Planes[1].Native);
	XmPlanes[1] = XMPlaneNormalize(XmPlanes[1]);
	XMStoreFloat4(&Planes[1].Native, XmPlanes[1]);

	// Calculate left plane of frustum.
	Planes[2].x = Matrix._14 + Matrix._11;
	Planes[2].y = Matrix._24 + Matrix._21;
	Planes[2].z = Matrix._34 + Matrix._31;
	Planes[2].w = Matrix._44 + Matrix._41;
	XmPlanes[2] = XMLoadFloat4(&Planes[2].Native);
	XmPlanes[2] = XMPlaneNormalize(XmPlanes[2]);
	XMStoreFloat4(&Planes[2].Native, XmPlanes[2]);

	// Calculate right plane of frustum.
	Planes[3].x = Matrix._14 - Matrix._11;
	Planes[3].y = Matrix._24 - Matrix._21;
	Planes[3].z = Matrix._34 - Matrix._31;
	Planes[3].w = Matrix._44 - Matrix._41;
	XmPlanes[3] = XMLoadFloat4(&Planes[3].Native);
	XmPlanes[3] = XMPlaneNormalize(XmPlanes[3]);
	XMStoreFloat4(&Planes[3].Native, XmPlanes[3]);

	// Calculate top plane of frustum.
	Planes[4].x = Matrix._14 - Matrix._12;
	Planes[4].y = Matrix._24 - Matrix._22;
	Planes[4].z = Matrix._34 - Matrix._32;
	Planes[4].w = Matrix._44 - Matrix._42;
	XmPlanes[4] = XMLoadFloat4(&Planes[4].Native);
	XmPlanes[4] = XMPlaneNormalize(XmPlanes[4]);
	XMStoreFloat4(&Planes[4].Native, XmPlanes[4]);

	// Calculate bottom plane of frustum.
	Planes[5].x = Matrix._14 + Matrix._12;
	Planes[5].y = Matrix._24 + Matrix._22;
	Planes[5].z = Matrix._34 + Matrix._32;
	Planes[5].w = Matrix._44 + Matrix._42;
	XmPlanes[5] = XMLoadFloat4(&Planes[5].Native);
	XmPlanes[5] = XMPlaneNormalize(XmPlanes[5]);
	XMStoreFloat4(&Planes[5].Native, XmPlanes[5]);
}

bool CEFrustum::CheckAABB(const CEAABB& boundingBox) {

#if WINDOWS_PLATFORM
	const XMFLOAT3 Center = boundingBox.GetCenter().Native;
	const float Width = boundingBox.GetWidth() / 2.0f;
	const float Height = boundingBox.GetHeight() / 2.0f;
	const float Depth = boundingBox.GetDepth() / 2.0f;

	XMVECTOR Coords[8];
	Coords[0] = XMVectorSet((Center.x - Width), (Center.y - Height), (Center.z - Depth), 1.0f);
	Coords[1] = XMVectorSet((Center.x + Width), (Center.y - Height), (Center.z - Depth), 1.0f);
	Coords[2] = XMVectorSet((Center.x - Width), (Center.y + Height), (Center.z - Depth), 1.0f);
	Coords[3] = XMVectorSet((Center.x + Width), (Center.y + Height), (Center.z - Depth), 1.0f);
	Coords[4] = XMVectorSet((Center.x - Width), (Center.y - Height), (Center.z + Depth), 1.0f);
	Coords[5] = XMVectorSet((Center.x + Width), (Center.y - Height), (Center.z + Depth), 1.0f);
	Coords[6] = XMVectorSet((Center.x - Width), (Center.y + Height), (Center.z + Depth), 1.0f);
	Coords[7] = XMVectorSet((Center.x + Width), (Center.y + Height), (Center.z + Depth), 1.0f);

	for (int32 Index = 0; Index < 6; Index++) {
		XMVECTOR Plane = XMLoadFloat4(&Planes[Index].Native);
		if (XMPlaneDotCoord(Plane, Coords[0]).m128_f32[0] >= 0.0f) {
			continue;
		}
		if (XMPlaneDotCoord(Plane, Coords[1]).m128_f32[0] >= 0.0f) {
			continue;
		}
		if (XMPlaneDotCoord(Plane, Coords[2]).m128_f32[0] >= 0.0f) {
			continue;
		}
		if (XMPlaneDotCoord(Plane, Coords[3]).m128_f32[0] >= 0.0f) {
			continue;
		}
		if (XMPlaneDotCoord(Plane, Coords[4]).m128_f32[0] >= 0.0f) {
			continue;
		}
		if (XMPlaneDotCoord(Plane, Coords[5]).m128_f32[0] >= 0.0f) {
			continue;
		}
		if (XMPlaneDotCoord(Plane, Coords[6]).m128_f32[0] >= 0.0f) {
			continue;
		}
		if (XMPlaneDotCoord(Plane, Coords[7]).m128_f32[0] >= 0.0f) {
			continue;
		}

		return false;
	}

	return true;
#else
	CE_LOG_ERROR("[CEFrustum] CheckAABB NOT IMPLEMENTED FOR THIS API...");
	
	return false;
#endif

}
