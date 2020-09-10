//
//  ConceptView.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 08/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit
import Foundation

@objc(ConceptView)
class ConceptView: MTKView {
    
    var utilities: Utilities!
    
    var engine: ConceptEngine!
    
    var renderPipelineState: MTLRenderPipelineState!
    
    var vertices: [Vertex]!
    
    var vertexBuffer: MTLBuffer!
    
    required init(coder: NSCoder) {
        super.init(coder: coder)
        self.utilities = Utilities()
        self.device = MTLCreateSystemDefaultDevice()
        self.engine = ConceptEngine.Initialize(device: self.device!)
        self.clearColor = utilities.ClearColor
        self.colorPixelFormat = utilities.PixelFormat
        createRenderPipelineState()
        createVertices()
        createBuffers()
    }
    
    func createVertices() {
        vertices = [
            Vertex(position: float3( 0, 1, 0), color: float4(1, 0, 0, 1)),
            Vertex(position: float3(-1,-1, 0), color: float4(1, 1, 0, 1)),
            Vertex(position: float3( 1,-1, 0), color: float4(0, 0, 1, 1)),
        ]
    }
    
    func createRenderPipelineState() {

        
        let renderPipelineStateDescriptor = MTLRenderPipelineDescriptor()
        renderPipelineStateDescriptor.colorAttachments[0].pixelFormat = utilities.PixelFormat
        renderPipelineStateDescriptor.vertexFunction = engine.ShaderLibrary.Vertex(.Basic)
        renderPipelineStateDescriptor.fragmentFunction = engine.ShaderLibrary.Fragment(.Basic)
        renderPipelineStateDescriptor.vertexDescriptor = vertexDescriptor
        do {
            renderPipelineState = try self.engine.Device.makeRenderPipelineState(descriptor: renderPipelineStateDescriptor)
        } catch let error as NSError {
            print(error)
        }
        
    }
    
    func createBuffers() {
        print("Vertex stride: \(Vertex.stride) Vertex stride * vertices.count \(Vertex.stride(vertices.count))")
        vertexBuffer = self.engine.Device.makeBuffer(bytes: vertices, length: Vertex.stride(vertices.count), options: [])
    }
    
    override func draw(_ dirtyRect: NSRect) {
        guard let drawable = self.currentDrawable, let renderPassDescriptor = self.currentRenderPassDescriptor else { return }
        let commandBuffer = self.engine.CommandQueue.makeCommandBuffer()
        let renderCommandEncoder = commandBuffer?.makeRenderCommandEncoder(descriptor: renderPassDescriptor)
        renderCommandEncoder?.setRenderPipelineState(renderPipelineState)
        
        renderCommandEncoder?.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
        renderCommandEncoder?.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertices.count)
        
        renderCommandEncoder?.endEncoding()
        commandBuffer?.present(drawable)
        commandBuffer?.commit()
    }
}
