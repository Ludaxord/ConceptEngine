//
//  BaseShader.metal
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 08/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

#include <metal_stdlib>
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
    rasterizer_input.surfaceNormal = (model.modelMatrix * float4(vInput.normal, 1.0)).xyz;
    
    return rasterizer_input;
}

fragment half4 basic_fragment_shader(
                                     RasterizerInput rasterizer_input [[ stage_in ]],
                                     constant CEMaterial &material [[ buffer(1) ]],
                                     constant int &lightCount [[ buffer(2) ]],
                                     constant CELightData *lightDatas [[ buffer(3) ]],
                                     sampler sampler2d [[ sampler(0) ]],
                                     texture2d<float> texture [[ texture(0) ]]
                                     ) {
    
    float2 texCoord = rasterizer_input.textureCoordinate;
    float4 color;
    if (material.useMaterialColor) {
        color = material.color;
    } else if (material.useTexture) {
        color = texture.sample(sampler2d, texCoord);
    } else if (material.useDefaultTrigonometricTexture) {
        float gameTime = rasterizer_input.gameTime;
        color = CETrigonometricTextures(texCoord, gameTime).color;
        color = texture.sample(sampler2d, texCoord);
    } else {
        color = rasterizer_input.color;
    }
    
    if (material.isIlluminated) {
        float3 unitNormal = normalize(rasterizer_input.surfaceNormal);

        float3 ambients = float3(0,0,0);
        float3 diffuses = float3(0,0,0);
        for (int i = 0; i < lightCount; i++) {
            CELightData lightData = lightDatas[i];
            
            float3 unitToLightVector = normalize(lightData.position - rasterizer_input.worldPosition);
            
            float3 ambientness = material.ambient * lightData.ambientIntensity;
            float3 ambientColor = clamp(ambientness * lightData.color * lightData.brightness, 0.0, 0.1);
            ambients += ambientColor;
                        
            float3 diffuseness = material.diffuse * lightData.diffuseIntensity;
            float nDotL = max(dot(unitNormal, unitToLightVector), 0.0);
            float3 diffuseColor = clamp(diffuseness * nDotL * lightData.color * lightData.brightness, 0.0, 0.1);
            diffuses += diffuseColor;
            
        }
        float3 phongIntensity = ambients + diffuses;
        color *= float4(phongIntensity, 1.0);
    }
    
//    CELightData lightData = lightDatas[0];
    
    return half4(color.r, color.g, color.b, color.a);
}
