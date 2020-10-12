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
    var vertexOptions: CEVertexOptions! { get set }
    func setInstanceCount(_ count: Int)
    func drawPrimitives(_ renderCommandEncoder: MTLRenderCommandEncoder)
}

class CEGameMesh: CEMesh {
    var vertexOptions: CEVertexOptions!
    var vertices: [CEVertex]!
    var vertexBuffer: MTLBuffer!
    var instanceCount: Int = 1
    var vertexCount: Int! {
        return vertices.count
    }
    
    init() {
        vertices = []
        createVertexOptions()
        createVertices()
        createBuffers(device: ConceptEngine.GPUDevice)
    }
    
    //in all overrides create addVertex function with vertexOptions instead of creating every vertex manually.
    func createVertices() {}
    
    func createVertexOptions() {}
    
    func addVertex(position: float3, color: float4 = float4(1, 0, 1, 1), textureCoordinate: float2 = float2(0)) {
        vertices.append(CEVertex(position: position, color: color
//            ,
//                                 textureCoordinate: textureCoordinate
        ))
    }
    
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
    override func createVertexOptions() {
        vertexOptions = CEVertexOptions(
            positions: [float3( 0, 1, 0), float3(-1,-1, 0), float3( 1,-1, 0)],
            colors: [float4(1, 0, 0, 1), float4(1, 1, 0, 1), float4(0, 0, 1, 1)],
            textureCoordinate: []
        )
    }
    
    override func createVertices() {
        vertices = [
            CEVertex(position: float3( 0, 1, 0), color: float4(1, 0, 0, 1)),
            CEVertex(position: float3(-1,-1, 0), color: float4(1, 1, 0, 1)),
            CEVertex(position: float3( 1,-1, 0), color: float4(0, 0, 1, 1)),
        ]
    }
}

final class CEQuadGameMesh: CEGameMesh {
    
    override func createVertexOptions() {
        vertexOptions = CEVertexOptions(
            positions: [float3( 0.5, 0.5, 0), float3(-0.5, 0.5, 0), float3(-0.5,-0.5, 0), float3( 0.5, 0.5, 0), float3(-0.5,-0.5, 0), float3( 0.5,-0.5, 0)],
            colors: [float4(1, 0, 0, 1), float4(0, 1, 0, 1), float4(0, 0, 1, 1), float4(1, 0, 0, 1), float4(0, 0, 1, 1), float4(1, 0, 1, 1)],
            textureCoordinate: []
        )
    }
    
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
    
    override func createVertexOptions() {
        vertexOptions = CEVertexOptions(
            positions: [float3( 0, 1, 0), float3(-1,-1, 0), float3( 1,-1, 0)],
            colors: [float4(1, 0, 0, 1), float4(1, 1, 0, 1), float4(0, 0, 1, 1)],
            textureCoordinate: []
        )
    }
    
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
