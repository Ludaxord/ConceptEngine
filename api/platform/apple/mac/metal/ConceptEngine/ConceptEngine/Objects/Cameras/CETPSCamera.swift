//
//  CETPSCamera.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 16/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import Foundation
import simd

public class CETPSCamera: CECamera {
    var cameraType: CameraTypes = CameraTypes.TPS
    
    var position: float3 = float3(0)
    
    func defaultCameraBehavior(deltaTime: Float) {
        
    }
}
