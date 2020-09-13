//
//  Renderer.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CERenderer: NSObject {
    
    var GameObjects: [CENode] = []

    override init() {
        GameObjects.append(CEGamePlayer())
        GameObjects.append(CEGameNPC())
    }
}


extension CERenderer: MTKViewDelegate {
    
    public func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
    }
    
    public func draw(in view: MTKView) {
        guard let drawable = view.currentDrawable, let renderPassDescriptor = view.currentRenderPassDescriptor else { return }
        let commandBuffer = ConceptEngine.CommandQueue.makeCommandBuffer()
        let renderCommandEncoder = commandBuffer?.makeRenderCommandEncoder(descriptor: renderPassDescriptor)
        for object in GameObjects {
            if let gameObject = object as? CEGameObject {
                gameObject.update(deltaTime: 1 / Float(view.preferredFramesPerSecond))
            }
            object.render(renderCommandEncoder: renderCommandEncoder!)
        }
        renderCommandEncoder?.endEncoding()
        commandBuffer?.present(drawable)
        commandBuffer?.commit()
    }
    
    
}

extension CERenderer {
//    func getEngine() -> ConceptEngine {
//        return self.Engine
//    }
}
