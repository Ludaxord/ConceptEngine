struct CEPixelShaderInput {
	float4 Color : COLOR;
};

struct PSInput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

// float4 main(CEPixelShaderInput IN) : SV_Target {
// 	return IN.Color;
// }


float4 main(PSInput input) : SV_Target{
	return input.color;
}
