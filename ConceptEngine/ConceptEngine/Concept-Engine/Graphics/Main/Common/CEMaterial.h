#pragma once

#include "../RenderLayer/CEBuffer.h"
#include "../RenderLayer/CESamplerState.h"
#include "../RenderLayer/CEResourceViews.h"
#include "../RenderLayer/CETexture.h"
#include "../Common/API.h"
#include "../../../Core/Common/CETypes.h"
#include "../../../Core/Common/CERef.h"
#include "../../../Core/Containers/CEArray.h"
#include "../../../Core/Containers/CEStaticArray.h"
#include "../../../Graphics/Main/RenderLayer/CECommandList.h"
#include "../../../Graphics/Main/Managers/CEGraphicsManager.h"
#include "../../../Core/Application/CECore.h"
#include "../../../Core/Common/CEManager.h"

namespace ConceptEngine::Graphics::Main::Common {
	struct CEMaterialProperties {
		CEVectorFloat3 Albedo = CEVectorFloat3(1.0f, 1.0f, 1.0f);
		float Roughness = 0.0f;
		float Metallic = 0.0f;
		float AO = 0.5f;
		int32 EnableHeight = 0;
	};

	class CEMaterial {
	public:
		CEMaterial(const CEMaterialProperties& Properties);
		virtual ~CEMaterial() = default;

		void Create();

		void BuildBuffer(RenderLayer::CECommandList& commandList);

		bool IsBufferDirty() const {
			return MaterialBufferIsDirty;
		}

		virtual void SetAlbedo(float R, float G, float B) = 0;

		void SetMetallic(float metallic);
		void SetRoughness(float roughness);
		void SetAmbientOcclusion(float AO);

		void EnableHeightMap(bool enableHeightMap);

		void SetDebugName(const std::string& debugName);

		RenderLayer::CEShaderResourceView* const* GetShaderResourceViews() const;

		RenderLayer::CESamplerState* GetMaterialSampler() const {
			return Sampler.Get();
		}

		RenderLayer::CEConstantBuffer* GetMaterialBuffer() const {
			return MaterialBuffer.Get();
		}

		bool HasAlphaMask() const {
			return AlphaMask;
		}

		bool HasHeightMap() const {
			return HeightMap;
		}

		const CEMaterialProperties& GetMaterialProperties() const {
			return Properties;
		}

	public:
		CERef<RenderLayer::CETexture2D> AlbedoMap;
		CERef<RenderLayer::CETexture2D> NormalMap;
		CERef<RenderLayer::CETexture2D> RoughnessMap;
		CERef<RenderLayer::CETexture2D> HeightMap;
		CERef<RenderLayer::CETexture2D> AOMap;
		CERef<RenderLayer::CETexture2D> MetallicMap;
		CERef<RenderLayer::CETexture2D> AlphaMask;

	protected:
		std::string DebugName;
		bool MaterialBufferIsDirty = true;

		CEMaterialProperties Properties;
		CERef<RenderLayer::CEConstantBuffer> MaterialBuffer;
		CERef<RenderLayer::CESamplerState> Sampler;

		mutable CEStaticArray<RenderLayer::CEShaderResourceView*, 7> ShaderResourceViews;
	};
}
