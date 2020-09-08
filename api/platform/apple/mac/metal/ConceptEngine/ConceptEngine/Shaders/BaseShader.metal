//
//  BaseShader.metal
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 08/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;


vertex float4 basic_vertex_shader(device float3 *vertices [[ buffer(0) ]], uint vertexID [[ vertex_id ]]) {
    return float4(vertices[vertexID], 1);
}

fragment half4 basic_fragment_shader() {
    return half4(1);
}
