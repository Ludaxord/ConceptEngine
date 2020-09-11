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
        self.Engine = ConceptEngine.Initialize()
        self.Engine.injectGpuToView(view: self)
        self.Engine.injectDefaultColorsToView(view: self)
        self.Renderer = CERenderer()
        self.delegate = self.Renderer
    }
}
