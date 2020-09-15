//
//  ConceptView.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 08/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit
import Foundation

@objc(CGameView)
class CEGameView: MTKView {
        
    var Renderer: CERenderer!
    var Engine: ConceptEngine!
    
    required init(coder: NSCoder) {
        super.init(coder: coder)
        self.device = MTLCreateSystemDefaultDevice()
        self.Engine = ConceptEngine.Initialize(device: self.device!)
        self.Engine.injectGpuToView(view: self)
        self.Engine.injectDefaultColorsToView(view: self)
        self.Renderer = CERenderer(self)
        self.delegate = self.Renderer
    }
}

extension CEGameView {
    override var acceptsFirstResponder: Bool { return true }
    
    override func keyDown(with event: NSEvent) {
        CEKeyboard.SetKeyPressed(event.keyCode, isOn: true)
    }
    
    override func keyUp(with event: NSEvent) {
        CEKeyboard.SetKeyPressed(event.keyCode, isOn: false)
    }
}

extension CEGameView {
    override func mouseDown(with event: NSEvent) {
        CEMouse.SetMouseButtonPressed(button: event.buttonNumber, isOn: true)
    }
    
    override func mouseUp(with event: NSEvent) {
        CEMouse.SetMouseButtonPressed(button: event.buttonNumber, isOn: false)
    }
    
    override func rightMouseDown(with event: NSEvent) {
        CEMouse.SetMouseButtonPressed(button: event.buttonNumber, isOn: true)
    }
    
    override func otherMouseDown(with event: NSEvent) {
        CEMouse.SetMouseButtonPressed(button: event.buttonNumber, isOn: true)
    }
    
    override func rightMouseUp(with event: NSEvent) {
        CEMouse.SetMouseButtonPressed(button: event.buttonNumber, isOn: false)
    }
    
    override func otherMouseUp(with event: NSEvent) {
        CEMouse.SetMouseButtonPressed(button: event.buttonNumber, isOn: false)
    }
}

extension CEGameView {
    override func mouseMoved(with event: NSEvent) {
         setMousePositionChanged(event: event)
    }
    
    override func scrollWheel(with event: NSEvent) {
         CEMouse.ScrollMouse(deltaY: Float(event.deltaY))
    }
    
    override func mouseDragged(with event: NSEvent) {
         setMousePositionChanged(event: event)
    }
    
    override func rightMouseDragged(with event: NSEvent) {
         setMousePositionChanged(event: event)
    }
    
    override func otherMouseDragged(with event: NSEvent) {
         setMousePositionChanged(event: event)
    }
    
    private func setMousePositionChanged(event: NSEvent){
         let overallLocation = float2(Float(event.locationInWindow.x),
                                      Float(event.locationInWindow.y))
         let deltaChange = float2(Float(event.deltaX),
                                  Float(event.deltaY))
         CEMouse.SetMousePositionChange(overallPosition: overallLocation,
                                      deltaPosition: deltaChange)
    }
    
    override func updateTrackingAreas() {
         let area = NSTrackingArea(rect: self.bounds,
                                   options: [NSTrackingArea.Options.activeAlways,
                                             NSTrackingArea.Options.mouseMoved,
                                             NSTrackingArea.Options.enabledDuringMouseDrag],
                                   owner: self,
                                   userInfo: nil)
         self.addTrackingArea(area)
    }
}
