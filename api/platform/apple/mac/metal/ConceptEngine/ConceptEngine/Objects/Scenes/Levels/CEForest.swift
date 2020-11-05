//
//  CEForest.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 03/11/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//
import MetalKit

public final class CEForest: CEScene {
    
    override func buildCameras() {
        buildDebugCamera()
        super.buildCameras()
    }
    
    override func buildLights() {
        buildSunLight()
        super.buildLights()
    }
    
    override func buildScene() {
        buildForrest()
    }
    
    public override func update() {
        mouseObjectManipulation()
        super.update()
    }
    
    private func buildSunLight() {
      let sunColor = float4(0.7, 0.5, 0, 1.0)
      var sunMaterial = CEMaterial()
      sunMaterial.isIlluminated = false
      sunMaterial.color = sunColor
      
      let sun = CELight(meshType: .Sphere, name: "Sun", camera: camera)
      sun.setScale(5)
      sun.setPosition(float3(0, 100, 100))
      sun.setMaterial(sunMaterial)
      lights.append(sun)

      let light = CELight(name: "Light", camera: camera)
      light.setPosition(float3(0, 100, -100))
      light.setLightBrightness(0.5)
      lights.append(light)
    }
             
    private func buildForrest() {
        let terrain = CEGameObj(meshType: .GroundGrass, camera: _cameraManager.currentCamera)
        terrain.setScale(100)
        addNodeChild(terrain)
    }
    
    private func mouseObjectManipulation() {
        for node in nodeChildren {
            if let gameObject = node as? CEGame3DObject {
                if CEMouse.IsMouseButtonPressed(button: .LEFT) {
                    gameObject.rotateX(CEMouse.GetDY() * CEGameTime.DeltaTime)
                    gameObject.rotateY(CEMouse.GetDX() * CEGameTime.DeltaTime)
                }
            }
        }
    }

    private func buildDebugCamera() {
        camera = CEDebugCamera()
        camera.setPosition(0,1,3)
        camera.setRotationX(Float(10).toRadians)
    }
}
