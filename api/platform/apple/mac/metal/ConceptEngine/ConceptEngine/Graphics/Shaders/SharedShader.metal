//
//  SharedShader.metal
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

#ifndef SHARED_METAL
#define SHARED_METAL

#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float3 position [[ attribute(0) ]];
    float4 color [[ attribute(1) ]];
    float2 textureCoordinate [[ attribute(2) ]];
    float3 normal [[ attribute(3) ]];
};

struct RasterizerInput {
    float4 position [[ position ]];
    float4 color;
    float2 textureCoordinate;
    float gameTime;
    float3 worldPosition;
    float3 surfaceNormal;
    float3 toCameraVector;
};

struct CEModelDefaults{
    float4x4 modelMatrix;
};

struct CESceneDefaults{
    float gameTime;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float3 cameraPosition;
};

struct CEMaterial {
    float4 color;
    bool useMaterialColor;
    bool useTexture;
    bool useDefaultTrigonometricTexture;
    bool isIlluminated;
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float shininess;
};

struct CELightData {
    float3 position;
    float3 color;
    float brightness;
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
};

struct CETrigonometricTextures {
    float x;
    float y;
    float z;
    float4 color;
    
    CETrigonometricTextures(float2 texCoord, float gameTime) {
        x = sin((texCoord.x + gameTime) * 20);
        y =  sin((texCoord.y - gameTime) * 20);
        z = tan((texCoord.x + gameTime) * 20);
        color = float4(x, y, z, 1);
    }
};

struct CELightCollection {
    int lightCount;
//    CELightData* lightDatas;
};

#endif
