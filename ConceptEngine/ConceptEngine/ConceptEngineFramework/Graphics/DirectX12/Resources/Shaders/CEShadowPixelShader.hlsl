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
	float2 TexC : TEXCOORD;
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

// This is only used for alpha cut out geometry, so that shadows 
// show up correctly.  Geometry that does not need to sample a
// texture can use a NULL pixel shader for depth pass.
void PS(VertexOut pin) {
	// Fetch the material data.
	NormalMapMaterialData matData = gMaterialData[gMaterialIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	uint diffuseMapIndex = matData.DiffuseMapIndex;

	// Dynamically look up the texture in the array.
	diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

#ifdef ALPHA
	// Discard pixel if texture alpha < 0.1.  We do this test as soon 
	// as possible in the shader so that we can potentially exit the
	// shader early, thereby skipping the rest of the shader code.
	clip(diffuseAlbedo.a - 0.1f);
#endif
}
