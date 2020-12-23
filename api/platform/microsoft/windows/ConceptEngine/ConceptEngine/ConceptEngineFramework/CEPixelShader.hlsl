struct CEPixelShaderInput {
	float4 Color : COLOR;
};

float4 main(CEPixelShaderInput IN) : SV_TARGET {
	return IN.Color;
}
