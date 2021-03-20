#include "CETypes.hlsl"

struct VertexIn {
	float3 PosL : POSITION;
	float2 TexC : TEXCOORD;
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vIn) {
	VertexOut vOut = (VertexOut)0.0f;

	//Already in homogeneous clip space
	vOut.PosH = float4(vIn.PosL, 1.0f);

	vOut.TexC = vIn.TexC;

	return vOut;
}
