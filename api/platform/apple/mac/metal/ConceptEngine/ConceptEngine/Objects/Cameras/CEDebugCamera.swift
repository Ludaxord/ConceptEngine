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
    
    public override var projectionMatrix: matrix_float4x4 {
        return matrix_float4x4.perspective(degreesFieldOfView: 45, aspectRatio: CERenderer.AspectRatio, near: 0.1, far: 1000)
    }
    
    init() {
        super.init(cameraType: .Debug)
    }
        
    public override func defaultCameraBehavior() {
        
        let wheel = CEMouse.GetDWheel()
        let wheelChange = CEMouse.GetDWheelChange()
        
        if CEKeyboard.IsKeyPressed(.z) {
            self.position.z -= 50
        }
        
        if CEKeyboard.IsKeyPressed(.x) {
            self.position.z += 50
        }
        
        if CEKeyboard.IsKeyPressed(.leftArrow) {
            self.moveX(-CEGameTime.DeltaTime)
        }
        
        if CEKeyboard.IsKeyPressed(.rightArrow) {
            self.moveX(CEGameTime.DeltaTime)
        }
                
        if CEKeyboard.IsKeyPressed(.downArrow) {
            self.moveY(-CEGameTime.DeltaTime)
        }
                
        if CEKeyboard.IsKeyPressed(.upArrow) {
            self.moveY(CEGameTime.DeltaTime)
        }
        
        if (wheel > wheelChange) {
            self.position.z += 1
        }


        if (wheel < wheelChange) {
            self.position.z -= 1
        }
    }
    
}
