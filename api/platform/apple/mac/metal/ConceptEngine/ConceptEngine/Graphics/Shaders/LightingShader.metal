//
//  LightingShader.metal
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

#ifndef LIGHTING_METAL
#define LIGHTING_METAL

#include <metal_stdlib>
#include "SharedShader.metal"
using namespace metal;

class LightingShader {
public:
    static float3 GetMutedPhongIntensity(constant CEMaterial &material, constant CELightData *lightDatas, int lightCount, float3 worldPosition, float3 unitNormal, float3 unitToCameraVector) {
        float3 ambients = float3(0,0,0);
               float3 diffuses = float3(0,0,0);
               float3 speculars = float3(0,0,0);
               for (int i = 0; i < lightCount; i++) {
                   CELightData lightData = lightDatas[i];
                   
                   float3 unitToLightVector = normalize(lightData.position - worldPosition);
                   float3 unitReflectionVector = normalize(reflect(-unitToLightVector, unitNormal));
                   
                   float3 ambientness = material.ambient * lightData.ambientIntensity;
                   float3 ambientColor = clamp(ambientness * lightData.color * lightData.brightness, 0.0, 0.1);
                   ambients += ambientColor;
                               
                   float3 diffuseness = material.diffuse * lightData.diffuseIntensity;
                   float nDotL = max(dot(unitNormal, unitToLightVector), 0.0);
                   float3 diffuseColor = clamp(diffuseness * nDotL * lightData.color * lightData.brightness, 0.0, 0.1);
                   diffuses += diffuseColor;
                   
                   float3 specularness = material.specular * lightData.specularIntensity;
                   float rDotV = max(dot(unitReflectionVector, unitToCameraVector), 0.0);
                   float specularExp = pow(rDotV, material.shininess);
                   float3 specularColor = clamp(specularness * specularExp * lightData.color* lightData.brightness, 0.0, 1.0);
                   speculars += specularColor;
//                   Test Value
//                   speculars *= speculars * 10;
               }
               float3 phongIntensity = ambients + diffuses + speculars;
        return phongIntensity;
    }
    
    static float3 GetDynamicPhongIntensity(constant CEMaterial &material,
                                    constant CELightData *lightDatas,
                                    int lightCount,
                                    float3 worldPosition,
                                    float3 unitNormal,
                                    float3 unitToCameraVector) {
        float3 totalAmbient = float3(0,0,0);
        float3 totalDiffuse = float3(0,0,0);
        float3 totalSpecular = float3(0,0,0);
        for(int i = 0; i < lightCount; i++){
            CELightData lightData = lightDatas[i]; // m light
            
            float3 unitToLightVector = normalize(lightData.position - worldPosition); // world_pos -----> light_pos
            float3 unitReflectionVector = normalize(reflect(-unitToLightVector, unitNormal)); // R Vector

            // Ambient Lighting
            float3 ambientness = material.ambient * lightData.ambientIntensity;
            float3 ambientColor = clamp(ambientness * lightData.color * lightData.brightness, 0.0, 1.0);
            totalAmbient += ambientColor;
            
            // Diffuse Lighting
            float3 diffuseness = material.diffuse * lightData.diffuseIntensity;
            float nDotL = max(dot(unitNormal, unitToLightVector), 0.0); // N  dot  L
            float3 diffuseColor = clamp(diffuseness * nDotL * lightData.color * lightData.brightness, 0.0, 1.0);
            totalDiffuse += diffuseColor;
            
            // Specular Lighting
            float3 specularness = material.specular * lightData.specularIntensity;
            float rDotV = max(dot(unitReflectionVector, unitToCameraVector) ,0.0);
            float specularExp = pow(rDotV, material.shininess);
            float3 specularColor = clamp(specularness * specularExp * lightData.color * lightData.brightness, 0.0, 1.0);
            totalSpecular += specularColor;
            
        }
        return totalAmbient + totalDiffuse + totalSpecular;
    }
};

#endif



