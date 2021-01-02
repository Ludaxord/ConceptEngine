#include "CEDevice.h"

#include "CED3D12Device.h"
#include "CEMetalDevice.h"
#include "CEOpenGLDevice.h"
#include "CEVulkanDevice.h"

std::shared_ptr<CEDevice> CEDevice::Create(std::shared_ptr<CEAdapter> adapter, CEGraphicsApiType apiType) {
	switch (apiType) {
	case CEGraphicsApiType::direct3d12:
		return std::make_shared<CED3D12Device>(adapter);
	case CEGraphicsApiType::vulkan:
		return std::make_shared<CEVulkanDevice>(adapter);
	case CEGraphicsApiType::opengl:
		return std::make_shared<CEOpenGLDevice>(adapter);
	case CEGraphicsApiType::metal:
		return std::make_shared<CEMetalDevice>(adapter);
	default:
		return std::make_shared<CEOpenGLDevice>(adapter);
	}
}
