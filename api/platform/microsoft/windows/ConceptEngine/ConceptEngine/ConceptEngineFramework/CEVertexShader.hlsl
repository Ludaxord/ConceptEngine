struct CEModelViewProjection {
	matrix MVP;
};

ConstantBuffer<CEModelViewProjection> ModelViewProjectionCB : register(b0);

struct CEVertexPosColor {
	float3 Position : POSITION;
	float3 Color : COLOR;
};

struct CEVertexShaderOutput {
	float4 Position : SV_Position;
	float4 Color : COLOR;
};

struct PSInput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

// CEVertexShaderOutput main(CEVertexPosColor IN) {
// 	CEVertexShaderOutput OUT;
//
// 	OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
// 	OUT.Color = float4(IN.Color, 1.0f);
//
// 	return OUT;
// }

PSInput main(float4 position: POSITION, float4 color: COLOR) {
	PSInput result;
	result.position = position;
	result.color = color;

	return result;
}

