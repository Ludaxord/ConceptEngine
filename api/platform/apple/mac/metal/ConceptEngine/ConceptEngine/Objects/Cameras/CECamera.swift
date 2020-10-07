//
//  CECamera.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 16/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import simd

public enum CameraTypes {
    case Debug
    case FPS
    case TPS
}

public class CECamera: CENode {
    var cameraType: CameraTypes!
    
    var viewMatrix: matrix_float4x4 {
        var viewMatrix = matrix_identity_float4x4
        viewMatrix.translate(direction: -getPosition())
        return viewMatrix
    }
    
    var projectionMatrix: matrix_float4x4 {
        return matrix_identity_float4x4
    }
    
    public func defaultCameraBehavior() {
        preconditionFailure("This method must be overridden")
    }
    
    public override func update() {
        defaultCameraBehavior()
    }

    
    init(cameraType: CameraTypes){
        super.init(name: "Camera")
        self.cameraType = cameraType
    }
}


extension CECamera {
    
    var cameraMatrix: matrix_float4x4 {
        var cameraMatrix = matrix_identity_float4x4
        cameraMatrix.translate(direction: -position)
        return cameraMatrix
    }
}
