#pragma once
#include <cassert>

#include "CEDXDevice.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::RenderLayer {
	class CEDXDeviceElement {
	public:
		CEDXDeviceElement(CEDXDevice* device) : Device(device) {
			assert(Device != nullptr);
		}

		~CEDXDeviceElement() {
			Device = nullptr;
		}

		CEDXDevice* GetDevice() const {
			return Device;
		}

	private:
		CEDXDevice* Device;
	};
}
