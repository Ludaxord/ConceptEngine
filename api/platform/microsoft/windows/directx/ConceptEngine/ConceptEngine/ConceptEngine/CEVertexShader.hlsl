struct CEVertexOut {
	float4 color : COLOR;
	float4 pos: SV_POSITION;
};

cbuffer CEConstantBuffer {
	matrix transform;
}

CEVertexOut main(float2 pos : POSITION, float4 color : COLOR) {
	CEVertexOut vso;
	vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform);
	vso.color = color;
	return vso;
}
