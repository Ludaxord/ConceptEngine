//
//  CEMeshLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum MeshTypes {
    case Triangle
    case Quad
    case Cube
}

public protocol CEMesh {
    var vertexBuffer: MTLBuffer! { get }
    var vertexCount: Int! { get }
    func setInstanceCount(_ count: Int)
    func drawPrimitives(_ renderCommandEncoder: MTLRenderCommandEncoder)
}

class CEGameMesh: CEMesh {
    
    var vertices: [CEVertex]!
    var vertexBuffer: MTLBuffer!
    var instanceCount: Int = 1
    var vertexCount: Int! {
        return vertices.count
    }
    
    init() {
        createVertices()
        createBuffers(device: ConceptEngine.GPUDevice)
    }
    
    func createVertices() {}
    
    func createBuffers(device: MTLDevice) {
        vertexBuffer = device.makeBuffer(bytes: vertices, length: CEVertex.stride(vertices.count), options: [])
    }
    
    func setInstanceCount(_ count: Int) {
        self.instanceCount = count
    }
    
    func drawPrimitives(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
        renderCommandEncoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertexCount, instanceCount: instanceCount)
    }
}

final class CETriangleGameMesh: CEGameMesh {
    override func createVertices() {
        vertices = [
            CEVertex(position: float3( 0, 1, 0), color: float4(1, 0, 0, 1)),
            CEVertex(position: float3(-1,-1, 0), color: float4(1, 1, 0, 1)),
            CEVertex(position: float3( 1,-1, 0), color: float4(0, 0, 1, 1)),
        ]
    }
}

final class CEQuadGameMesh: CEGameMesh {
    override func createVertices() {
        vertices = [
            CEVertex(position: float3( 0.5, 0.5, 0), color: float4(1, 0, 0, 1)),
            CEVertex(position: float3(-0.5, 0.5, 0), color: float4(0, 1, 0, 1)),
            CEVertex(position: float3(-0.5,-0.5, 0), color: float4(0, 0, 1, 1)),
            
            CEVertex(position: float3( 0.5, 0.5, 0), color: float4(1, 0, 0, 1)),
            CEVertex(position: float3(-0.5,-0.5, 0), color: float4(0, 0, 1, 1)),
            CEVertex(position: float3( 0.5,-0.5, 0), color: float4(1, 0, 1, 1)),
        ]
    }
}

final class CECubeGameMesh: CEGameMesh {
    override func createVertices() {
        vertices = [
            //Left
            CEVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 0.5, 0.0, 1.0)),
            CEVertex(position: float3(-1.0,-1.0, 1.0), color: float4(0.0, 1.0, 0.5, 1.0)),
            CEVertex(position: float3(-1.0, 1.0, 1.0), color: float4(0.0, 0.5, 1.0, 1.0)),
            CEVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 1.0, 0.0, 1.0)),
            CEVertex(position: float3(-1.0, 1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0)),
            CEVertex(position: float3(-1.0, 1.0,-1.0), color: float4(1.0, 0.0, 1.0, 1.0)),
            
            //RIGHT
            CEVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 0.0, 0.5, 1.0)),
            CEVertex(position: float3( 1.0,-1.0,-1.0), color: float4(0.0, 1.0, 0.0, 1.0)),
            CEVertex(position: float3( 1.0, 1.0,-1.0), color: float4(0.0, 0.5, 1.0, 1.0)),
            CEVertex(position: float3( 1.0,-1.0,-1.0), color: float4(1.0, 1.0, 0.0, 1.0)),
            CEVertex(position: float3( 1.0, 1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0)),
            CEVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 0.5, 1.0, 1.0)),
            
            //TOP
            CEVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 0.0, 0.0, 1.0)),
            CEVertex(position: float3( 1.0, 1.0,-1.0), color: float4(0.0, 1.0, 0.0, 1.0)),
            CEVertex(position: float3(-1.0, 1.0,-1.0), color: float4(0.0, 0.0, 1.0, 1.0)),
            CEVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 1.0, 0.0, 1.0)),
            CEVertex(position: float3(-1.0, 1.0,-1.0), color: float4(0.5, 1.0, 1.0, 1.0)),
            CEVertex(position: float3(-1.0, 1.0, 1.0), color: float4(1.0, 0.0, 1.0, 1.0)),
            
            //BOTTOM
            CEVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 0.5, 0.0, 1.0)),
            CEVertex(position: float3(-1.0,-1.0,-1.0), color: float4(0.5, 1.0, 0.0, 1.0)),
            CEVertex(position: float3( 1.0,-1.0,-1.0), color: float4(0.0, 0.0, 1.0, 1.0)),
            CEVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 1.0, 0.5, 1.0)),
            CEVertex(position: float3(-1.0,-1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0)),
            CEVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 0.5, 1.0, 1.0)),
            
            //BACK
            CEVertex(position: float3( 1.0, 1.0,-1.0), color: float4(1.0, 0.5, 0.0, 1.0)),
            CEVertex(position: float3(-1.0,-1.0,-1.0), color: float4(0.5, 1.0, 0.0, 1.0)),
            CEVertex(position: float3(-1.0, 1.0,-1.0), color: float4(0.0, 0.0, 1.0, 1.0)),
            CEVertex(position: float3( 1.0, 1.0,-1.0), color: float4(1.0, 1.0, 0.0, 1.0)),
            CEVertex(position: float3( 1.0,-1.0,-1.0), color: float4(0.0, 1.0, 1.0, 1.0)),
            CEVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 0.5, 1.0, 1.0)),
            
            //FRONT
            CEVertex(position: float3(-1.0, 1.0, 1.0), color: float4(1.0, 0.5, 0.0, 1.0)),
            CEVertex(position: float3(-1.0,-1.0, 1.0), color: float4(0.0, 1.0, 0.0, 1.0)),
            CEVertex(position: float3( 1.0,-1.0, 1.0), color: float4(0.5, 0.0, 1.0, 1.0)),
            CEVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 1.0, 0.5, 1.0)),
            CEVertex(position: float3(-1.0, 1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0)),
            CEVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 0.0, 1.0, 1.0))
        ]
    }
}

public final class CEMeshLibrary: CEStandardLibrary {

     private var meshes: [MeshTypes: CEMesh] = [:]
          
     required init() {
         createDefaultMeshes()
     }
    
    public func Mesh(_ meshType: MeshTypes) -> CEMesh {
        return meshes[meshType]!
    }
    
    private func createDefaultMeshes() {
       meshes.updateValue(CETriangleGameMesh(), forKey: .Triangle)
       meshes.updateValue(CEQuadGameMesh(), forKey: .Quad)
       meshes.updateValue(CECubeGameMesh(), forKey: .Cube)
    }
    
}
