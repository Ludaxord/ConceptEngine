//
//  CELight.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 17/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CELight : CEGameObject {
    
    var lightData = CELightData()
    
    init(name: String, camera: CECamera) {
        super.init(meshType: .None, camera: camera)
        self.setName(name)
    }
    
    init(meshType: MeshTypes, name: String, camera: CECamera) {
        super.init(meshType: meshType, camera: camera)
        super.setName(name)
    }
    
    override func update() {
        self.lightData.position = self.getPosition()
        super.update()
    }
}

extension CELight {
    // Light Color
     public func setLightColor(_ color: float3) { self.lightData.color = color }
     public func setLightColor(_ r: Float,_ g: Float,_ b: Float) { setLightColor(float3(r,g,b)) }
     public func getLightColor()->float3 { return self.lightData.color }
     
     // Light Brightness
     public func setLightBrightness(_ brightness: Float) { self.lightData.brightness = brightness }
     public func getLightBrightness()->Float { return self.lightData.brightness }

     // Ambient Intensity
     public func setLightAmbientIntensity(_ intensity: Float) { self.lightData.ambientIntensity = intensity }
     public func getLightAmbientIntensity()->Float { return self.lightData.ambientIntensity }
     
     // Diffuse Intensity
     public func setLightDiffuseIntensity(_ intensity: Float) { self.lightData.diffuseIntensity = intensity }
     public func getLightDiffuseIntensity()->Float { return self.lightData.diffuseIntensity }
     
     // Specular Intensity
     public func setLightSpecularIntensity(_ intensity: Float) { self.lightData.specularIntensity = intensity }
     public func getLightSpecularIntensity()->Float { return self.lightData.specularIntensity }
}
