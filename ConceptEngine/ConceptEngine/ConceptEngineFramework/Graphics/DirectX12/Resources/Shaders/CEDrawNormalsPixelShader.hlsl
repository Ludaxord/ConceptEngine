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
#if defined(SHADOW)
Texture2D gTextureMaps[10] : register(t2);
#elif defined(SSAO)
Texture2D gTextureMaps[10] : register(t3);
#else
Texture2D gTextureMaps[10] : register(t2);
#endif

struct VertexIn {
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC : TEXCOORD;
	float3 TangentU : TANGENT;
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TexC : TEXCOORD;
};

float4 PS(VertexOut pin) : SV_Target {

	//Fetch material data
	NormalMapMaterialData matData = gMaterialData[gMaterialIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	uint diffuseMapIndex = matData.DiffuseMapIndex;
	uint normalMapIndex = matData.NormalMapIndex;

	diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

#if defined(ALPHA)
/*
 * Discard pixel if texture alpha < 0.1. We do this test as soon as possible in shader so that we can potentially exit shader early, thereby skipping rest of shader code
 */
	clip(diffuseAlbedo.a - 0.1f);
#endif

	//Interpolating normal can unnormalize it, so renormalize it.
	pin.NormalW = normalize(pin.NormalW);

	//NOTE: We use interpolated vertex normal for SSAO

	//Write normal in view space coordinates
	float3 normalV = mul(pin.NormalW, (float3x3)gView);

	return float4(normalV, 0.0f);
}
