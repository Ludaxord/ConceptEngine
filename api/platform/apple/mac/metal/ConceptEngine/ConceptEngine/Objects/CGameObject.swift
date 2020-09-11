//
//  CGameObject.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObject {
    
    var vertices: [Vertex]!
    var vertexBuffer: MTLBuffer!
    var engine: ConceptEngine!
    
    init(engine: ConceptEngine? = nil) {
        if engine == nil {
            self.engine = ConceptEngine(device:  MTLCreateSystemDefaultDevice()!)
        } else {
         self.engine = engine
        }
        createVertices()
        createBuffers(device: self.engine.GPUDevice)
    }
    
    func createVertices() {
        vertices = [
            Vertex(position: float3( 0, 1, 0), color: float4(1, 0, 0, 1)),
            Vertex(position: float3(-1,-1, 0), color: float4(1, 1, 0, 1)),
            Vertex(position: float3( 1,-1, 0), color: float4(0, 0, 1, 1)),
        ]
    }
    
    func createBuffers(device: MTLDevice) {
        vertexBuffer = device.makeBuffer(bytes: vertices, length: Vertex.stride(vertices.count), options: [])
    }
    
    func render(renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setRenderPipelineState((engine.getLibrary(.RenderPipelineState) as! CRenderPipelineStateLibrary).PipelineState(.Basic))
        renderCommandEncoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
        renderCommandEncoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertices.count)
    }
}
