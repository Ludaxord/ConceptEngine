#pragma once


#include "../../../RenderLayer/CommandList.h"
#include "../../../RenderLayer/PipelineState.h"

struct TextureFactoryData {
	CERef<ComputePipelineState> PanoramaPSO;
	CERef<ComputeShader> ComputeShader;
	CommandList CmdList;
};

inline TextureFactoryData GlobalFactoryData;
