#include "CETypes.hlsl"

struct NormalMapMaterialData {
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Roughness;
	float4x4 MatTransform;
	uint DiffuseMapIndex;
	uint NormalMapIndex;
	uint MatPad1;
	uint MatPad2;
};

/*
 * Put in space1 so texture array does not overlap with there resources
 * Texture array will occupy registers t0, t1, ... t3 in space0
 */
StructuredBuffer<NormalMapMaterialData> gMaterialData : register(t0, space1);

/*
 * Array of textures, which is only supported in shader model 5.1+ Unlike Texture2DArray, the textures in this array can be different sizes and formats, making it more flexible than texture arrays
 */
Texture2D gTextureMaps[10] : register(t2);

struct VertexIn {
	float3 PosL : POSITION;
	float2 TexC : TEXCOORD;
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vIn) {
	VertexOut vout = (VertexOut)0.0f;

	NormalMapMaterialData matData = gMaterialData[gMaterialIndex];

	// Transform to world space.
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);

	// Transform to homogeneous clip space.
	vout.PosH = mul(posW, gViewProj);

	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vIn.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

	return vout;
}
