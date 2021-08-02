#pragma once
#include "../RenderLayer/CEShaderCompiler.h"

#include "D3D12Helpers.h"
#include "D3D12Shader.h"

#include <string>

#include <d3d12shader.h>

class CEDX12ShaderCompiler : public ICEShaderCompiler
{
public:
    CEDX12ShaderCompiler();
    ~CEDX12ShaderCompiler();

    bool Create();
    
    virtual bool CompileFromFile(
        const std::string& FilePath,
        const std::string& EntryPoint,
        const CEArray<ShaderDefine>* Defines,
        EShaderStage ShaderStage,
        EShaderModel ShaderModel,
        CEArray<uint8>& Code) override final;

    virtual bool CompileShader(
        const std::string& ShaderSource,
        const std::string& EntryPoint,
        const CEArray<ShaderDefine>* Defines,
        EShaderStage ShaderStage,
        EShaderModel ShaderModel,
        CEArray<uint8>& Code) override final;

    bool GetReflection(D3D12BaseShader* Shader, ID3D12ShaderReflection** Reflection);
    bool GetLibraryReflection(D3D12BaseShader* Shader, ID3D12LibraryReflection** Reflection);

    bool HasRootSignature(D3D12BaseShader* Shader);

private:
    bool InternalCompileFromSource(
        IDxcBlob* SourceBlob, 
        LPCWSTR FilePath, 
        LPCWSTR Entrypoint, 
        EShaderStage ShaderStage,
        EShaderModel ShaderModel,
        const CEArray<ShaderDefine>* Defines,
        CEArray<uint8>& Code);

    bool InternalGetReflection(const TComPtr<IDxcBlob>& ShaderBlob, REFIID iid, void** ppvObject);

    bool ValidateRayTracingShader(const TComPtr<IDxcBlob>& ShaderBlob, LPCWSTR Entrypoint);

private:
    TComPtr<IDxcCompiler>       DxCompiler;
    TComPtr<IDxcLibrary>        DxLibrary;
    TComPtr<IDxcLinker>         DxLinker;
    TComPtr<IDxcIncludeHandler> DxIncludeHandler;
    TComPtr<IDxcContainerReflection> DxReflection;
    HMODULE DxCompilerDLL;
};

extern CEDX12ShaderCompiler*  gD3D12ShaderCompiler;
extern DxcCreateInstanceProc DxcCreateInstanceFunc;