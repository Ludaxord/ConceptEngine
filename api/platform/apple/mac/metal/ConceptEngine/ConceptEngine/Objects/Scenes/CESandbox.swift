//
//  CESandbox.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 15/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public final class CESandbox: CEScene {
    
    var camera: CECamera!
    
    //Global sandbox objects
//    var quad: CEGame3DQuad!
    
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
        build3dMovingQuadWithTextures()
        super.update()
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
        let quad = CEGame3DQuad(camera: cameraManager.currentCamera)
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
        
        c.scale = float3(0.2)
        
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

    private func buildWithDefaultCamera() {
        addCamera(camera)
    }
    
    private func buildDefault3DSandbox() {
        camera = CEDebugCamera()
        buildWithDefaultCamera()
        camera.position.z = 5
        
//        build3DCubeGrid(xElementCount: 20, yElementCount: 20, zElementCount: 20)
//        build3DCubeCollection()
//        build3DCubeCollection(cubeWidth: 8, cubeHeight: 5, cubeBack: 5, scale: 1.0)
//        build3DMovingQuad()
        build3DQuadWithTextures()
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
        let quad = CEGame3DQuad(camera: cameraManager.currentCamera)
        quad.setScale(1.3)
        quad.setColor(CEColorUtils.hexStringToColor(hex: "#fdf800"))
        addNodeChild(quad)
        let cube = CEGame3DCube(camera: cameraManager.currentCamera)
        cube.setScale(0.3)
        cube.setColor(CEColorUtils.hexStringToColor(hex: "#ffffff"))
        addNodeChild(cube)
    }
    
    private func buildDefault2DSandbox() {
        camera = CEDebugCamera()
        buildWithDefaultCamera()
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
                    let cube = CEGame3DCube(camera: cameraManager.currentCamera)
                    cube.position.y = yPosition
                    cube.position.x = xPosition
                    cube.position.z = zPosition
                    cube.scale = float3(scale)
                    cube.setColor(CEColorUtils.randomColor)
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
                    gameObject = CEGame2DQuad(camera: cameraManager.currentCamera)
                }else {
                    gameObject = CEGame2DTriangle(camera: cameraManager.currentCamera)
                }
                gameObject.position.x = Float(Float(x) + elementsSpace) / Float(elementsCount)
                gameObject.position.y = Float(Float(y) + elementsSpace) / Float(elementsCount)
                gameObject.scale = float3(scale)
                addNodeChild(gameObject)
            }
        }
    }
}
