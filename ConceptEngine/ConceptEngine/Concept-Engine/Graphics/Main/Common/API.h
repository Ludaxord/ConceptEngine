#pragma once

#include "../CEGraphics.h"

static bool DirectXAPI;
static bool VulkanAPI;
static bool OpenGLAPI;
static bool MetalAPI;
static bool UnknownAPI;

static void SetAPI(ConceptEngine::Graphics::Main::GraphicsAPI api) {
	switch (api) {
	case ConceptEngine::Graphics::Main::GraphicsAPI::DirectX: {
		DirectXAPI = true;
		VulkanAPI = false;
		OpenGLAPI = false;
		MetalAPI = false;
		UnknownAPI = false;
		break;
	}
	case ConceptEngine::Graphics::Main::GraphicsAPI::Vulkan: {
		DirectXAPI = false;
		VulkanAPI = true;
		OpenGLAPI = false;
		MetalAPI = false;
		UnknownAPI = false;
		break;
	}
	case ConceptEngine::Graphics::Main::GraphicsAPI::OpenGL: {
		DirectXAPI = false;
		VulkanAPI = false;
		OpenGLAPI = true;
		MetalAPI = false;
		UnknownAPI = false;
		break;
	}
	case ConceptEngine::Graphics::Main::GraphicsAPI::Metal: {
		DirectXAPI = false;
		VulkanAPI = false;
		OpenGLAPI = false;
		MetalAPI = true;
		UnknownAPI = false;
		break;
	}
	case ConceptEngine::Graphics::Main::GraphicsAPI::Count:
	case ConceptEngine::Graphics::Main::GraphicsAPI::Unknown: {
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
