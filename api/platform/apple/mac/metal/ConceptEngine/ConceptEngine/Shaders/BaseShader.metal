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
    float3 position;
    float4 color;
};

struct RasterizerInput {
    float4 position [[ position ]];
    float4 color;
};

vertex RasterizerInput basic_vertex_shader(device VertexInput *vertices [[ buffer(0) ]], uint vertexID [[ vertex_id ]]) {
    RasterizerInput rasterizer_input;
    rasterizer_input.position = float4(vertices[vertexID].position, 1);
    rasterizer_input.color = vertices[vertexID].color;
    return rasterizer_input;
}

fragment half4 basic_fragment_shader(RasterizerInput rasterizer_input [[ stage_in ]]) {
    float4 color = rasterizer_input.color;
    return half4(color.r, color.g, color.b, color.a);
}
