RaytracingAccelerationStructure rtScene : register(t0);
RWTexture2D<float4> output : register(u0);

cbuffer PerFrame : register(b0) {
float3 A;
float3 B;
float3 C;
}

float3 linearToSRGB(float3 c) {
	// Based on http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
	float3 sq1 = sqrt(c);
	float3 sq2 = sqrt(sq1);
	float3 sq3 = sqrt(sq2);
	float3 srgb = 0.662002687 * sq1 + 0.684122060 * sq2 - 0.323583601 * sq3 - 0.0225411470 * c;
	return srgb;
}

struct RayPayload {
	float3 color;
};

[shader("raygeneration")]
void rayGen() {
	uint3 launchIndex = DispatchRaysIndex();
	uint3 launchDim = DispatchRaysDimensions();

	float2 crd = float2(launchIndex.xy);
	float2 dims = float2(launchDim.xy);

	float2 d = ((crd / dims) * 2.0f - 1.0f);
	float aspectRatio = dims.x / dims.y;

	RayDesc ray;
	ray.Origin = float3(0, 0, -2);
	ray.Direction = normalize(float3(d.x * aspectRatio, -d.y, 1));

	RayPayload payload;
	TraceRay(rtScene, 0, 0xFF, 0, 2, 0, ray, payload);
	float3 col = linearToSRGB(payload.color);
	output[launchIndex.xy] = float4(col, 1);
}

[shader("miss")]
void miss(inout RayPayload payload) {
	payload.color = float3(0.4, 0.6, 0.2);
}

[shader("closesthit")]
void triangleChs(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attributes) {
	float3 baryCentrics = float3(
		1.0 - attributes.barycentrics.x - attributes.barycentrics.y,
		attributes.barycentrics.x,
		attributes.barycentrics.y
	);
	payload.color = A * baryCentrics.x + B * baryCentrics.y + C * baryCentrics.z;
}

struct ShadowPayload {
	bool hit;
};

[shader("closesthit")]
void planeChs(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attributes) {
	float hitT = RayTCurrent();
	float3 rayDirW = WorldRayDirection();
	float3 rayOriginW = WorldRayOrigin();

	//Find world-space hit position
	float3 posW = rayOriginW + hitT * rayDirW;

	//Fire shadow ray. Direction is hard-coded here, but can be fetched from a constant-buffer
	RayDesc ray;
	ray.Origin = posW;
	ray.Direction = normalize(float3(0.5, 0.5, -0.5));
	ray.TMin = 0.01;
	ray.TMax = 100000;
	ShadowPayload shadowPayload;
	TraceRay(rtScene, 0, 0xFF, 1, 0, 1, ray, shadowPayload);

	float factor = shadowPayload.hit ? 0.1 : 1.0;
	payload.color = float4(0.9f, 0.9f, 0.9f, 1.0f) * factor;
}

[shader("closesthit")]
void shadowChs(inout ShadowPayload payload, in BuiltInTriangleIntersectionAttributes attributes) {
	payload.hit = true;
}

[shader("miss")]
void shadowMiss(inout ShadowPayload payload) {
	payload.hit = false;
}
