#pragma once
#include "CEResource.h"
#include "CETexture.h"

#include "../../../Core/Containers/CEStaticArray.h"
#include "../../../Core/Common/CERef.h"

#include "../RenderLayer/CEBuffer.h"

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
		enum class CEType {
			Texture2D = 1,
			Texture2DArray = 2,
			TextureCube = 3,
			TextureCubeArray = 4,
			Texture3D = 5,
			VertexBuffer = 6,
			IndexBuffer = 7,
			StructuredBuffer = 8
		};

		CEShaderResourceViewCreateInfo(CEType type) : Type(type) {

		}

		CEType Type;

		union {
			struct {
				CETexture2D* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
				uint32 NumMips = 0;
				float MinMipsBias = 0.0f;
			} Texture2D;

			struct {
				CETexture2DArray* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
				uint32 NumMips = 0;
				uint32 ArraySlice = 0;
				uint32 NumArraySlices = 0;
				float MinMipBias = 0.0f;
			} Texture2DArray;

			struct {
				CETextureCube* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
				uint32 NumMips = 0;
				float MinMipBias = 0.0f;
			} TextureCube;

			struct {
				CETextureCubeArray* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
				uint32 NumMips = 0;
				uint32 ArraySlice = 0;
				uint32 NumArraySlices = 0;
				float MinMipBias = 0.0f;
			} TextureCubeArray;

			struct {
				CETexture3D* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
				uint32 NumMips = 0;
				uint32 DepthSlice = 0;
				uint32 NumDepthSlice = 0;
				float MinMipBias = 0.0f;
			} Texture3D;

			struct {
				CEVertexBuffer* Buffer = nullptr;
				uint32 FirstVertex = 0;
				uint32 NumVertices = 0;
			} VertexBuffer;

			struct {
				CEIndexBuffer* Buffer = nullptr;
				uint32 FirstIndex = 0;
				uint32 NumIndices = 0;
			} IndexBuffer;

			struct {
				CEStructuredBuffer* Buffer = nullptr;
				uint32 FirstElement = 0;
				uint32 NumElements = 0;
			} StructuredBuffer;
		};
	};

	struct CEUnorderedAccessViewCreateInfo {

		enum class CEType {
			Texture2D = 1,
			Texture2DArray = 2,
			TextureCube = 3,
			TextureCubeArray = 4,
			Texture3D = 5,
			VertexBuffer = 6,
			IndexBuffer = 7,
			StructuredBuffer = 8
		};

		CEUnorderedAccessViewCreateInfo(CEType type) : Type(type) {

		}

		CEType Type;

		union {
			struct {
				CETexture2D* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
			} Texture2D;

			struct {
				CETexture2DArray* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
				uint32 ArraySlice = 0;
				uint32 NumArraySlices = 0;
			} Texture2DArray;

			struct {
				CETextureCube* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
			} TextureCube;

			struct {
				CETextureCubeArray* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
				uint32 ArraySlice = 0;
				uint32 NumArraySlices = 0;
			} TextureCubeArray;

			struct {
				CETexture3D* Texture = nullptr;
				CEFormat Format = CEFormat::Unknown;
				uint32 Mip = 0;
				uint32 DepthSlice = 0;
				uint32 NumDepthSlices = 0;
			} Texture3D;

			struct {
				CEVertexBuffer* Buffer = nullptr;
				uint32 FirstVertex = 0;
				uint32 NumVertices = 0;
			} VertexBuffer;

			struct {
				CEIndexBuffer* Buffer = nullptr;
				uint32 FirstIndex = 0;
				uint32 NumIndices = 0;
			} IndexBuffer;

			struct {
				CEStructuredBuffer* Buffer = nullptr;
				uint32 FirstElement = 0;
				uint32 NumElements = 0;
			} StructuredBuffer;
		};
	};

	struct CERenderTargetViewCreateInfo {

		enum class CEType {
			Texture2D = 1,
			Texture2DArray = 2,
			TextureCube = 3,
			TextureCubeArray = 4,
			Texture3D = 5
		};

		CERenderTargetViewCreateInfo(CEType type) : Type(type) {

		}

		CEType Type;
		CEFormat Format = CEFormat::Unknown;

		union {
			struct {
				CETexture2D* Texture = nullptr;
				uint32 Mip = 0;
			} Texture2D;

			struct {
				CETexture2DArray* Texture = nullptr;
				uint32 Mip = 0;
				uint32 ArraySlice = 0;
				uint32 NumArraySlices = 0;
			} Texture2DArray;

			struct {
				CETextureCube* Texture = nullptr;
				CECubeFace CubeFace = CECubeFace::PosX;
				uint32 Mip = 0;
			} TextureCube;

			struct {
				CETextureCubeArray* Texture = nullptr;
				CECubeFace CubeFace = CECubeFace::PosX;
				uint32 Mip = 0;
				uint32 ArraySlice = 0;
			} TextureCubeArray;

			struct {
				CETexture3D* Texture = nullptr;
				uint32 Mip = 0;
				uint32 DepthSlice = 0;
				uint32 NumDepthSlices = 0;
			} Texture3D;
		};
	};

	struct CEDepthStencilViewCreateInfo {

		enum class CEType {
			Texture2D = 1,
			Texture2DArray = 2,
			TextureCube = 3,
			TextureCubeArray = 4
		};

		CEDepthStencilViewCreateInfo(CEType type) : Type(type) {

		}

		CEType Type;
		CEFormat Format = CEFormat::Unknown;

		union {
			struct {
				CETexture2D* Texture = nullptr;
				uint32 Mip = 0;
			} Texture2D;

			struct {
				CETexture2DArray* Texture = nullptr;
				uint32 Mip = 0;
				uint32 ArraySlice = 0;
				uint32 NumArraySlices = 0;
			} Texture2DArray;

			struct {
				CETextureCube* Texture = nullptr;
				CECubeFace CubeFace = CECubeFace::PosX;
				uint32 Mip = 0;
			} TextureCube;

			struct {
				CETextureCubeArray* Texture = nullptr;
				CECubeFace CubeFace = CECubeFace::PosX;
				uint32 Mip = 0;
				uint32 ArraySlice = 0;
			} TextureCubeArray;
		};
	};
}
