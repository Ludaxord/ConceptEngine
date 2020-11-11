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
    
    init(cameraType: CameraTypes){
        super.init(name: "Camera")
        self.cameraType = cameraType
    }
    
    var cameraType: CameraTypes!
    
    private var _viewMatrix = matrix_identity_float4x4
    var viewMatrix: matrix_float4x4 {
        return _viewMatrix
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
    
    override func updateModelMatrix() {
        _viewMatrix = matrix_identity_float4x4
        _viewMatrix.rotate(angle: self.getRotationX(), axis: X_AXIS)
        _viewMatrix.rotate(angle: self.getRotationY(), axis: Y_AXIS)
        _viewMatrix.rotate(angle: self.getRotationZ(), axis: Z_AXIS)
        _viewMatrix.translate(direction: -getPosition())
    }

}
