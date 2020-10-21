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
    public func setLightColor(_ color: float3) { self.lightData.color = color }
    public func getLightColor() -> float3 { return self.lightData.color }
    
    public func setLightBrightness(_ brightness: Float) { self.lightData.brightness = brightness }
    public func getLightBrightness() -> Float { return self.lightData.brightness }
    
    public func setLightAmbientIntensity(_ intensity: Float) { self.lightData.ambientIntensity = intensity }
    public func getLightAmbientIntensity() -> Float { return self.lightData.ambientIntensity }
}
