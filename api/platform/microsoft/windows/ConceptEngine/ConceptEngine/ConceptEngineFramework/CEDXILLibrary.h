#pragma once
#include <cstdint>
#include <d3d12.h>
#include <d3dcommon.h>
#include <fstream>
#include <string>
#include <vector>
#include <dxc/Support/dxcapi.use.h>
#include <wrl/client.h>

#include "CEHelper.h"
#include "CESubObject.h"
static dxc::DxcDllSupport gDxcDllHelper;

namespace Concept::GraphicsEngine::Direct3D {
	class CEDevice;

	class CEDXIlLibrary : public CESubObject {
	public:
		static Microsoft::WRL::ComPtr<ID3DBlob> CreateLibrary(std::wstring fileName, std::wstring target);

	protected:
		friend class CEDevice;
		friend class std::default_delete<CEDXIlLibrary>;

		CEDXIlLibrary(Microsoft::WRL::ComPtr<ID3DBlob> pBlob,
		              const WCHAR* entryPoint[],
		              uint32_t entryPointCount);

		virtual ~CEDXIlLibrary() {
		};

	private:
		D3D12_DXIL_LIBRARY_DESC dxilLibDesc = {};
		Microsoft::WRL::ComPtr<ID3DBlob> pShaderBlob;
		std::vector<D3D12_EXPORT_DESC> exportDesc;
		std::vector<std::wstring> exportName;
	};
}
