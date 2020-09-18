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
        buildDefaultSandbox()
    }
    
    public override func update(deltaTime: Float) {
        buildDefaultRotation()
        buildDefaultKeyboardClick(deltaTime: deltaTime)
        buildDefaultMouseClick(deltaTime: deltaTime)
        super.update(deltaTime: deltaTime)
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
    
    private func buildDefaultSandbox() {
        camera = CEDebugCamera()
        buildWithDefaultCamera()
        let elementsCount = 5
        for y in -elementsCount..<elementsCount {
            for x in -elementsCount..<elementsCount {
                let gameObject: CEGameObject!
                if (y + x) % 2 == 0 {
                    gameObject = CEGamePlayer(camera: cameraManager.currentCamera)
                }else {
                    gameObject = CEGameNPC(camera: cameraManager.currentCamera)
                }
                gameObject.position.x = Float(Float(x) + 0.5) / Float(elementsCount)
                gameObject.position.y = Float(Float(y) + 0.5) / Float(elementsCount)
                gameObject.scale = float3(0.1)
                addNodeChild(gameObject)
            }
        }
    }
}
