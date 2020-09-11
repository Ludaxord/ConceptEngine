//
//  CENode.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CENode {
    
    public func render(renderCommandEncoder: MTLRenderCommandEncoder) {
        if let renderable = self as? CERenderable {
            renderable.doRender(renderCommandEncoder)
        }
    }
    
}
