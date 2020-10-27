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
        buildDefaultCamera()
        super.buildCameras()
    }
    
    override func buildLights() {
        //Lighting
//        buildDefaultLights()
        buildSunLight()
        super.buildLights()
    }
    
    override func buildScene() {
//        2D
//        buildDefault2DSandbox()
        
//        3D
        buildDefault3DSandbox()
    }
    
    public override func update() {
        
//        2D
//        buildDefault2DUpdate()
        
//        3D Cubes
//        buildDefault3DUpdate()
        
//        3D CubeCollection
//        buildDefault3DUpdateCubeCollection()
        
//        3D Moving Cube
//        build3DMovingCubeActions()
        
//        3D Move Quad
//        build3dMovingQuadWithTextures()
        
//        Obj Move CarTruck
        buildObjRotationCarTruck()
        
//        Obj Manipulation CarTruck
        mouseObjectManipulation()
        super.update()
    }
    
    private func buildDefault3DSandbox() {
        //Camera

        buildDefaultCamera()
        
        //3d Objects
//        build3DCubeGrid(xElementCount: 20, yElementCount: 20, zElementCount: 20)
//        build3DCubeCollection()
//        build3DCubeCollection(cubeWidth: 8, cubeHeight: 5, cubeBack: 5, scale: 1.0)
//        build3DMovingQuad()
//        build3DQuadWithTextures()
//        buildObjCarHatch()
//        buildObjCarSport()
        
//        buildObjCarTruck()
//        buildMipMappedQuad()
//        buildMipMappedTruck()
        buildMipMappedBlenderMonkeys()
    }
    
    private func buildSunLight() {
        let sun = CELight(name: "Sun",camera: camera)
        sun.setScale(0.3)
        sun.setPosition(0, 2, 2)
        sun.setMaterialIsIlluminated(false)
        sun.setLightBrightness(0.3)
        sun.setMaterialColor(1,1,1,1)
        sun.setLightColor(1,1,1)
        lights.append(sun)
    }
    
    private func buildMipMappedTruck() {
        let carTruck = CEGameObjCarTruck(camera: _cameraManager.currentCamera)
        carTruck.setMaterialAmbient(0.01)
        carTruck.setRotationX(0.3)
        carTruck.setMaterialShininess(10)
        carTruck.setMaterialSpecular(10)
        carTruck.setTexture(.CarTruck)
        addNodeChild(carTruck)
    }
    
    private func buildMipMappedBlenderMonkeys() {
        let monkeys = CEGameObjMonkeys(camera: _cameraManager.currentCamera)
        monkeys.setMaterialColor(0.4, 0.4, 0.4, 0.1)
        monkeys.setMaterialAmbient(0.01)
        monkeys.setRotationX(0.3)
        monkeys.setMaterialShininess(10)
        monkeys.setMaterialSpecular(10)
//        monkeys.setTexture(.Monkeys)
        addNodeChild(monkeys)
    }
    
    private func buildMipMappedQuad() {
        let quad = CEGame3DQuad(camera: camera)
        quad.setMaterialAmbient(0.01)
        quad.setMaterialShininess(10)
        quad.setMaterialSpecular(5)
//        quad.setMaterialIsIlluminated(false)
        quad.setTexture(.CarTest)
        addNodeChild(quad)
    }
    
    private func buildDefaultLights() {
        let leftSun = CESunLight(camera: camera)
        leftSun.setName("LeftSun")
        leftSun.setPosition(float3(-1, 1, 0))
        leftSun.setMaterialIsIlluminated(false)
        leftSun.setLightBrightness(0.3)
        leftSun.setMaterialColor(float4(1,0,0,1))
        leftSun.setLightColor(float3(1,0,0))
        lights.append(leftSun)
        
        let middleSun = CESunLight(camera: camera)
        middleSun.setName("MiddleSun")
        middleSun.setPosition(float3(0, 1, 0))
        middleSun.setMaterialIsIlluminated(false)
        middleSun.setLightBrightness(0.3)
        middleSun.setMaterialColor(float4(1,1,1,1))
        middleSun.setLightColor(float3(1,1,1))
        lights.append(middleSun)
        
        let rightSun = CESunLight(camera: camera)
        rightSun.setName("RightSun")
        rightSun.setPosition(float3( 1, 1, 0))
        rightSun.setMaterialIsIlluminated(false)
        rightSun.setLightBrightness(0.3)
        rightSun.setMaterialColor(float4(0,0,1,1))
        rightSun.setLightColor(float3(0,0,1))
        lights.append(rightSun)
    }
    
    private func buildObjCarTruck() {
        let carTruck = CEGameObjCarTruck(camera: _cameraManager.currentCamera)
        carTruck.setTexture(.CarTruck)
        carTruck.setMaterialAmbient(0.01)
        carTruck.setRotationX(0.3)
        carTruck.setMaterialShininess(10)
        carTruck.setMaterialSpecular(5)
//        carTruck.setPositionY(-1)
        addNodeChild(carTruck)
    }
    
    private func buildObjCarHatch() {
        camera.position.z = -100
        let carHatch = CEGameObjCarHatch(camera: _cameraManager.currentCamera)
//        carHatch.setColor(float4(1, 0, 0, 1))
        carHatch.setTexture(.CarHatch)
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
        quad.setTexture(.CarTest)
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

    private func buildDefaultCamera() {
        camera = CEDebugCamera()
        camera.setPosition(0,0,4)
        camera.position.z = 5
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
        quad.setMaterialColor(CEColorUtils.hexStringToColor(hex: "#fdf800"))
        addNodeChild(quad)
        let cube = CEGame3DCube(camera: _cameraManager.currentCamera)
        cube.setScale(0.3)
        cube.setMaterialColor(CEColorUtils.hexStringToColor(hex: "#ffffff"))
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
                    cube.setMaterialColor(CEColorUtils.randomColor)
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
