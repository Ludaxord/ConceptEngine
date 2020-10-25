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
    
    internal var lights: [CELight] = []
    //change to Array of cameras for more flexibility
    internal var camera: CECamera!
    
    override init(name: String? = nil) {
        super.init(name: name)
        buildCameras()
        buildLights()
        buildScene()
    }
    
    func buildCameras() {
        addCamera(camera)
    }
    
    func buildLights() {
        addLights(lights)
    }
    
    func buildScene() {
    }
    
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
    
    func addLights(_ lights: [CELight]) {
        self.addNodeChildren(lights)
        _lightManager.addLights(lights)
    }
    
    func updateCameras() {
        _cameraManager.update()
    }
    
    func updateSceneDefaults() {
        _sceneDefaults.viewMatrix = _cameraManager.currentCamera.cameraMatrix
        _sceneDefaults.projectionMatrix = _cameraManager.currentCamera.projectionMatrix
        _sceneDefaults.gameTime = CEGameTime.TotalGameTime
        _sceneDefaults.cameraPosition = _cameraManager.currentCamera.getPosition()
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
