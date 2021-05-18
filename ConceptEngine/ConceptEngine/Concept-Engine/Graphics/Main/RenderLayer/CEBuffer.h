#pragma once
#include "CEResource.h"
#include "../../../Core/Common/CETypes.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	enum CEBufferFlags : uint32 {
		BufferFlag_None = 0,
		BufferFlag_Default = FLAG(1),
		BufferFlag_Upload= FLAG(1),
		BufferFlag_UAV = FLAG(2),
		BufferFlag_SRV= FLAG(3)
	};

	class CEBuffer : public CEResource {

	};

	class CEVertexBuffer : public CEResource {

	};

	class CEIndexBuffer : public CEBuffer {

	};

	class CEConstantBuffer : public CEBuffer {

	};

	class CEStructuredBuffer : public CEBuffer {

	};
}
