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
    var indicates: [UInt32]!
    
    var vertexBuffer: MTLBuffer!
    var indexBuffer: MTLBuffer!
    
    var instanceCount: Int = 1
    
    var vertexCount: Int! {
        return vertices.count
    }
    var indexCount: Int! {
        return indicates.count
    }
    
    init() {
        vertices = []
        indicates = []
        createMesh()
        createBuffers(device: ConceptEngine.GPUDevice)
    }
    
    func populateVertexes() throws {
        if vertexOptions != nil {
            for (position, color, textureCoordinate) in vertexOptions.combination {
                addVertex(position: position, color: color, textureCoordinate: textureCoordinate)
            }
        } else {
            addDefaultVertex()
        }
    }
    
    func createMesh() {
        addDefaultVertex()
    }
        
    func addDefaultVertex() {}
    
    func addVertex(position: float3, color: float4 = float4(1, 0, 1, 1), textureCoordinate: float2 = float2(0, 0), normal: float3 = float3(0, 1, 0)) {
        vertices.append(CEVertex(position: position, color: color, textureCoordinate: textureCoordinate, normal: normal))
    }
    
    func addIndicates(_ indicates: [UInt32]) {
        self.indicates.append(contentsOf: indicates)
    }
    
    func createBuffers(device: MTLDevice) {
        if vertexCount > 0 {
            vertexBuffer = device.makeBuffer(bytes: vertices, length: CEVertex.stride(vertexCount), options: [])
        }
        
        if indexCount > 0 {
            indexBuffer = device.makeBuffer(bytes: indicates, length: UInt32.stride(indexCount), options: [])
        }
    }
    
    func setInstanceCount(_ count: Int) {
        self.instanceCount = count
    }
    
    func drawPrimitives(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        if vertexCount > 0 {
            renderCommandEncoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
            if indexCount > 0 {
                renderCommandEncoder.drawIndexedPrimitives(type: .triangle, indexCount: indexCount, indexType: .uint32, indexBuffer: indexBuffer, indexBufferOffset: 0, instanceCount: instanceCount)
            } else {
                renderCommandEncoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertexCount, instanceCount: instanceCount)
            }
        }
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
    override func addDefaultVertex() {
        addVertex(position: float3( 0, 1,0), color: float4(1,0,0,1))
        addVertex(position: float3(-1,-1,0), color: float4(0,1,0,1))
        addVertex(position: float3( 1,-1,0), color: float4(0,0,1,1))
    }
}

final class CEQuadGameMesh: CEGameMesh {
    
    override func addDefaultVertex() {
        addVertex(position: float3( 1, 1,0), color: float4(1,0,0,1), textureCoordinate: float2(1,0), normal: float3(0,0,1)) //Top Right
        addVertex(position: float3(-1, 1,0), color: float4(0,1,0,1), textureCoordinate: float2(0,0), normal: float3(0,0,1)) //Top Left
        addVertex(position: float3(-1,-1,0), color: float4(0,0,1,1), textureCoordinate: float2(0,1), normal: float3(0,0,1)) //Bottom Left
//        addVertex(position: float3( 1, 1,0), color: float4(1,0,0,1), textureCoordinate: float2(1,0), normal: float3(0,0,1)) //Top Right
//        addVertex(position: float3(-1,-1,0), color: float4(0,0,1,1), textureCoordinate: float2(0,1), normal: float3(0,0,1)) //Bottom Left
        addVertex(position: float3( 1,-1,0), color: float4(1,0,1,1), textureCoordinate: float2(1,1), normal: float3(0,0,1)) //Bottom Right
        
        addIndicates([
            0,1,2,
            0,2,3
        ])
    }
}

final class CECubeGameMesh: CEGameMesh {
    
    override func addDefaultVertex() {
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
