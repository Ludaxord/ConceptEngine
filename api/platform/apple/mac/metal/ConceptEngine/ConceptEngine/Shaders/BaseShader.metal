//
//  BaseShader.metal
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 08/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float3 position [[ attribute(0) ]];
    float4 color [[ attribute(1) ]];
};

struct RasterizerInput {
    float4 position [[ position ]];
    float4 color;
};

struct CEModelDefaults{
    float4x4 modelMatrix;
};

struct CESceneDefaults{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

vertex RasterizerInput basic_vertex_shader(const VertexInput vInput [[ stage_in ]], constant CESceneDefaults &scene [[ buffer(1) ]], constant CEModelDefaults &model [[ buffer(2) ]]) {
    RasterizerInput rasterizer_input;
    
    rasterizer_input.position = scene.projectionMatrix * scene.viewMatrix * model.modelMatrix * float4(vInput.position, 1);
    rasterizer_input.color = vInput.color;
    
    return rasterizer_input;
}

fragment half4 basic_fragment_shader(RasterizerInput rasterizer_input [[ stage_in ]]) {
    float4 color = rasterizer_input.color;
    return half4(color.r, color.g, color.b, color.a);
}
