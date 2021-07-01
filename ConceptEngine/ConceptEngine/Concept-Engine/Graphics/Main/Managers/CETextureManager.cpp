#include "CETextureManager.h"

#ifdef min
#undef min
#endif

#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "CEGraphicsManager.h"

#include "../../../Core/Application/CECore.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Containers/CEUniquePtr.h"

using namespace ConceptEngine::Graphics::Main::Managers;
using namespace ConceptEngine::Graphics::Main::RenderLayer;

CETextureManager::CETextureManager(): CEManager() {
}

CETextureManager::~CETextureManager() {
}

void CETextureManager::Release() {
	MainTextureData.PanoramaPSO.Reset();
	MainTextureData.ComputeShader.Reset();
}

CETexture2D* CETextureManager::LoadFromFile(const std::string& filePath, uint32 flags, RenderLayer::CEFormat format) {
	int32 width = 0;
	int32 height = 0;
	int32 channelCount = 0;

	CEUniquePtr<uint8> pixels;
	if (format == CEFormat::R8G8B8A8_Unorm) {
		pixels = CEUniquePtr<uint8>(stbi_load(filePath.c_str(), &width, &height, &channelCount, 4));
	}
	else if (format == CEFormat::R8_Unorm) {
		pixels = CEUniquePtr<uint8>(stbi_load(filePath.c_str(), &width, &height, &channelCount, 1));
	}
	else if (format == CEFormat::R32G32B32A32_Float) {
		pixels = CEUniquePtr<uint8>(
			reinterpret_cast<uint8*>(stbi_loadf(filePath.c_str(), &width, &height, &channelCount, 4)));
	}
	else {
		CE_LOG_ERROR("[CETextureManager]: Format not supported");
		return nullptr;
	}

	if (!pixels) {
		CE_LOG_ERROR("[CETextureManager]: Failed to load image '" + filePath + "'");
		return nullptr;
	}

	CE_LOG_INFO("[CETexutreManager]: Loaded image from file path: '" + filePath + "'");

	return LoadFromMemory(pixels.Get(), width, height, flags, format);
}

CETexture2D* CETextureManager::LoadFromMemory(const uint8* pixels, uint32 width, uint32 height, uint32 createFlags,
                                              CEFormat format) {
	if (format != CEFormat::R8_Unorm && format != CEFormat::R8G8B8A8_Unorm && format != CEFormat::R32G32B32A32_Float) {
		CE_LOG_ERROR("[CETextureManager]: Format not supported");
		return nullptr;
	}

	const bool generateMips = createFlags & TextureFlag_GenerateMips;
	const uint32 NumMips = generateMips ? uint32(std::min(std::log2(width), std::log2(height))) : 1;

	Assert(NumMips != 0);

	const uint32 stride = GetByteStrideFromFormat(format);
	const uint32 rowPitch = width * stride;

	Assert(rowPitch > 0);

	CEResourceData initialData = CEResourceData(pixels, format, width);
	CERef<CETexture2D> texture = CastGraphicsManager()->CreateTexture2D(format,
	                                                                    width,
	                                                                    height,
	                                                                    NumMips,
	                                                                    1,
	                                                                    TextureFlag_SRV,
	                                                                    CEResourceState::PixelShaderResource,
	                                                                    &initialData);

	if (!texture) {
		return nullptr;
	}

	if (generateMips) {
		CECommandList& commandList = MainTextureData.CommandList;
		using namespace std::placeholders;
		commandList.Execute([&commandList, &texture] {
			commandList.TransitionTexture(texture.Get(),
			                              CEResourceState::PixelShaderResource,
			                              CEResourceState::CopyDest);
			commandList.GenerateMips(texture.Get());
			commandList.TransitionTexture(texture.Get(),
			                              CEResourceState::CopyDest,
			                              CEResourceState::PixelShaderResource);
		});
		CommandListExecutor.ExecuteCommandList(commandList);
	}

	return texture.ReleaseOwnership();
}

CETextureCube* CETextureManager::CreateTextureCubeFromPanorama(CETexture2D* panoramaSource,
                                                               uint32 cubeMapSize,
                                                               uint32 flags,
                                                               CEFormat format) {
	Assert(panoramaSource->IsSRV());

	const bool generateNumMips = flags & TextureFlag_GenerateMips;
	const uint16 numMips = (generateNumMips) ? static_cast<uint16>(std::log2(cubeMapSize)) : 1U;

	CERef<CETextureCube> stagingTexture = CastGraphicsManager()->CreateTextureCube(
		format, cubeMapSize, numMips, TextureFlag_UAV, CEResourceState::Common, nullptr);

	if (!stagingTexture) {
		return nullptr;
	}

	stagingTexture->SetName("TextureCube from panorama staging texture");

	CERef<CEUnorderedAccessView> stagingTextureUAV = CastGraphicsManager()->CreateUnorderedAccessViewForTextureCube(
		stagingTexture.Get(), format, 0);

	if (!stagingTextureUAV) {
		return nullptr;
	}

	stagingTexture->SetName("Texture Cube from Panorama staging texture Unordered Access View");

	CERef<CETextureCube> texture = CastGraphicsManager()->CreateTextureCube(
		format, cubeMapSize, numMips, TextureFlag_SRV, CEResourceState::Common, nullptr);
	if (!texture) {
		return nullptr;
	}

	CECommandList& commandList = MainTextureData.CommandList;
	commandList.Execute(
		[&commandList,&panoramaSource, &stagingTexture, &cubeMapSize, &stagingTextureUAV, &texture, &generateNumMips] {
			commandList.TransitionTexture(panoramaSource, CEResourceState::PixelShaderResource,
			                              CEResourceState::NonPixelShaderResource);
			commandList.TransitionTexture(stagingTexture.Get(), CEResourceState::Common,
			                              CEResourceState::UnorderedAccess);

			commandList.SetComputePipelineState(MainTextureData.PanoramaPSO.Get());

			struct ConstantBuffer {
				uint32 CubeMapSize;
			} CB0;

			CB0.CubeMapSize = cubeMapSize;

			commandList.Set32BitShaderConstants(MainTextureData.ComputeShader.Get(), &CB0, 1);
			commandList.SetUnorderedAccessView(MainTextureData.ComputeShader.Get(), stagingTextureUAV.Get(), 0);

			CEShaderResourceView* panoramaSourceView = panoramaSource->GetShaderResourceView();
			commandList.SetShaderResourceView(MainTextureData.ComputeShader.Get(), panoramaSourceView, 0);

			constexpr uint32 localWorkGroupCount = 16;
			const uint32 threadsX = Math::CEMath::DivideByMultiple(cubeMapSize, localWorkGroupCount);
			const uint32 threadsY = Math::CEMath::DivideByMultiple(cubeMapSize, localWorkGroupCount);
			commandList.Dispatch(threadsX, threadsY, 6);

			commandList.TransitionTexture(panoramaSource, CEResourceState::NonPixelShaderResource,
			                              CEResourceState::PixelShaderResource);
			commandList.TransitionTexture(stagingTexture.Get(), CEResourceState::UnorderedAccess,
			                              CEResourceState::CopySource);
			commandList.TransitionTexture(texture.Get(), CEResourceState::Common, CEResourceState::CopyDest);

			commandList.CopyTexture(texture.Get(), stagingTexture.Get());

			if (generateNumMips) {
				commandList.GenerateMips(texture.Get());
			}

			commandList.TransitionTexture(texture.Get(), CEResourceState::CopyDest,
			                              CEResourceState::PixelShaderResource);
		});

	CommandListExecutor.ExecuteCommandList(commandList);

	return texture.ReleaseOwnership();
}
