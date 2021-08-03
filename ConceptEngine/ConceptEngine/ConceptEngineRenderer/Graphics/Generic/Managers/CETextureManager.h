#pragma once
#include "../../../Managers/CEManager.h"
#include "../../../RenderLayer/CommandList.h"
#include "../../../RenderLayer/PipelineState.h"
#include "../../../RenderLayer/RenderingCore.h"


enum CETextureFactoryFlags : uint32 {
	TextureFactoryFlag_None = 0,
	TextureFactoryFlag_GenerateMips = FLAG(1),
};

struct TextureFactoryData {
	CERef<ComputePipelineState> PanoramaPSO;
	CERef<ComputeShader> ComputeShader;
	CommandList CmdList;
};

inline static TextureFactoryData GlobalFactoryData;

class CETextureManager : public CEManager {

public:
	CETextureManager();
	~CETextureManager() override;

	virtual bool Create() = 0;
	virtual void Release() = 0;

	static class Texture2D* LoadFromFile(const std::string& filePath, uint32 flags,
	                                     EFormat format);
	static class Texture2D* LoadFromMemory(const uint8* pixels, uint32 width, uint32 height,
	                                       uint32 createFlags, EFormat format);

	static class TextureCube* CreateTextureCubeFromPanorama(
		class Texture2D* panoramaSource, uint32 cubeMapSize, uint32 flags,
		EFormat format);
};
