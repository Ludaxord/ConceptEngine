//
//  ConceptView.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 08/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class ConceptView: MTKView {
    
    var commandQueue: MTLCommandQueue!
    var renderPipelineState: MTLRenderPipelineState!
    
    var vertices: [Vertex]!
    
    var vertexBuffer: MTLBuffer!
    
    required init(coder: NSCoder) {
        super.init(coder: coder)
        self.device = MTLCreateSystemDefaultDevice()
        self.clearColor = MTLClearColor(red: 0.89, green: 0.29, blue: 0.10, alpha: 1.0)
        self.colorPixelFormat = .bgra8Unorm
        self.commandQueue = device?.makeCommandQueue()
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
        let library = device?.makeDefaultLibrary()
        let vertexFunction = library?.makeFunction(name: "basic_vertex_shader")
        let fragmentFunction = library?.makeFunction(name: "basic_fragment_shader")
        
        let vertexDescriptor = MTLVertexDescriptor()
        vertexDescriptor.attributes[0].format = .float3
        vertexDescriptor.attributes[0].bufferIndex = 0
        vertexDescriptor.attributes[0].offset = 0
        
        vertexDescriptor.attributes[1].format = .float4
        vertexDescriptor.attributes[1].bufferIndex = 0
        vertexDescriptor.attributes[1].offset = float3.size()
        
        vertexDescriptor.layouts[0].stride = Vertex.stride()
        
        let renderPipelineStateDescriptor = MTLRenderPipelineDescriptor()
        renderPipelineStateDescriptor.colorAttachments[0].pixelFormat = .bgra8Unorm
        renderPipelineStateDescriptor.vertexFunction = vertexFunction
        renderPipelineStateDescriptor.fragmentFunction = fragmentFunction
        renderPipelineStateDescriptor.vertexDescriptor = vertexDescriptor
        do {
            renderPipelineState = try device?.makeRenderPipelineState(descriptor: renderPipelineStateDescriptor)
        } catch let error as NSError {
            print(error)
        }
        
    }
    
    func createBuffers() {
        print("Vertex stride: \(Vertex.stride()) Vertex stride * vertices.count \(Vertex.stride(vertices.count))")
        vertexBuffer = device?.makeBuffer(bytes: vertices, length: Vertex.stride(vertices.count), options: [])
    }
    
    override func draw(_ dirtyRect: NSRect) {
        guard let drawable = self.currentDrawable, let renderPassDescriptor = self.currentRenderPassDescriptor else { return }
        let commandBuffer = commandQueue.makeCommandBuffer()
        let renderCommandEncoder = commandBuffer?.makeRenderCommandEncoder(descriptor: renderPassDescriptor)
        renderCommandEncoder?.setRenderPipelineState(renderPipelineState)
        
        renderCommandEncoder?.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
        renderCommandEncoder?.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertices.count)
        
        renderCommandEncoder?.endEncoding()
        commandBuffer?.present(drawable)
        commandBuffer?.commit()
    }
}
