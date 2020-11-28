struct CEVertexOut {
	float4 color : COLOR;
	float4 pos: SV_POSITION;
};

cbuffer CEConstantBuffer {
	matrix transformation;
}

CEVertexOut main(float3 pos : POSITION, float4 color : COLOR) {
	CEVertexOut vso;
	vso.pos = mul(float4(pos, 1.0f), transformation);
	vso.color = color;
	return vso;
}
