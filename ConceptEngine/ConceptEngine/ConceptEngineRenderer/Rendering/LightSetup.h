#pragma once
#include "../RenderLayer/Resources.h"
#include "../RenderLayer/ResourceViews.h"
#include "../RenderLayer/CommandList.h"

#include "../Scene/CEScene.h"

struct PointLightData
{
    XMFLOAT3 Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
    float Padding0;
};

struct ShadowCastingPointLightData
{
    XMFLOAT3 Color         = XMFLOAT3(1.0f, 1.0f, 1.0f);
    float    ShadowBias    = 0.005f;
    float    FarPlane      = 10.0f;
    float    MaxShadowBias = 0.05f;
    float Padding0;
    float Padding1;
};

struct PointLightShadowMapGenerationData
{
    CEStaticArray<XMFLOAT4X4, 6> Matrix;
    CEStaticArray<XMFLOAT4X4, 6> ViewMatrix;
    CEStaticArray<XMFLOAT4X4, 6> ProjMatrix;
    float    FarPlane;
    XMFLOAT3 Position;
};

struct DirectionalLightData
{
    XMFLOAT3   Color         = XMFLOAT3(1.0f, 1.0f, 1.0f);
    float      ShadowBias    = 0.005f;
    XMFLOAT3   Direction     = XMFLOAT3(0.0f, -1.0f, 0.0f);
    float      MaxShadowBias = 0.05f;
    XMFLOAT4X4 LightMatrix;
};

struct DirLightShadowMapGenerationData
{
    XMFLOAT4X4 Matrix;
    float      FarPlane;
    XMFLOAT3   Position;
};

struct LightSetup
{
    const EFormat ShadowMapFormat      = EFormat::D32_Float;
    const EFormat LightProbeFormat     = EFormat::R16G16B16A16_Float;
    const uint32  MaxPointLights       = 256;
    const uint32  MaxDirectionalLights = 256;
    const uint32  MaxPointLightShadows = 8;
    const uint16  ShadowMapWidth       = 4096;
    const uint16  ShadowMapHeight      = 4096;
    const uint16  PointLightShadowSize = 1024;

    LightSetup()  = default;
    ~LightSetup() = default;

    bool Init();

    void BeginFrame(CommandList& CmdList, const CEScene& Scene);
    void Release();

    CEArray<XMFLOAT4>           PointLightsPosRad;
    CEArray<PointLightData>     PointLightsData;
    CERef<ConstantBuffer> PointLightsBuffer;
    CERef<ConstantBuffer> PointLightsPosRadBuffer;

    CEArray<PointLightShadowMapGenerationData> PointLightShadowMapsGenerationData;
    CEArray<XMFLOAT4>                    ShadowCastingPointLightsPosRad;
    CEArray<ShadowCastingPointLightData> ShadowCastingPointLightsData;
    CERef<ConstantBuffer>          ShadowCastingPointLightsBuffer;
    CERef<ConstantBuffer>          ShadowCastingPointLightsPosRadBuffer;
    CERef<TextureCubeArray>        PointLightShadowMaps;
    CEArray<DepthStencilViewCube>        PointLightShadowMapDSVs;

    CEArray<DirLightShadowMapGenerationData> DirLightShadowMapsGenerationData;
    CEArray<DirectionalLightData> DirectionalLightsData;
    CERef<ConstantBuffer>   DirectionalLightsBuffer;
    CERef<Texture2D>        DirLightShadowMaps;

    CERef<TextureCube>         IrradianceMap;
    CERef<UnorderedAccessView> IrradianceMapUAV;

    CERef<TextureCube>                 SpecularIrradianceMap;
    CEArray<CERef<UnorderedAccessView>> SpecularIrradianceMapUAVs;
    CEArray<UnorderedAccessView*>            WeakSpecularIrradianceMapUAVs;
};