#pragma once
#include <activation.h>
#include <d3d12.h>
#include <memory>
#include <string>

#include "CESubObject.h"

namespace Concept::GraphicsEngine::Direct3D {
	class CEDevice;

	class CEHitGroup: public CESubObject {
	protected:
		friend class CEDevice;
		friend class std::default_delete<CEHitGroup>;

		CEHitGroup(LPCWSTR ahsExport, LPCWSTR chsExport, const std::wstring& name);
	private:
		std::wstring exportName;
		D3D12_HIT_GROUP_DESC desc{};
	};
}
