//
//  CEScene.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 15/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CEScene: CENode {
    
    var sceneDefaults = CESceneDefaults()
    var camera = CEDebugCamera()
    
    override init() {
        super.init()
        buildScene()
    }
    
    func buildScene() {}
    
    func updateSceneDefaults() {
        sceneDefaults.viewMatrix = camera.cameraMatrix
    }
    
    public override func update(deltaTime: Float) {
        camera.update(deltaTime: deltaTime)
        updateSceneDefaults()
        super.update(deltaTime: deltaTime)
    }
    
    public override func render(renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setVertexBytes(&sceneDefaults, length: CESceneDefaults.stride, index: 1)
        super.render(renderCommandEncoder: renderCommandEncoder)
    }
}
