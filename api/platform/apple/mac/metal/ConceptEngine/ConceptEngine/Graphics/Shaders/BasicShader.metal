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
    
    rasterizer_input.position = scene.projectionMatrix * scene.viewMatrix * model.modelMatrix * float4(vInput.position, 1);
    rasterizer_input.color = vInput.color;
    
    return rasterizer_input;
}

fragment half4 basic_fragment_shader(
                                     RasterizerInput rasterizer_input [[ stage_in ]],
                                     constant CEMaterial &material [[ buffer(1) ]]
                                     ) {
    float4 color = material.userMaterialColor ? material.color : rasterizer_input.color;
    return half4(color.r, color.g, color.b, color.a);
}
