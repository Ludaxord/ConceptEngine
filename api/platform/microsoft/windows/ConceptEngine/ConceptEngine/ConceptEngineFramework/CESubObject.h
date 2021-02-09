#pragma once
#include <d3d12.h>

class CESubObject {
public:
	D3D12_STATE_SUBOBJECT operator()() const {
		return subObject;
	}

protected:
	CESubObject() = default;
	D3D12_STATE_SUBOBJECT subObject;
private:
};
