//
//  CEScene.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 15/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CEScene: CENode {
    
    internal var _cameraManager = CECameraManager()
    
    private var _lightManager = CELightManager()
    private var _sceneDefaults = CESceneDefaults()
    
    override init(name: String? = nil) {
        super.init(name: name)
        buildScene()
    }
    
    func buildScene() {}
    
    func addCamera(_ camera: CECamera, _ currentCamera: Bool = true) {
        _cameraManager.registerCamera(camera: camera)
        if currentCamera {
            _cameraManager.setCamera(camera.cameraType)
        }
    }
    
    func addLight(_ light: CELight) {
        self.addNodeChild(light)
        _lightManager.addLight(light)
    }
    
    func updateCameras() {
        _cameraManager.update()
    }
    
    func updateSceneDefaults() {
        _sceneDefaults.viewMatrix = _cameraManager.currentCamera.cameraMatrix
        _sceneDefaults.projectionMatrix = _cameraManager.currentCamera.projectionMatrix
        _sceneDefaults.gameTime = CEGameTime.TotalGameTime
    }
    
    public override func update() {
        updateSceneDefaults()
        super.update()
    }
    
    public override func render(renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setVertexBytes(&_sceneDefaults, length: CESceneDefaults.stride, index: 1)
        _lightManager.setLightData(renderCommandEncoder)
        super.render(renderCommandEncoder: renderCommandEncoder)
    }
}
