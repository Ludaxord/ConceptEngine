//
//  CESandbox.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 15/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public final class CESandbox: CEScene {
    
    override func buildCameras() {
        buildDebugCamera()
        super.buildCameras()
    }
    
    override func buildLights() {
        buildSunLight()
        super.buildLights()
    }
    
    override func buildScene() {
        buildDefault3DSandbox()
    }
    
    public override func update() {
        mouseObjectManipulation()
        super.update()
    }
    
    private func buildDefault3DSandbox() {
        buildDefaultCamera()
        buildNormalMapQuad()
    }
    
    private func buildSunLight() {
        let sun = CELight(
            name: "Sun",
            camera: camera
        )
        sun.setPosition(0, 4, 2)
        sun.setLightBrightness(0.7)
        sun.setLightAmbientIntensity(0.04)
        lights.append(sun)
    }
    
    private func buildMipMappedTruck() {
        let carTruck = CEGameObjCarTruck(camera: _cameraManager.currentCamera)
        carTruck.setRotationX(0.3)
        carTruck.setBaseColorTexture(.CarTruck)
        addNodeChild(carTruck)
    }
    
    private func buildCarBugatti() {
        let car = CEGameObjCarBugatti(camera: _cameraManager.currentCamera)
        car.setRotationX(0.3)
        car.setScale(0.3)
        var carMaterial = CEMaterial()
        carMaterial.diffuse = float3(1, 1, 0)
        carMaterial.ambient = float3(1, 1, 1)
        carMaterial.shininess = 100.0
        car.setMaterial(carMaterial)
        addNodeChild(car)
    }
    
    private func buildQuadGrass() {
        let grass = CEGameObjQuadGrass(camera: _cameraManager.currentCamera)
        grass.setRotationX(0.3)
        grass.setScale(0.3)
//        grass.setBaseColorTexture(.QuadGrass)
        var grassMaterial = CEMaterial()
        grassMaterial.diffuse = float3(1, 1, 0)
        grassMaterial.ambient = float3(1, 1, 1)
        grassMaterial.shininess = 100.0
        grass.setMaterial(grassMaterial)
        addNodeChild(grass)
    }
        
        private func buildNormalMapQuad() {
            let carTruck = CEGameObjCarTruck(camera: _cameraManager.currentCamera)
            carTruck.setBaseColorTexture(.CarTruck)
            carTruck.moveX(-1)
            addNodeChild(carTruck)
            
            let quad = CEGameObjQuad(camera: _cameraManager.currentCamera)
            quad.setBaseColorTexture(.CarTest)
            quad.setNormalMapTexture(.PillowNormal)
            quad.rotateX(4.5)
            quad.moveX(1)
            addNodeChild(quad)
        }
    
    private func buildMipMappedBlenderMonkeys() {
        let monkeys = CEGameObjMonkeys(camera: _cameraManager.currentCamera)
        monkeys.setRotationX(0.3)
        
        var monkeysMaterial = CEMaterial()
        monkeysMaterial.diffuse = float3(1, 1, 0)
        monkeysMaterial.ambient = float3(1, 1, 1)
        monkeysMaterial.shininess = 100.0
        monkeys.setMaterial(monkeysMaterial)
        
        addNodeChild(monkeys)
    }
        
    private func buildMipMappedDigimonAgumon() {
        let agumon = CEGameObjDigimonAgumon(camera: _cameraManager.currentCamera)
        agumon.setRotationX(0.3)
        agumon.setBaseColorTexture(.DigimonAgumon)
        addNodeChild(agumon)
    }
    
    private func buildMipMappedQuad() {
        let quad = CEGame3DQuad(camera: camera)
        quad.setBaseColorTexture(.CarTest)
        addNodeChild(quad)
    }
    
    private func buildDefaultLights() {
        let leftSun = CESunLight(camera: camera)
        leftSun.setName("LeftSun")
        leftSun.setPosition(float3(-1, 1, 0))
        leftSun.setLightBrightness(0.3)
        leftSun.setLightColor(float3(1,0,0))
        lights.append(leftSun)
        
        let middleSun = CESunLight(camera: camera)
        middleSun.setName("MiddleSun")
        middleSun.setPosition(float3(0, 1, 0))
        middleSun.setLightBrightness(0.3)
        middleSun.setLightColor(float3(1,1,1))
        lights.append(middleSun)
        
        let rightSun = CESunLight(camera: camera)
        rightSun.setName("RightSun")
        rightSun.setPosition(float3( 1, 1, 0))
        rightSun.setLightBrightness(0.3)
        rightSun.setLightColor(float3(0,0,1))
        lights.append(rightSun)
    }
    
    private func buildObjCarTruck() {
        let carTruck = CEGameObjCarTruck(camera: _cameraManager.currentCamera)
        carTruck.setBaseColorTexture(.CarTruck)
        carTruck.setRotationX(0.3)
        addNodeChild(carTruck)
    }
    
    private func buildObjCarHatch() {
        camera.position.z = -100
        let carHatch = CEGameObjCarHatch(camera: _cameraManager.currentCamera)
//        carHatch.setColor(float4(1, 0, 0, 1))
        carHatch.setBaseColorTexture(.CarHatch)
        addNodeChild(carHatch)
    }
        
    private func buildObjCarSport() {
        camera.position.z = -100
        let carSport = CEGameObjCarSport(camera: _cameraManager.currentCamera)
//        carSport.setColor(float4(0, 1, 0, 1))
        addNodeChild(carSport)
    }
    
    private func mouseObjectManipulation() {
        for node in nodeChildren {
            if let gameObject = node as? CEGame3DObject {
                if CEMouse.IsMouseButtonPressed(button: .LEFT) {
                    gameObject.rotateX(CEMouse.GetDY() * CEGameTime.DeltaTime)
                    gameObject.rotateY(CEMouse.GetDX() * CEGameTime.DeltaTime)
                }
            }
            
            if let light = node as? CELight {
                if light.getName() == "LeftSun" {
                    light.setPositionX(cos(CEGameTime.TotalGameTime) - 1)
                } else if light.getName() == "MiddleSun" {
                    light.setPositionX(cos(CEGameTime.TotalGameTime))
                }else if light.getName() == "RightSun" {
                    light.setPositionX(cos(CEGameTime.TotalGameTime) + 1)
                }
                
            }
        }
    }
    
    private func buildObjRotationCarTruck() {
        for node in nodeChildren {
            if let carTruck = node as? CEGameObjCarTruck {
//                carTruck.rotateX(CEGameTime.DeltaTime)
//                carTruck.rotateY(CEGameTime.DeltaTime)
            }
        }
    }
    
    private func buildObjMoving() {
        for node in nodeChildren {
            if let carTruck = node as? CEGameObj {
                carTruck.rotateY(CEGameTime.DeltaTime)
            }
        }
    }
    
    private func build3dMovingQuadWithTextures() {
        for node in nodeChildren {
            if let quad = node as?
//                CEGame3DCube
                CEGame3DQuad
            {
//                quad.rotateY(sin(CEGameTime.DeltaTime))
//                quad.rotateX(tan(CEGameTime.DeltaTime) * 2)
//                quad.rotateZ(CEGameTime.DeltaTime / 2)
                
//                For Texture
                quad.rotateY(CEGameTime.DeltaTime)
            }
        }
    }
    
    private func build3DQuadWithTextures() {
//        let quad = CEGame3DCube(camera: cameraManager.currentCamera)
        let quad = CEGame3DQuad(camera: _cameraManager.currentCamera)
//        quad.setScale(1.3)
//        quad.setRotationY(4.2)
        quad.setBaseColorTexture(.CarTest)
        addNodeChild(quad)
    }
    
    private func buildDefault3DUpdate() {
        for node in nodeChildren {
            if let cube = node as? CEGame3DCube {
                cube.rotation.x += (Float.randomFromZeroToOne * CEGameTime.DeltaTime)
                cube.rotation.y += (Float.randomFromZeroToOne * CEGameTime.DeltaTime)
            }
        }
    }
    
    private func buildDefault3DUpdateCubeCollection() {
        for node in nodeChildren {
            if let cubes = node as? CECubeCollection {
                cubes.rotation.z += CEGameTime.DeltaTime
            }
        }
    }
    
    private func buildDefault2DUpdate() {
        buildDefaultRotation()
        buildDefaultKeyboardClick()
        buildDefaultMouseClick()
    }
    
    private func buildDefaultRotation() {
        for child in nodeChildren {
            child.rotation.z += 0.02
        }
    }
    
    private func buildDefaultMouseClick() {
        let c = nodeChildren[0]
        if (CEMouse.IsMouseButtonPressed(button: .LEFT)) {
            c.scale = float3(c.scale * 2)
        }
    }
    
    private func buildDefaultKeyboardClick() {
        let c = nodeChildren[0]
        
        c.scale = float3(repeating: 0.2)
        
        if (CEKeyboard.IsKeyPressed(.d)) {
            c.position.x += CEGameTime.DeltaTime
        }
        
        
        if (CEKeyboard.IsKeyPressed(.a)) {
            c.position.x -= CEGameTime.DeltaTime
        }
        
        if (CEKeyboard.IsKeyPressed(.w)) {
            c.position.y += CEGameTime.DeltaTime
        }
        
        if (CEKeyboard.IsKeyPressed(.s)) {
            c.position.y -= CEGameTime.DeltaTime
        }
    }

    private func buildDebugCamera() {
        camera = CEDebugCamera()
        camera.setPosition(0,0,10)
//        camera.position.z = 5
    }

    
    private func build3DMovingCubeActions() {
        for node in nodeChildren {
            node.setPositionX(cos(CEGameTime.TotalGameTime))
            if let cubes = node as? CEGame3DCube {
                cubes.rotateX(cos(CEGameTime.DeltaTime) / 8)
                cubes.rotateY(-(sin(CEGameTime.DeltaTime) / 8))
            }
        }
    }
    
    private func build3DMovingQuad() {
        let quad = CEGame3DQuad(camera: _cameraManager.currentCamera)
        quad.setScale(1.3)
        addNodeChild(quad)
        let cube = CEGame3DCube(camera: _cameraManager.currentCamera)
        cube.setScale(0.3)
        addNodeChild(cube)
    }
    
    private func buildDefault2DSandbox() {
        camera = CEDebugCamera()
        buildDefaultCamera()
        build2DTriangeQuadGrid(elementsCount: 5)
    }
    
    private func build3DCubeCollection(cubeWidth: Int = 20, cubeHeight: Int = 20, cubeBack: Int = 20, scale: Float = 0.3) {
        let cubeCollection = CECubeCollection(cubeWidth: cubeWidth, cubeHeight: cubeHeight, cubeBack: cubeBack, scale: scale)
        addNodeChild(cubeCollection)
    }
    
    private func build3DCubeGrid(xElementCount: Int, yElementCount: Int, zElementCount: Int, elementsSpace: Float = 0.5, scale: Float = 0.3) {
        for y in -yElementCount..<yElementCount {
            let yPosition = Float(y) + elementsSpace
            for x in -xElementCount..<xElementCount {
                let xPosition = Float(x) + elementsSpace
                for z in -zElementCount..<zElementCount {
                    let zPosition = Float(z) + elementsSpace
                    let cube = CEGame3DCube(camera: _cameraManager.currentCamera)
                    cube.position.y = yPosition
                    cube.position.x = xPosition
                    cube.position.z = zPosition
                    cube.scale = float3(repeating: scale)
                    addNodeChild(cube)
                }
            }
        }
    }
    
    private func build2DTriangeQuadGrid(elementsCount: Int, elementsSpace: Float = 0.5, scale: Float = 0.1) {
        for y in -elementsCount..<elementsCount {
            for x in -elementsCount..<elementsCount {
                let gameObject: CEGameObject!
                if (y + x) % 2 == 0 {
                    gameObject = CEGame2DQuad(camera: _cameraManager.currentCamera)
                }else {
                    gameObject = CEGame2DTriangle(camera: _cameraManager.currentCamera)
                }
                gameObject.position.x = Float(Float(x) + elementsSpace) / Float(elementsCount)
                gameObject.position.y = Float(Float(y) + elementsSpace) / Float(elementsCount)
                gameObject.scale = float3(repeating: scale)
                addNodeChild(gameObject)
            }
        }
    }
}
