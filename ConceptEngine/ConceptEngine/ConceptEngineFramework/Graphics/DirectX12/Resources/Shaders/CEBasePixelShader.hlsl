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

float4 PS(VertexOut pin) : SV_Target {
	// Fetch the material data.
	MaterialData matData = gMaterialData[gMaterialIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float roughness = matData.Roughness;
	uint diffuseTexIndex = matData.DiffuseMapIndex;

	// Dynamically look up the texture in the array.
	diffuseAlbedo *= gDiffuseMap[diffuseTexIndex].Sample(gsamAnisotropicWrap, pin.TexC);

	// Interpolating normal can unnormalize it, so renormalize it.
	pin.NormalW = normalize(pin.NormalW);

	// Vector from point being lit to eye. 
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// Light terms.
	float4 ambient = gAmbientLight * diffuseAlbedo;

	const float shininess = 1.0f - roughness;
	Material mat = {diffuseAlbedo, fresnelR0, shininess};
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
	                                     pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// Add in specular reflections.
	float3 r = reflect(-toEyeW, pin.NormalW);
	float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
	float3 fresnelFactor = SchlickFresnel(fresnelR0, pin.NormalW, r);
	litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

	// Common convention to take alpha from diffuse albedo.
	litColor.a = diffuseAlbedo.a;

	return litColor;
}
