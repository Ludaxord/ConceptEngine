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
    
    override func buildScene() {
//        2D
//        buildDefault2DSandbox()
        
//        3D
        buildDefault3DSandbox()
    }
    
    public override func update(deltaTime: Float) {
//        2D
//        buildDefault2DUpdate(deltaTime: deltaTime)
//        3D Cubes
//        buildDefault3DUpdate(deltaTime: deltaTime)
//        3D CubeCollection
        buildDefault3DUpdateCubeCollection(deltaTime: deltaTime)
        super.update(deltaTime: deltaTime)
    }
    
    private func buildDefault3DUpdate(deltaTime: Float) {
        for node in nodeChildren {
            if let cube = node as? CEGame3DCube {
                cube.rotation.x += (Float.randomFromZeroToOne * deltaTime)
                cube.rotation.y += (Float.randomFromZeroToOne * deltaTime)
            }
        }
    }
    
    private func buildDefault3DUpdateCubeCollection(deltaTime: Float) {
        for node in nodeChildren {
            if let cubes = node as? CECubeCollection {
                cubes.rotation.z += deltaTime
            }
        }
    }
    
    private func buildDefault2DUpdate(deltaTime: Float) {
        buildDefaultRotation()
        buildDefaultKeyboardClick(deltaTime: deltaTime)
        buildDefaultMouseClick(deltaTime: deltaTime)
    }
    
    private func buildDefaultRotation() {
        for child in nodeChildren {
            child.rotation.z += 0.02
        }
    }
    
    private func buildDefaultMouseClick(deltaTime: Float) {
        let c = nodeChildren[0]
        if (CEMouse.IsMouseButtonPressed(button: .LEFT)) {
            c.scale = float3(c.scale * 2)
        }
    }
    
    private func buildDefaultKeyboardClick(deltaTime: Float) {
        let c = nodeChildren[0]
        
        c.scale = float3(0.2)
        
        if (CEKeyboard.IsKeyPressed(.d)) {
            c.position.x += deltaTime
        }
        
        
        if (CEKeyboard.IsKeyPressed(.a)) {
            c.position.x -= deltaTime
        }
        
        if (CEKeyboard.IsKeyPressed(.w)) {
            c.position.y += deltaTime
        }
        
        if (CEKeyboard.IsKeyPressed(.s)) {
            c.position.y -= deltaTime
        }
    }

    private func buildWithDefaultCamera() {
        addCamera(camera)
    }
    
    private func buildDefault3DSandbox() {
        camera = CEDebugCamera()
        buildWithDefaultCamera()
        camera.position.z = 100
//        build3DCubeGrid(xElementCount: 20, yElementCount: 20, zElementCount: 20)
        build3DCubeCollection()
    }
    
    private func buildDefault2DSandbox() {
        camera = CEDebugCamera()
        buildWithDefaultCamera()
        build2DTriangeQuadGrid(elementsCount: 5)
    }
    
    private func build3DCubeCollection(cubeWidth: Int = 20, cubeHeight: Int = 20, cubeBack: Int = 20) {
        let cubeCollection = CECubeCollection(cubeWidth: cubeWidth, cubeHeight: cubeHeight, cubeBack: cubeBack)
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
