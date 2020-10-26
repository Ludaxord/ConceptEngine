//
//  CEMeshLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum MeshTypes {
    case None
    
    case Triangle
    case Quad
    case Cube
    
    case Sphere
    
    case CarTruck
    case CarSport
    case CarHatch
}

public protocol CEMesh {
    var vertexBuffer: MTLBuffer! { get }
//    var vertexCount: Int! { get }
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
        createVertices()
        createBuffers(device: ConceptEngine.GPUDevice)
    }
    
    func populateVertexes() throws {
        if vertexOptions != nil {
            for (position, color, textureCoordinate) in vertexOptions.combination {
                addVertex(position: position, color: color, textureCoordinate: textureCoordinate)
            }
        } else {
            defaultVertexCreation()
        }
    }
    
    func createVertices() {
        defaultVertexCreation()
    }
        
    func defaultVertexCreation() {}
    
    func addVertex(position: float3, color: float4 = float4(1, 0, 1, 1), textureCoordinate: float2 = float2(0, 0), normal: float3 = float3(0, 1, 0)) {
        vertices.append(CEVertex(position: position, color: color, textureCoordinate: textureCoordinate, normal: normal))
    }
    
    func createBuffers(device: MTLDevice) {
        if vertices.count > 0 {
            vertexBuffer = device.makeBuffer(bytes: vertices, length: CEVertex.stride(vertices.count), options: [])
        } else {
            print("vertices count is less than 0")
        }
    }
    
    func setInstanceCount(_ count: Int) {
        self.instanceCount = count
    }
    
    func drawPrimitives(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
        renderCommandEncoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertexCount, instanceCount: instanceCount)
    }
}

class CEModelGameMesh: CEMesh {
    
    private var _meshes: [Any]!
    
    private var _instanceCount: Int = 1
    
    var vertexBuffer: MTLBuffer!
    
    var vertexOptions: CEVertexOptions!
    
    init(modelName: String) {
        loadModel(modelName: modelName)
    }
    
    func loadModel(modelName: String) {
        guard let assetUrl = Bundle.main.url(forResource: modelName, withExtension: "obj") else {
            fatalError("Asset \(modelName) does not exist...")
        }
        
        let descriptor = MTKModelIOVertexDescriptorFromMetal((ConceptEngine.getLibrary(.VertexDescriptor) as! CEVertexDescriptorLibrary).Descriptor(.Basic))
        (descriptor.attributes[0] as! MDLVertexAttribute).name = MDLVertexAttributePosition
        (descriptor.attributes[1] as! MDLVertexAttribute).name = MDLVertexAttributeColor
        (descriptor.attributes[2] as! MDLVertexAttribute).name = MDLVertexAttributeTextureCoordinate
        (descriptor.attributes[3] as! MDLVertexAttribute).name = MDLVertexAttributeNormal
        
        let bufferAllocator = MTKMeshBufferAllocator(device: ConceptEngine.GPUDevice)
        let asset: MDLAsset = MDLAsset(url: assetUrl, vertexDescriptor: descriptor, bufferAllocator: bufferAllocator)
        do {
            self._meshes = try MTKMesh.newMeshes(asset: asset, device: ConceptEngine.GPUDevice).metalKitMeshes
        } catch {
            print("ERROR::LOADING_MESH::__\(modelName)__::\(error)")
        }
    }
    
    func setInstanceCount(_ count: Int) {
        self._instanceCount = count
    }
    
    func drawPrimitives(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        guard let meshes = self._meshes as? [MTKMesh] else {
            return
        }
        for mesh in meshes {
            for vertexBuffer in mesh.vertexBuffers {
                renderCommandEncoder.setVertexBuffer(vertexBuffer.buffer, offset: vertexBuffer.offset, index: 0)
                for submesh in mesh.submeshes {
                    renderCommandEncoder.drawIndexedPrimitives(type: submesh.primitiveType, indexCount: submesh.indexCount, indexType: submesh.indexType, indexBuffer: submesh.indexBuffer.buffer, indexBufferOffset: submesh.indexBuffer.offset, instanceCount: self._instanceCount)
                }
            }
        }
    }
}

final class CEEmptyMesh: CEGameMesh {
    override func setInstanceCount(_ count: Int) { }
    override func drawPrimitives(_ renderCommandEncoder: MTLRenderCommandEncoder) { }
}

final class CETriangleGameMesh: CEGameMesh {
    override func defaultVertexCreation() {
        addVertex(position: float3( 0, 1,0), color: float4(1,0,0,1))
        addVertex(position: float3(-1,-1,0), color: float4(0,1,0,1))
        addVertex(position: float3( 1,-1,0), color: float4(0,0,1,1))
    }
}

