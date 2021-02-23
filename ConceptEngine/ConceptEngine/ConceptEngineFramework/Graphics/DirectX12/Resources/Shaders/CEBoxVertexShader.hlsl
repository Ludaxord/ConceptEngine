#include "VertexHeader.h"

cbuffer cbPerObject : register(b0) {
float4x4 gWorldViewProj;
}

VertexOut VS(VertexIn vIn) {
	VertexOut vOut;

	//Transform to homogeneous clip space
	vOut.PosH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);

	// Pass vertex color into pixel shader

	vOut.Color = vIn.Color;

	return vOut;
}
