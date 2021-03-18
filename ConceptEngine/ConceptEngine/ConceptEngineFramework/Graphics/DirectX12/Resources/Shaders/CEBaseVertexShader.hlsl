#include "CETypes.hlsl"
/*
 * Put in space1 so texture array does not overlap with there resources
 * Texture array will occupy registers t0, t1, ... t3 in space0
 */
StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);

/*
 * Array of textures, which is only supported in shader model 5.1+ Unlike Texture2DArray, the textures in this array can be different sizes and formats, making it more flexible than texture arrays
 */
Texture2D gDiffuseMap[4] : register(t1);

struct VertexIn {
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC : TEXCOORD;
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin) {
	VertexOut vout = (VertexOut)0.0f;

	// Fetch the material data.
	MaterialData matData = gMaterialData[gMaterialIndex];

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	// Transform to homogeneous clip space.
	vout.PosH = mul(posW, gViewProj);

	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

	return vout;
}
