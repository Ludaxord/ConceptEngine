struct PixelShaderInput {
	float4 Color : COLOR;
};


float4 main(PixelShaderInput IN) : SV_TARGET {
	/*
	 * Return gamma corrected color
	 */
	return pow(abs(IN.Color), 1.0f / 2.0f);
	// return IN.Color;
}
