//
//  Renderer.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CERenderer: NSObject {
    public static var ScreenSize = float2(0,0)
    public static var AspectRatio: Float {
        return ScreenSize.x / ScreenSize.y
    }
    init(_ mtkView: MTKView) {
        super.init()
        updateScreenSize(view: mtkView)
    }
}


extension CERenderer: MTKViewDelegate {
    
    public func updateScreenSize(view: MTKView){
        CERenderer.ScreenSize = float2(Float(view.bounds.width), Float(view.bounds.height))
    }
    
    public func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        updateScreenSize(view: view)
    }
    
    public func draw(in view: MTKView) {
        guard let renderPassDescriptor = view.currentRenderPassDescriptor else { return }
        let commandBuffer = ConceptEngine.CommandQueue.makeCommandBuffer()
        commandBuffer?.label = "CE Command Buffer"
        
        let renderCommandEncoder = commandBuffer?.makeRenderCommandEncoder(descriptor: renderPassDescriptor)
        renderCommandEncoder?.label = "CE Render Command Encoder"
        
        (ConceptEngine.getManager(.SceneManager) as! CESceneManager).GenerateScene(renderCommandEncoder: renderCommandEncoder!, deltaTime: 1 / Float(view.preferredFramesPerSecond))
        
        renderCommandEncoder?.endEncoding()
        commandBuffer?.present(view.currentDrawable!)
        commandBuffer?.commit()
    }
    
    
}

