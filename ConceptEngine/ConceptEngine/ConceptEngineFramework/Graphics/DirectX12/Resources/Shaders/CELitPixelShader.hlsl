#include "LightUtil.hlsl"
#include "VertexHeader.h"


cbuffer cbPerObject : register(b0)
{
float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
float4 gDiffuseAlbedo;
float3 gFresnelR0;
float gRoughness;
float4x4 gMatTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b2)
{
float4x4 gView;
float4x4 gInvView;
float4x4 gProj;
float4x4 gInvProj;
float4x4 gViewProj;
float4x4 gInvViewProj;
float3 gEyePosW;
float cbPerObjectPad1;
float2 gRenderTargetSize;
float2 gInvRenderTargetSize;
float gNearZ;
float gFarZ;
float gTotalTime;
float gDeltaTime;
float4 gAmbientLight;

// Indices [0, NUM_DIR_LIGHTS) are directional lights;
// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
// are spot lights for a maximum of MaxLights per object.
Light gLights[MaxLights];
};

float4 PS(LitVertexOut pIn) : SV_Target {
	// Interpolating normal can unnormalize it, so renormalize it.
	pIn.NormalW = normalize(pIn.NormalW);

	//Vector from point being lit to eye
	float3 toEyeW = normalize(gEyePosW - pIn.PosW);

	//Indirect lighting
	float4 ambient = gAmbientLight * gDiffuseAlbedo;

	const float shininess = 1.0f - gRoughness;
	Material mat = {gDiffuseAlbedo, gFresnelR0, shininess};
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pIn.PosW, pIn.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	//Common convertion to take alpha from diffuse material.
	litColor.a = gDiffuseAlbedo.a;

	return litColor;
}
