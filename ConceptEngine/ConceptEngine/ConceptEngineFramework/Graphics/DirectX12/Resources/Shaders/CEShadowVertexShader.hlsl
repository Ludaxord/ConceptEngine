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
#if defined(MODEL)
	float3 ElementsWeights : WEIGHTS;
	uint4 ElementIndices : BONEINDICES;
#endif
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vIn) {
	VertexOut vout = (VertexOut)0.0f;

	NormalMapMaterialData matData = gMaterialData[gMaterialIndex];

#if defined(MODEL)
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vIn.ElementsWeights.x;
	weights[1] = vIn.ElementsWeights.y;
	weights[2] = vIn.ElementsWeights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i) {
		/*
		 * Assume no nonuniform when transforming normals, so that we do not have to use inverse-transpose
		 */
		float3 tr = mul(float4(vIn.PosL, 1.0f), gModelTransforms[vIn.ElementIndices[i]]).xyz;
		posL += weights[i] * tr;
	}

	vIn.PosL = posL;
#endif

	// Transform to world space.
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);

	// Transform to homogeneous clip space.
	vout.PosH = mul(posW, gViewProj);

	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vIn.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

	return vout;
}
