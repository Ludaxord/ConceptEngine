#pragma once
#ifndef RAYTRACINGHLSLCOMPAT_H
#define RAYTRACINGHLSLCOMPAT_H

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

#endif
