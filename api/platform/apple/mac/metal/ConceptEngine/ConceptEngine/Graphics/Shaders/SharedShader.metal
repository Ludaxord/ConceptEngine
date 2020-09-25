//
//  SharedShader.metal
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/09/2020.
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

struct CEMaterial {
    float4 color;
    bool userMaterialColor;
};
