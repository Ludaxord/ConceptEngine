struct CEVertexOut {
	float4 color : COLOR;
	float4 pos: SV_POSITION;
};


CEVertexOut main(float2 pos : POSITION, float4 color : COLOR) {
	CEVertexOut vso;
	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
	vso.color = color;
	return vso;
}
