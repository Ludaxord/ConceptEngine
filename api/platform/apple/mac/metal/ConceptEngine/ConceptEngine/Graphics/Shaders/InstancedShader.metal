//
//  InstancedShader.metal
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

#include <metal_stdlib>
#include "SharedShader.metal"
using namespace metal;

vertex RasterizerInput instanced_vertex_shader(
                                           const VertexInput vInput [[ stage_in ]],
                                           constant CESceneDefaults &scene [[ buffer(1) ]],
                                           constant CEModelDefaults *models [[ buffer(2) ]],
                                           uint instanceId [[ instance_id ]]
                                           ) {
    RasterizerInput rasterizer_input;
    
    CEModelDefaults model = models[instanceId];
    float4 worldPosition = model.modelMatrix * float4(vInput.position, 1);
    rasterizer_input.position = scene.projectionMatrix * scene.viewMatrix * worldPosition;
    rasterizer_input.color = vInput.color;
    rasterizer_input.textureCoordinate = vInput.textureCoordinate;
    rasterizer_input.gameTime = scene.gameTime;
    rasterizer_input.worldPosition = worldPosition.xyz;
    rasterizer_input.toCameraVector = scene.cameraPosition - worldPosition.xyz;
    
    rasterizer_input.surfaceNormal = normalize(model.modelMatrix * float4(vInput.normal, 0.0)).xyz;
    rasterizer_input.surfaceTangent = normalize(model.modelMatrix * float4(vInput.tangent, 0.0)).xyz;
    rasterizer_input.surfaceBitangent = normalize(model.modelMatrix * float4(vInput.bitangent, 0.0)).xyz;
    
//    rasterizer_input.position = scene.projectionMatrix * scene.viewMatrix * model.modelMatrix * float4(vInput.position, 1);
//    rasterizer_input.color = vInput.color;
    
    return rasterizer_input;
}
