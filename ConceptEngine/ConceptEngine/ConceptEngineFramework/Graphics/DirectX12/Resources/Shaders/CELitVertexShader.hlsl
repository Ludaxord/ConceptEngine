#include "VertexHeader.h"

cbuffer cbPerObject : register(b0) {
float4x4 gWorld;
}

VertexOut VS(VertexIn vIn) {
	VertexOut vOut = (VertexOut)0.0f;

	//Transform to world space
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);

	//Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	// vOut.NormalW = mul(vIn.NormalL, (float3x3)gWorld);

	//transform to homogeneous clip space
	// vOut.PosH = mul(posW, gViewProj);

	return vOut;
}
