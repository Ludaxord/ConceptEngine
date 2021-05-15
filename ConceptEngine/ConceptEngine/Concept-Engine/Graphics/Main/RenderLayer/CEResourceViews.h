#pragma once
#include "CEResource.h"

#include "../../../Core/Containers/CEStaticArray.h"
#include "../../../Core/Common/CERef.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEShaderResourceView : public CEResource {

	};

	class CEUnorderedAccessView : public CEResource {

	};

	class CEDepthStencilView : public CEResource {

	};

	class CERenderTargetView : public CEResource {

	};

	using CEDepthStencilViewCube = Core::Containers::CEStaticArray<Core::Common::CERef<CEDepthStencilView>, 6>;

	struct CEShaderResourceViewCreateInfo {
	};

	struct CEUnorderedAccessViewCreateInfo {
	};

	struct CERenderTargetViewCreateInfo {
		
	};

	struct CEDepthStencilViewCreateInfo {
		
	};
}
