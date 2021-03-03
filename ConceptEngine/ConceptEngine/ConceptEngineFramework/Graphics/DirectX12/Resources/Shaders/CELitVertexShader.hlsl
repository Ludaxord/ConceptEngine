#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "LightUtil.hlsl"
#include "VertexHeader.h"

cbuffer cbPerObject : register(b0) {
float4x4 gWorld;
}

cbuffer cbMaterial : register(b1) {
float4 gDiffuseAlbedo;
float3 gFresnelR0;
float gRoughness;
float4x4 gMatTransform;
}

cbuffer cbPass : register(b2) {
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
float gAmbientLight;

Light gLights[MaxLights];
}

LitVertexOut VS(LitVertexIn vIn) {
	LitVertexOut vOut = (LitVertexOut)0.0f;

	//Transform to world space
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);
	vOut.PosW = posW.xyz;

	//Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vOut.NormalW = mul(vIn.NormalL, (float3x3)gWorld);

	//transform to homogeneous clip space
	vOut.PosH = mul(posW, gViewProj);

	return vOut;
}
