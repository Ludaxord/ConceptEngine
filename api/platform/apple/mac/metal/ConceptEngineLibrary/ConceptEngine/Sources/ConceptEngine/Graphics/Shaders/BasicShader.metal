//
//  BaseShader.metal
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 08/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

#include <metal_stdlib>
#include "LightingShader.metal"
#include "SharedShader.metal"
using namespace metal;


vertex RasterizerInput basic_vertex_shader(
                                           const VertexInput vInput [[ stage_in ]],
                                           constant CESceneDefaults &scene [[ buffer(1) ]],
                                           constant CEModelDefaults &model [[ buffer(2) ]]
                                           ) {
    RasterizerInput rasterizer_input;
    
    float4 worldPosition = model.modelMatrix * float4(vInput.position, 1);

    rasterizer_input.position = scene.projectionMatrix * scene.viewMatrix * worldPosition;
    rasterizer_input.color = vInput.color;
    rasterizer_input.textureCoordinate = vInput.textureCoordinate;
    rasterizer_input.gameTime = scene.gameTime;
    rasterizer_input.worldPosition = worldPosition.xyz;
    rasterizer_input.toCameraVector = scene.cameraPosition - worldPosition.xyz;
    
    rasterizer_input.surfaceNormal = normalize(model.modelMatrix * float4(vInput.normal, 1.0)).xyz;
    rasterizer_input.surfaceTangent = normalize(model.modelMatrix * float4(vInput.tangent, 1.0)).xyz;
    rasterizer_input.surfaceBitangent = normalize(model.modelMatrix * float4(vInput.bitangent, 1.0)).xyz;
    
    return rasterizer_input;
}

fragment half4 basic_fragment_shader(
                                     RasterizerInput rasterizer_input [[ stage_in ]],
                                     constant CEMaterial &material [[ buffer(1) ]],
                                     constant int &lightCount [[ buffer(2) ]],
                                     constant CELightData *lightDatas [[ buffer(3) ]],
                                     sampler sampler2d [[ sampler(0) ]],
                                     texture2d<float> baseColorMap [[ texture(0) ]],
                                     texture2d<float> normalMap [[ texture(1) ]]
                                     ) {
    
    float2 texCoord = rasterizer_input.textureCoordinate;
    float4 color = material.color;

    if (material.useBaseTexture) {
        color = baseColorMap.sample(sampler2d, texCoord);
    }
    
    if (material.isIlluminated) {
        float3 unitNormal = normalize(rasterizer_input.surfaceNormal);
        if (material.useNormalMapTexture) {
            float3 sampleNormal = normalMap.sample(sampler2d, texCoord).rgb * 2 - 1;
            float3x3 tangentBitangentNormalMatrix = { rasterizer_input.surfaceTangent, rasterizer_input.surfaceBitangent, rasterizer_input.surfaceNormal };
            unitNormal = tangentBitangentNormalMatrix * sampleNormal;
        }
        float3 unitToCameraVector = normalize(rasterizer_input.toCameraVector);

        float3 phongIntensity = LightingShader::GetDynamicPhongIntensity(material,
                                                            lightDatas,
                                                            lightCount,
                                                            rasterizer_input.worldPosition,
                                                            unitNormal,
                                                            unitToCameraVector);
        color *= float4(phongIntensity, 1.0);
    }
        
    return half4(color.r, color.g, color.b, color.a);
}
