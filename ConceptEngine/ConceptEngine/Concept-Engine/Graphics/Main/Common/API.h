#pragma once

#include "../CEGraphics.h"

enum class GraphicsAPI {
	Unknown = 0,
	DirectX = 1,
	Vulkan = 2,
	OpenGL = 3,
	Metal = 4,
	Count
};

static bool DirectXAPI;
static bool VulkanAPI;
static bool OpenGLAPI;
static bool MetalAPI;
static bool UnknownAPI;

static void SetAPI(GraphicsAPI api) {
	switch (api) {
	case GraphicsAPI::DirectX: {
		DirectXAPI = true;
		VulkanAPI = false;
		OpenGLAPI = false;
		MetalAPI = false;
		UnknownAPI = false;
		break;
	}
	case GraphicsAPI::Vulkan: {
		DirectXAPI = false;
		VulkanAPI = true;
		OpenGLAPI = false;
		MetalAPI = false;
		UnknownAPI = false;
		break;
	}
	case GraphicsAPI::OpenGL: {
		DirectXAPI = false;
		VulkanAPI = false;
		OpenGLAPI = true;
		MetalAPI = false;
		UnknownAPI = false;
		break;
	}
	case GraphicsAPI::Metal: {
		DirectXAPI = false;
		VulkanAPI = false;
		OpenGLAPI = false;
		MetalAPI = true;
		UnknownAPI = false;
		break;
	}
	case GraphicsAPI::Count:
	case GraphicsAPI::Unknown: {
		DirectXAPI = false;
		VulkanAPI = false;
		OpenGLAPI = false;
		MetalAPI = false;
		UnknownAPI = true;
		break;
	}
	default: {
		DirectXAPI = false;
		VulkanAPI = false;
		OpenGLAPI = false;
		MetalAPI = false;
		UnknownAPI = false;
		break;
	}
	}
};

#define DIRECTX_API DirectXAPI;
#define VULKAN_API VulkanAPI;
#define OPENGL_API OpenGLAPI;
#define METAL_API MetalAPI;
#define UNKNOWN_API UnknownAPI;
