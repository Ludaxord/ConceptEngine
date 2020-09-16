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

protocol CECamera {
    var cameraType: CameraTypes { get }
    var position: float3 { get set }
    func defaultCameraBehavior(deltaTime: Float)
    func update(deltaTime: Float)
}


extension CECamera {
    
    func update(deltaTime: Float) {
        defaultCameraBehavior(deltaTime: deltaTime)
    }
    
    var cameraMatrix: matrix_float4x4 {
        var cameraMatrix = matrix_identity_float4x4
        cameraMatrix.translate(direction: -position)
        return cameraMatrix
    }
}
