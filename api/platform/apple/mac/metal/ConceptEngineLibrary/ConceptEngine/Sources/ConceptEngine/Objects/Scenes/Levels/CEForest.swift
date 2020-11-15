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
        buildForest()
        buildTruckInForest()
    }
    
    public override func update() {
//        mouseObjectManipulation()
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
    
    private func buildTruckInForest() {
        let carTruck = CEGameObjCarTruck(camera: _cameraManager.currentCamera)
        carTruck.moveY(0.15)
        carTruck.setBaseColorTexture(.CarTruck)
        addNodeChild(carTruck)
    }
             
    private func buildForest() {
        let terrain = CEGameObj(meshType: .GroundGrass, camera: _cameraManager.currentCamera)
        terrain.setScale(200)
        addNodeChild(terrain)

        // Trees
        let trees = CEGameObjTrees(treeACount: 4000, treeBCount: 4000, treeCCount: 4000)
        addNodeChild(trees)
        
        // Flowers
        let flowers = CEGameObjFlowers(flowerRedCount: 1500, flowerPurpleCount: 1500, flowerYellowCount: 1500)
        addNodeChild(flowers)
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
