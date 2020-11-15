//
//  CEFPSCamera.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 16/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import Foundation
import simd

public class CEFPSCamera: CECamera {
    
    public override var projectionMatrix: matrix_float4x4 {
        return matrix_float4x4.perspective(degreesFieldOfView: 45, aspectRatio: CERenderer.AspectRatio, near: 0.1, far: 1000)
    }
    
    init() {
        super.init(cameraType: .FPS)
    }
        
    public override func defaultCameraBehavior() {
    }
}
