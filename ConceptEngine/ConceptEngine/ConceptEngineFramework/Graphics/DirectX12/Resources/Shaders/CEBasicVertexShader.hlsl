#include "VertexHeader.h"

cbuffer cbPerObject : register(b0) {
float4x4 gWorld;
}

cbuffer cbPass : register(b1) {
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
}

VertexOut VS(VertexIn vIn) {
	VertexOut vOut;

	//Transform to homogeneous clip space
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);
	vOut.PosH = mul(posW, gViewProj);

	//pass vertex color into pixel shader
	vOut.Color = vIn.Color;

	return vOut;
}