final class CEQuadGameMesh: CEGameMesh {
    
    override func defaultVertexCreation() {
        addVertex(position: float3( 1, 1,0), color: float4(1,0,0,1), textureCoordinate: float2(1,0)) //Top Right
        addVertex(position: float3(-1, 1,0), color: float4(0,1,0,1), textureCoordinate: float2(0,0)) //Top Left
        addVertex(position: float3(-1,-1,0), color: float4(0,0,1,1), textureCoordinate: float2(0,1)) //Bottom Left
        
        addVertex(position: float3( 1, 1,0), color: float4(1,0,0,1), textureCoordinate: float2(1,0)) //Top Right
        addVertex(position: float3(-1,-1,0), color: float4(0,0,1,1), textureCoordinate: float2(0,1)) //Bottom Left
        addVertex(position: float3( 1,-1,0), color: float4(1,0,1,1), textureCoordinate: float2(1,1)) //Bottom Right
    }
}

final class CECubeGameMesh: CEGameMesh {
    
    override func defaultVertexCreation() {
        //Left
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 0.5, 0.0, 1.0))
        addVertex(position: float3(-1.0,-1.0, 1.0), color: float4(0.0, 1.0, 0.5, 1.0))
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(0.0, 0.5, 1.0, 1.0))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 1.0, 0.0, 1.0))
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0))
        addVertex(position: float3(-1.0, 1.0,-1.0), color: float4(1.0, 0.0, 1.0, 1.0))
        
        //RIGHT
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 0.0, 0.5, 1.0))
        addVertex(position: float3( 1.0,-1.0,-1.0), color: float4(0.0, 1.0, 0.0, 1.0))
        addVertex(position: float3( 1.0, 1.0,-1.0), color: float4(0.0, 0.5, 1.0, 1.0))
        addVertex(position: float3( 1.0,-1.0,-1.0), color: float4(1.0, 1.0, 0.0, 1.0))
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0))
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 0.5, 1.0, 1.0))
        
        //TOP
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 0.0, 0.0, 1.0))
        addVertex(position: float3( 1.0, 1.0,-1.0), color: float4(0.0, 1.0, 0.0, 1.0))
        addVertex(position: float3(-1.0, 1.0,-1.0), color: float4(0.0, 0.0, 1.0, 1.0))
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 1.0, 0.0, 1.0))
        addVertex(position: float3(-1.0, 1.0,-1.0), color: float4(0.5, 1.0, 1.0, 1.0))
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(1.0, 0.0, 1.0, 1.0))
        
        //BOTTOM
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 0.5, 0.0, 1.0))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(0.5, 1.0, 0.0, 1.0))
        addVertex(position: float3( 1.0,-1.0,-1.0), color: float4(0.0, 0.0, 1.0, 1.0))
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 1.0, 0.5, 1.0))
        addVertex(position: float3(-1.0,-1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 0.5, 1.0, 1.0))
        
        //BACK
        addVertex(position: float3( 1.0, 1.0,-1.0), color: float4(1.0, 0.5, 0.0, 1.0))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(0.5, 1.0, 0.0, 1.0))
        addVertex(position: float3(-1.0, 1.0,-1.0), color: float4(0.0, 0.0, 1.0, 1.0))
        addVertex(position: float3( 1.0, 1.0,-1.0), color: float4(1.0, 1.0, 0.0, 1.0))
        addVertex(position: float3( 1.0,-1.0,-1.0), color: float4(0.0, 1.0, 1.0, 1.0))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 0.5, 1.0, 1.0))
        
        //FRONT
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(1.0, 0.5, 0.0, 1.0))
        addVertex(position: float3(-1.0,-1.0, 1.0), color: float4(0.0, 1.0, 0.0, 1.0))
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(0.5, 0.0, 1.0, 1.0))
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 1.0, 0.5, 1.0))
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0))
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 0.0, 1.0, 1.0))
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
        meshes.updateValue(CEEmptyMesh(), forKey: .None)
       meshes.updateValue(CETriangleGameMesh(), forKey: .Triangle)
       meshes.updateValue(CEQuadGameMesh(), forKey: .Quad)
       meshes.updateValue(CECubeGameMesh(), forKey: .Cube)
       meshes.updateValue(CEModelGameMesh(modelName: "zuk"), forKey: .CarTruck)
       meshes.updateValue(CEModelGameMesh(modelName: "golf"), forKey: .CarHatch)
       meshes.updateValue(CEModelGameMesh(modelName: "aston_martin"), forKey: .CarSport)
        meshes.updateValue(CEModelGameMesh(modelName: "sphere"), forKey: .Sphere)
    }
    
}
