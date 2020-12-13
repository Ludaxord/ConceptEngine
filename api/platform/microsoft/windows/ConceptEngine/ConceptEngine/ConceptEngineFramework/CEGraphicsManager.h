#pragma once
#include <wrl.h>

#include "CEDevice.h"

namespace wrl = Microsoft::WRL;

class CEGraphicsManager {
public:
	virtual ~CEGraphicsManager() = default;
protected:
	virtual CEDevice GetDevice() {
		return CEDevice();
	};
};
