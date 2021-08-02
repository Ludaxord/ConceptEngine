#include "RenderLayer.h"
#include "CommandList.h"

#include "../D3D12/CEDX12GraphicsManager.h"
#include "../D3D12/CEDX12ShaderCompiler.h"

bool RenderLayer::Init(ERenderLayerApi InRenderApi)
{
    // Select RenderLayer
    if (InRenderApi == ERenderLayerApi::D3D12)
    {
        GraphicsManager = DBG_NEW CEDX12GraphicsManager();
        
        CEDX12ShaderCompiler* Compiler = DBG_NEW CEDX12ShaderCompiler();
        if (!Compiler->Create())
        {
            return false;
        }

        ShaderCompiler = Compiler;
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
    if (GraphicsManager->Create(EnableDebug))
    {
        ICommandContext* CmdContext = GraphicsManager->GetDefaultCommandContext();
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
    SafeDelete(GraphicsManager);
    SafeDelete(ShaderCompiler);
}
