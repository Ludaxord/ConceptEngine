//
//  Renderable.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

protocol CERenderable {
    func doRender(_ renderCommandEncoder: MTLRenderCommandEncoder)
}
