// Combines two images.
//***************************************************************************************

#include "VertexHeader.h"

Texture2D gBaseMap : register(t0);
Texture2D gEdgeMap : register(t1);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

static const float2 gTexCoords[6] =
{
	float2(0.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 0.0f),
	float2(1.0f, 1.0f)
};


CompositeVertexOut VS(uint vId : SV_VertexID) {
	CompositeVertexOut vOut;

	vOut.TexCoord = gTexCoords[vId];

	//Map [0,1]^2 to NDC space
	vOut.PosH = float4(2.0f * vOut.TexCoord.x - 1.0f, 1.0f - 2.0f * vOut.TexCoord.y, 0.0f, 1.0f);

	return vOut;
}
