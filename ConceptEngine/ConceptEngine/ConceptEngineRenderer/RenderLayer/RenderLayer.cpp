#include "RenderLayer.h"
#include "CommandList.h"

#include "../D3D12/D3D12RenderLayer.h"
#include "../D3D12/CEShaderCompiler.h"

bool RenderLayer::Init(ERenderLayerApi InRenderApi)
{
    // Select RenderLayer
    if (InRenderApi == ERenderLayerApi::D3D12)
    {
        gRenderLayer = DBG_NEW D3D12RenderLayer();
        
        CEShaderCompiler* Compiler = DBG_NEW CEShaderCompiler();
        if (!Compiler->Init())
        {
            return false;
        }

        gShaderCompiler = Compiler;
    }
    else
    {
        CE_LOG_ERROR("[RenderLayer::Init] Invalid RenderLayer enum");
        
        CEDebug::DebugBreak();
        return false;
    }

    // TODO: This should be in EngineConfig
    const bool EnableDebug =
#if ENABLE_API_DEBUGGING
        true;
#else
        false;
#endif

    // Init
    if (gRenderLayer->Init(EnableDebug))
    {
        ICommandContext* CmdContext = gRenderLayer->GetDefaultCommandContext();
        GCmdListExecutor.SetContext(CmdContext);

        return true;
    }
    else
    {
        return false;
    }
}

void RenderLayer::Release()
{
    SafeDelete(gRenderLayer);
    SafeDelete(gShaderCompiler);
}
