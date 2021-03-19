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
SamplerComparisonState gsamShadow : register(s6);

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

//We expand each point into a quad (4 vertices), so maximum number of vertices we output per geometry shader invocation is 4;
[maxvertexcount(4)]
void GS(point SpriteVertexOut gIn[1], uint primID : SV_PrimitiveID, inout TriangleStream<SpriteGeoOut> triStream) {
	/*
	 * Compute local coordinate system of sprite relative to the world
	 * space such that the billboard is aligned with the y-axis and faces the eye
	 */

	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = gEyePosW - gIn[0].CenterW;
	look.y = 0.0f; //y-axis aligned, so project to xz-plane
	look = normalize(look);
	float3 right = cross(up, look);

	/*
	 * compute triangle strip vertices (quad) in world space
	 */
	float halfWidth = 0.5f * gIn[0].SizeW.x;
	float halfHeight = 0.5f * gIn[0].SizeW.y;

	float4 v[4];
	v[0] = float4(gIn[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
	v[1] = float4(gIn[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
	v[2] = float4(gIn[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
	v[3] = float4(gIn[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);

	//Transform quad vertices to world space and output
	//then as a triangle strip

	float2 texCoord[4] = {
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	SpriteGeoOut gOut;
	[unroll]
	for (int i = 0; i < 4; ++i) {
		gOut.PosH = mul(v[i], gViewProj);
		gOut.PosW = v[i].xyz;
		gOut.NormalW = look;
		gOut.TexCoord = texCoord[i];
		gOut.PrimID = primID;

		triStream.Append(gOut);
	}
}
