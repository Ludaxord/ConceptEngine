//
//  CEDebugCamera.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 16/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import Foundation
import simd

public class CEDebugCamera: CECamera {
    
    public var projectionMatrix: matrix_float4x4 {
        return matrix_float4x4.perspective(degreesFieldOfView: 45, aspectRatio: CERenderer.AspectRatio, near: 0.1, far: 1000)
    }
    
    public var cameraType: CameraTypes = CameraTypes.Debug
    
    public var position: float3 = float3(0)
    
    public func defaultCameraBehavior(deltaTime: Float) {
        if CEKeyboard.IsKeyPressed(.leftArrow) {
            self.position.x -= deltaTime
        }
        
        if CEKeyboard.IsKeyPressed(.rightArrow) {
            self.position.x += deltaTime
        }
                
        if CEKeyboard.IsKeyPressed(.downArrow) {
            self.position.y -= deltaTime
        }
                
        if CEKeyboard.IsKeyPressed(.upArrow) {
            self.position.y += deltaTime
        }
    }
}
