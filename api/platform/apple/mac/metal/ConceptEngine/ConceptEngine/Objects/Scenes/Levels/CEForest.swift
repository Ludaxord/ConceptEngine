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
//        mouseObjectManipulation()
        mouseSceneManipulation()
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

        let treeCount: Int = 500
        let radius: Float = 10
        for i in 0..<treeCount {
            let tree = CEGameObj(meshType: selectRandomTreeMeshType(), camera: _cameraManager.currentCamera)
            let pos = float3(cos(Float(i)) * radius + Float.random(in: -2...2),
                             0,
                             sin(Float(i)) * radius + Float.random(in: -5...5))
            tree.setPosition(pos)
            tree.setScale(Float.random(in: 1...2))
            tree.rotateY(Float.random(in: 0...360))
            addNodeChild(tree)
        }

        let flowerCount: Int = 200
        for _ in 0..<flowerCount {
            let flower = CEGameObj(meshType: selectRandomFlowerMeshType(), camera: _cameraManager.currentCamera)
            let pos = float3(Float.random(in: -(radius-1)...(radius + 1)),
                             0,
                             Float.random(in: -(radius-1)...(radius + 1)))
            flower.setPosition(pos)
            flower.rotateY(Float.random(in: 0...360))
            addNodeChild(flower)
        }
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
    
    private func mouseSceneManipulation() {
        //TODO: Fix
        if CEMouse.IsMouseButtonPressed(button: .LEFT) {
            camera.rotateX(CEMouse.GetDY() * camera.getRotationX())
            camera.rotateY(CEMouse.GetDX() * camera.getRotationY())
//            print(camera.getRotationX())
//            print(camera.getRotationY())
        }
    }

    private func buildDebugCamera() {
        camera = CEDebugCamera()
        camera.setPosition(0,1,3)
        camera.setRotationX(Float(10).toRadians)
    }
    
    private func selectRandomTreeMeshType()->MeshTypes {
        let randVal = Int.random(in: 0..<3)
        switch randVal {
        case 0:
            return .TreePineTallADetailed
        case 1:
            return .TreePineDefaultB
        case 2:
            return .TreePineRoundC
        default:
            return .TreePineDefaultB
        }
    }

    private func selectRandomFlowerMeshType()->MeshTypes {
        let randVal = Int.random(in: 0..<3)
        switch randVal {
        case 0:
            return .FlowerPurpleA
        case 1:
            return .FlowerRedA
        case 2:
            return .FlowerYellowA
        default:
            return .FlowerPurpleA
        }
    }
}
