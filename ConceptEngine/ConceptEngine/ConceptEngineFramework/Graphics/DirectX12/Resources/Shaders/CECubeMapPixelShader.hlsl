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

float4 PS(VertexOut pIn) : SV_Target {
	return gCubeMap.Sample(gsamLinearWrap, pIn.PosL);
}
