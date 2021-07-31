#pragma once
#include "../../RenderLayer/Resources.h"

#include "../../Core/Containers/StaticArray.h"

struct MaterialProperties
{
    XMFLOAT3 Albedo    = XMFLOAT3(1.0f, 1.0f, 1.0f);
    float Roughness    = 0.0f;
    float Metallic     = 0.0f;
    float AO           = 0.5f;
    int32 EnableHeight = 0;
};

class Material
{
public:
    Material(const MaterialProperties& InProperties);
    ~Material() = default;

    void Init();

    void BuildBuffer(class CommandList& CmdList);

    FORCEINLINE bool IsBufferDirty() const
    {
        return MaterialBufferIsDirty;
    }

    void SetAlbedo(const XMFLOAT3& Albedo);
    void SetAlbedo(float R, float G, float B);

    void SetMetallic(float Metallic);
    void SetRoughness(float Roughness);
    void SetAmbientOcclusion(float AO);

    void EnableHeightMap(bool EnableHeightMap);

    void SetDebugName(const std::string& InDebugName);

    // ShaderResourceView are sorted in the way that the deferred rendering pass wants them
    // This means that one can call BindShaderResourceViews directly with this function
    ShaderResourceView* const* GetShaderResourceViews() const;

    SamplerState* GetMaterialSampler() const { return Sampler.Get(); }
    ConstantBuffer* GetMaterialBuffer() const { return MaterialBuffer.Get(); }

    bool HasAlphaMask() const { return AlphaMask; }

    bool HasHeightMap() const { return HeightMap; }

    const MaterialProperties& GetMaterialProperties() const { return Properties; }

public:
    CERef<Texture2D> AlbedoMap;
    CERef<Texture2D> NormalMap;
    CERef<Texture2D> RoughnessMap;
    CERef<Texture2D> HeightMap;
    CERef<Texture2D> AOMap;
    CERef<Texture2D> MetallicMap;
    CERef<Texture2D> AlphaMask;

private:
    std::string	DebugName;
    bool MaterialBufferIsDirty = true;
    
    MaterialProperties         Properties;
    CERef<ConstantBuffer> MaterialBuffer;
    CERef<SamplerState>   Sampler;

    mutable CEStaticArray<ShaderResourceView*, 7> ShaderResourceViews;
};