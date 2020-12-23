struct CEVertexPosColor {
	float3 Position : POSITION;
	float3 Color : COLOR;
};

float4 main(float4 pos : POSITION) : SV_POSITION {
	return pos;
}
