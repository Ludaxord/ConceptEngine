#include "CETypes.hlsl"

struct VertexIn {
	float3 PosL : POSITION;
	float2 TexC : TEXCOORD;
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

float4 PS(VertexOut pin) : SV_Target{
#ifdef SHADOW
	return float4(gShadowMap.Sample(gsamLinearWrap, pin.TexC).rrr, 1.0f);
#else
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
#endif
}
