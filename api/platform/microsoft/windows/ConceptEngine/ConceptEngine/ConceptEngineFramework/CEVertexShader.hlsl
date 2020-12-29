struct CEModelViewProjection {
	matrix MVP;
};

ConstantBuffer<CEModelViewProjection> ModelViewProjectionCB : register(b0);

struct CEVertexPosColor {
	float3 Position : POSITION;
	float3 Color : COLOR;
};

struct CEVertexShaderOutput {
	float4 Color : COLOR;
	float4 Position : SV_Position;
};

CEVertexShaderOutput main(CEVertexPosColor IN) {
	CEVertexShaderOutput OUT;

	OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
	OUT.Color = float4(IN.Color, 1.0f);

	return OUT;
}
