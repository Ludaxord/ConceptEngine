//
//  CECameraManager.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 18/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CECameraManager: CEManager {
    
    private var _cameras: [CameraTypes: CECamera] = [:]
    
    public var currentCamera: CECamera!
    
    public func registerCamera(camera: CECamera) {
        self._cameras.updateValue(camera, forKey: camera.cameraType)
    }
    
    public func setCamera(_ cameraType: CameraTypes) {
        self.currentCamera = _cameras[cameraType]
    }
    
    internal func update(deltaTime: Float) {
        for camera in _cameras.values {
            camera.update(deltaTime: deltaTime)
        }
    }
    
}
