#include "CETypes.hlsl"

struct VertexIn {
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC : TEXCOORD;
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

VertexOut VS(VertexIn vIn) {
	VertexOut vOut;

	//Use local vertex position as cubemap lookup vector;
	vOut.PosL = vIn.PosL;

	//Transform to world space
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);

	//Always center sky about camera
	posW.xyz += gEyePosW;

	//Set z = w so that z/w = 1 (i.e. skydome always on far plane)
	vOut.PosH = mul(posW, gViewProj).xyww;

	return vOut;
}
