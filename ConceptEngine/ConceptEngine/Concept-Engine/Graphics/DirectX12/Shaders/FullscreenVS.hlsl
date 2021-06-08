struct VSOutput {
	float2 TexCoord : TEXCOORD0;
	float4 Position :SV_POSITION;
};

VSOutput Main(uint VertexID : SV_VertexID) {
	VSOutput vsOut;
	vsOut.TexCoord = float2((VertexID << 1) & 2, VertexID & 2);
	vsOut.Position = float4((vsOut.TexCoord * 2.0f) - 1.0f, 0.0f, 1.0f);
	vsOut.Position.y = -vsOut.Position.y;
	return vsOut;
}
