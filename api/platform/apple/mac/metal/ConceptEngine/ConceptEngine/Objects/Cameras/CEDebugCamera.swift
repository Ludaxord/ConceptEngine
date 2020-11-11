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
    
    override var projectionMatrix: matrix_float4x4 {
        return matrix_float4x4.perspective(degreesFieldOfView: 45.0,
        aspectRatio: CERenderer.AspectRatio,
        near: 0.1,
        far: 1000)
    }
    
    private var _moveSpeed: Float = 4.0
    private var _turnSpeed: Float = 1.0
    
    init() {
        super.init(cameraType: .Debug)
    }
    
    private func cameraMovementWithCameraZoom() {
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
    
    private func cameraMovementWithSceneZoom() {
        if(CEKeyboard.IsKeyPressed(.leftArrow)){
            self.moveX(-CEGameTime.DeltaTime * _moveSpeed)
        }
        
        if(CEKeyboard.IsKeyPressed(.rightArrow)){
            self.moveX(CEGameTime.DeltaTime * _moveSpeed)
        }
        
        if(CEKeyboard.IsKeyPressed(.upArrow)){
            self.moveY(CEGameTime.DeltaTime * _moveSpeed)
        }
        
        if(CEKeyboard.IsKeyPressed(.downArrow)){
            self.moveY(-CEGameTime.DeltaTime * _moveSpeed)
        }
        
        if(CEMouse.IsMouseButtonPressed(button: .LEFT)) {
            self.rotate(CEMouse.GetDY() * CEGameTime.DeltaTime * _turnSpeed,
                        CEMouse.GetDX() * CEGameTime.DeltaTime * _turnSpeed,
                        0)
        }
        
        if(CEMouse.IsMouseButtonPressed(button: .CENTER)) {
            self.moveX(-CEMouse.GetDX() * CEGameTime.DeltaTime * _moveSpeed)
            self.moveY(CEMouse.GetDY() * CEGameTime.DeltaTime * _moveSpeed)
        }
        
        self.moveZ(-CEMouse.GetDWheel() * 0.1)
    }
        
    public override func defaultCameraBehavior() {
//        cameraMovementWithCameraZoom()
        cameraMovementWithSceneZoom()
        
    }
    
}
