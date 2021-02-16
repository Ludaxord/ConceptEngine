#ifndef RAYTRACING_HLSL
#define RAYTRACING_HLSL

#include "BoxRayTracingHlslCompat.h"

RaytracingAccelerationStructure Scene : register(t0, space0);
RWTexture2D<float4> RenderTarget : register(u0);
ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b0);

typedef BuiltInTriangleIntersectionAttributes Attributes;

struct RayPayload {
	float4 color;
};

bool IsInsideViewPort(float2 p, Viewport viewport) {
	return (
		p.x >= viewport.left &&
		p.x <= viewport.right &&
		p.y >= viewport.top &&
		p.y <= viewport.bottom
	);
}

[shader("raygeneration")]
void BoxRayGenShader() {
	float2 lerpValues = (float2)DispatchRaysIndex() / (float2)DispatchRaysDimensions();

	//Orthographic projection since we are RayTracing in scene space
	float3 rayDir = float3(0, 0, 1);
	float3 origin = float3(
		lerp(g_rayGenCB.viewport.left, g_rayGenCB.viewport.right, lerpValues.x),
		lerp(g_rayGenCB.viewport.top, g_rayGenCB.viewport.bottom, lerpValues.y),
		0.0f
	);

	if (IsInsideViewPort(origin.xy, g_rayGenCB.stencil)) {
		//Trace ray
		//Set ray's extents
		RayDesc ray;
		ray.Origin = origin;
		ray.Direction = rayDir;
		//Set TMIN to non-zero small value to avoid aliasing issues due to floating - point errors
		//Tmin should be kept small to prevent missing geometry at close contact areas
		ray.TMin = 0.001;
		ray.TMax = 10000.0;
		RayPayload payload = {float4(0, 0, 0, 0)};
		TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);

		//Write rayTraced color to output texture
		RenderTarget[DispatchRaysDimensions().xy] = payload.color;
	}
	else {
		//Render interpolated DispatchRaysIndex outside stencil window
		RenderTarget[DispatchRaysIndex().xy] = float4(lerpValues, 0, 1);
	}
}

[shader("closesthit")]
void BoxClosestHitShader(inout RayPayload payload, in Attributes attrs) {
	float3 baryCentrics = float3(1 - attrs.barycentrics.x - attrs.barycentrics.y, attrs.barycentrics.x,
	                             attrs.barycentrics.y);
	payload.color = float4(baryCentrics, 1);
}

[shader("miss")]
void BoxMissShader(inout RayPayload payload) {
	payload.color = float4(0, 0, 0, 1);
}

#endif
