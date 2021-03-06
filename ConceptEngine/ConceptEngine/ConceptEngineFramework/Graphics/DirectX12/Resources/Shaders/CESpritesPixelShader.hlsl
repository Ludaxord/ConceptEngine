// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightUtil.hlsl"
#include "VertexHeader.h"

Texture2DArray gTreeMapArray : register(t0);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
float4x4 gWorld;
float4x4 gTexTransform;
};

// Constant data that varies per pass.
cbuffer cbPass : register(b1)
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

// Allow application to change fog parameters once per frame.
// For example, we may only use fog for certain times of day.
float4 gFogColor;
float gFogStart;
float gFogRange;
float2 cbPerObjectPad2;

// Indices [0, NUM_DIR_LIGHTS) are directional lights;
// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
// are spot lights for a maximum of MaxLights per object.
Light gLights[MaxLights];
};

cbuffer cbMaterial : register(b2)
{
float4 gDiffuseAlbedo;
float3 gFresnelR0;
float gRoughness;
float4x4 gMatTransform;
};

float4 PS(SpriteGeoOut pIn) : SV_Target {

	float3 uvw = float3(pIn.TexCoord, pIn.PrimID % 3);
	float4 diffuseAlbedo = gTreeMapArray.Sample(gsamAnisotropicWrap, uvw) * gDiffuseAlbedo;

#ifdef ALPHA
	clip(diffuseAlbedo.a - 0.1f);
#endif

	pIn.NormalW = normalize(pIn.NormalW);

	//Vector from point being lit to eye
	float3 toEyeW = gEyePosW - pIn.PosW;
	float distToEye = length(toEyeW);
	toEyeW /= distToEye; //normalize

	//Light terms
	float4 ambient = gAmbientLight * diffuseAlbedo;

	const float shininess = 1.0f - gRoughness;
	Material mat = {diffuseAlbedo, gFresnelR0, shininess};
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pIn.PosW, pIn.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif

	//Common convention to take alpha from diffuse albedo
	litColor.a = diffuseAlbedo.a;

	return litColor;

}
