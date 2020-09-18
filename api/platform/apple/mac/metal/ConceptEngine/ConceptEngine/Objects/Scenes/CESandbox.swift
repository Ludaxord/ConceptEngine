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
//        3D
        buildDefault3DUpdate(deltaTime: deltaTime)
        super.update(deltaTime: deltaTime)
    }
    
    private func buildDefault3DUpdate(deltaTime: Float) {
        for node in nodeChildren {
            if let cube = node as? CEGame3DCube {
                cube.rotation.x += deltaTime
                cube.rotation.y += deltaTime
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
        let cube = CEGame3DCube(camera: cameraManager.currentCamera)
        camera.position.z = 5
        addNodeChild(cube)
    }
    
    private func buildDefault2DSandbox() {
        camera = CEDebugCamera()
        buildWithDefaultCamera()
        let elementsCount = 5
        for y in -elementsCount..<elementsCount {
            for x in -elementsCount..<elementsCount {
                let gameObject: CEGameObject!
                if (y + x) % 2 == 0 {
                    gameObject = CEGame2DQuad(camera: cameraManager.currentCamera)
                }else {
                    gameObject = CEGame2DTriangle(camera: cameraManager.currentCamera)
                }
                gameObject.position.x = Float(Float(x) + 0.5) / Float(elementsCount)
                gameObject.position.y = Float(Float(y) + 0.5) / Float(elementsCount)
                gameObject.scale = float3(0.1)
                addNodeChild(gameObject)
            }
        }
    }
}
