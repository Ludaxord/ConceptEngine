#include "VertexHeader.h"

float4 PS(VertexOut pIn) : SV_Target {
	return pIn.Color;
}
