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

struct LitTexVertexIn {
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct LitVertexOut {
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
};

struct LitTexVertexOut {
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct SpriteVertexIn {
	float3 PosW : POSITION;
	float2 SizeW : SIZE;
};

struct SpriteVertexOut {
	float3 CenterW : POSITION;
	float2 SizeW : SIZE;
};

struct SpriteGeoOut {
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD;
	uint PrimID : SV_PrimitiveID;
};

struct CompositeVertexOut {
	float4 PosH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};


struct TessVertexIn {
	float3 PosL : POSITION;
};

struct TessVertexOut {
	float3 PosL : POSITION;
};

struct PatchTess {
	float EdgeTess[4] : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

struct HullOut {
	float3 PosL : POSITION;
};

struct DomainOut {
	float4 PosH : SV_POSITION;
};

struct MaterialData {
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Roughness;
	float4x4 MatTransform;
	uint DiffuseMapIndex;
	uint MatPad0;
	uint MatPad1;
	uint MatPad2;
};

#endif
