#ifndef HLSLCOMPAT_H
#define HLSLCOMPAT_H

struct VertexIn {
	float3 PosL : POSITION;
	float4 Color : COLOR;
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

struct LitVertexIn {
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
};

struct LitVertexOut {
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
};

#endif
