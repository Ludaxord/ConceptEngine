#pragma once
struct Viewport {
	float left;
	float top;
	float right;
	float bottom;
};

struct RayGenConstantBuffer {
	Viewport viewport;
	Viewport stencil;
};
