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
    var cameraManager = CECameraManager()
    
    override init(name: String? = nil) {
        super.init(name: name)
        buildScene()
    }
    
    func buildScene() {}
    
    func addCamera(_ camera: CECamera, _ currentCamera: Bool = true) {
        cameraManager.registerCamera(camera: camera)
        if currentCamera {
            cameraManager.setCamera(camera.cameraType)
        }
    }
    
    func updateCameras() {
        cameraManager.update()
    }
    
    func updateSceneDefaults() {
        sceneDefaults.viewMatrix = cameraManager.currentCamera.cameraMatrix
        sceneDefaults.projectionMatrix = cameraManager.currentCamera.projectionMatrix
        sceneDefaults.gameTime = CEGameTime.TotalGameTime
    }
    
    public override func update() {
        updateSceneDefaults()
        super.update()
    }
    
    public override func render(renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setVertexBytes(&sceneDefaults, length: CESceneDefaults.stride, index: 1)
        super.render(renderCommandEncoder: renderCommandEncoder)
    }
}
