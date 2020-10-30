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
    case Monkeys
    case DigimonAgumon
}

public class CEGameMesh {
    var submeshes: [CESubMesh]!
    var vertices: [CEVertex]!
    
    var vertexBuffer: MTLBuffer!
    
    var instanceCount: Int = 1
    
    var vertexCount: Int = 0

    init() {
        vertices = []
        submeshes = []
        createMesh()
        createBuffer(device: ConceptEngine.GPUDevice)
    }
    
    init(modelName: String) {
        vertices = []
        submeshes = []
        loadModel(modelName)
    }
    
    func loadModel(_ modelName: String, ext: String = "obj") {
        guard let assetUrl = Bundle.main.url(forResource: modelName, withExtension: ext) else {
            fatalError("Asset \(modelName) does not exist...")
        }
        
        let descriptor = MTKModelIOVertexDescriptorFromMetal((ConceptEngine.getLibrary(.VertexDescriptor) as! CEVertexDescriptorLibrary).Descriptor(.Basic))
        (descriptor.attributes[0] as! MDLVertexAttribute).name = MDLVertexAttributePosition
        (descriptor.attributes[1] as! MDLVertexAttribute).name = MDLVertexAttributeColor
        (descriptor.attributes[2] as! MDLVertexAttribute).name = MDLVertexAttributeTextureCoordinate
        (descriptor.attributes[3] as! MDLVertexAttribute).name = MDLVertexAttributeNormal
        
        let bufferAllocator = MTKMeshBufferAllocator(device: ConceptEngine.GPUDevice)
        let asset: MDLAsset = MDLAsset(url: assetUrl, vertexDescriptor: descriptor, bufferAllocator: bufferAllocator, preserveTopology: true, error: nil)
        asset.loadTextures()
        
        var mtkMeshes: [MTKMesh] = []
        var mdlMeshes: [MDLMesh] = []
        do {
            mtkMeshes = try MTKMesh.newMeshes(asset: asset, device: ConceptEngine.GPUDevice).metalKitMeshes
            mdlMeshes = try MTKMesh.newMeshes(asset: asset, device: ConceptEngine.GPUDevice).modelIOMeshes
        } catch {
            print("ERROR::LOADING_MESH::__\(modelName)__::\(error)")
        }
        
        let mtkMesh = mtkMeshes[0]
        let mdlMesh = mdlMeshes[0]
        self.vertexBuffer = mtkMesh.vertexBuffers[0].buffer
        self.vertexCount = mtkMesh.vertexCount
        for i in 0..<mtkMesh.submeshes.count {
            let mtkSubmesh = mtkMesh.submeshes[i]
            let mdlSubmesh = mdlMesh.submeshes![i] as? MDLSubmesh
            let submesh = CESubMesh(mtkSubmesh: mtkSubmesh, mdlSubmesh: mdlSubmesh!)
            addSubMesh(submesh)
        }
    }
    
    func createMesh() {
        addDefaultVertex()
    }
        
    func addDefaultVertex() {}
    
    func addVertex(position: float3, color: float4 = float4(1, 0, 1, 1), textureCoordinate: float2 = float2(0, 0), normal: float3 = float3(0, 1, 0)) {
        vertices.append(CEVertex(position: position, color: color, textureCoordinate: textureCoordinate, normal: normal))
    }
        
    func createBuffer(device: MTLDevice) {
        if vertexCount > 0 {
            vertexBuffer = device.makeBuffer(bytes: vertices, length: CEVertex.stride(vertices.count), options: [])
        }
    }
    
    func setInstanceCount(_ count: Int) {
        self.instanceCount = count
    }
    
    func addSubMesh(_ submesh: CESubMesh) {
        submeshes.append(submesh)
    }
    
    func drawPrimitives(_ renderCommandEncoder: MTLRenderCommandEncoder, baseColorTextureType: TextureTypes = .None) {
        if vertexBuffer != nil {
            renderCommandEncoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
            if submeshes.count > 0 {
                for submesh in submeshes {
                    submesh.applyTexture(renderCommandEncoder: renderCommandEncoder, customBaseColorTextureType: baseColorTextureType)
                    renderCommandEncoder.drawIndexedPrimitives(type: submesh.primitiveType,
                                                                indexCount: submesh.indexCount,
                                                                indexType: submesh.indexType,
                                                                indexBuffer: submesh.indexBuffer,
                                                                indexBufferOffset: submesh.indexBufferOffset)
                }
            } else {
                renderCommandEncoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertexCount, instanceCount: instanceCount)
            }
        }
    }
}

class CESubMesh {
    
    private var _indices: [UInt32] = []
    
    private var _indexCount: Int = 0
    public var indexCount: Int { return _indexCount }
    
    private var _indexBuffer: MTLBuffer!
    public var indexBuffer: MTLBuffer { return _indexBuffer }

    private var _primitiveType: MTLPrimitiveType = .triangle
    public var primitiveType: MTLPrimitiveType { return _primitiveType }
    
    private var _indexType: MTLIndexType = .uint32
    public var indexType: MTLIndexType { return _indexType }
    
    private var _indexBufferOffset: Int = 0
    public var indexBufferOffset: Int { return _indexBufferOffset }
    
    private var _material: CEMaterial!
    
    private var _baseColorTexture: MTLTexture!
    
    var vertexBuffer: MTLBuffer!
    
    init(indices: [UInt32]) {
        self._material = CEMaterial()
        self._indices = indices
        self._indexCount = indices.count
        createIndexBuffer()
    }
    
    init(mtkSubmesh: MTKSubmesh, mdlSubmesh: MDLSubmesh) {
        _material = CEMaterial()
        _indexBuffer = mtkSubmesh.indexBuffer.buffer
        _indexBufferOffset = mtkSubmesh.indexBuffer.offset
        _indexCount = mtkSubmesh.indexCount
        _indexType = mtkSubmesh.indexType
        _primitiveType = mtkSubmesh.primitiveType
        
        createTexture(mdlSubmesh.material!)
        createMaterial(mdlSubmesh.material!)
    }
    
    private func texture(for semantic: MDLMaterialSemantic, in material: MDLMaterial?, textureOrigin: MTKTextureLoader.Origin) -> MTLTexture? {
        let textureLoader = MTKTextureLoader(device: ConceptEngine.GPUDevice)
        guard let materialProperty = material?.property(with: semantic) else { return nil }
        guard let sourceTexture = materialProperty.textureSamplerValue?.texture else { return nil }
        let options: [MTKTextureLoader.Option: Any] = [
            MTKTextureLoader.Option.origin: textureOrigin as Any,
            MTKTextureLoader.Option.generateMipmaps: true
        ]
        let tex = try? textureLoader.newTexture(texture: sourceTexture, options: options)
        return tex
    }
    
    private func createTexture(_ mdlMaterial: MDLMaterial) {
        _baseColorTexture = texture(for: .baseColor, in: mdlMaterial, textureOrigin: .bottomLeft)
    }
    
    func applyTexture(renderCommandEncoder: MTLRenderCommandEncoder, customBaseColorTextureType: TextureTypes) {
        renderCommandEncoder.setFragmentSamplerState((ConceptEngine.getLibrary(.SamplerState) as! CESamplerStateLibrary).SamplerState(.Linear), index: 0)
        let baseColorTex = customBaseColorTextureType == .None ? _baseColorTexture : (ConceptEngine.getLibrary(.Texture) as! CETextureLibrary).Texture(customBaseColorTextureType)
        renderCommandEncoder.setFragmentTexture(baseColorTex, index: 0)
    }
    
    private func createMaterial(_ mdlMaterial: MDLMaterial) {
        
    }
        
    private func createIndexBuffer() {
          if(_indices.count > 0) {
              _indexBuffer = ConceptEngine.GPUDevice.makeBuffer(bytes: _indices,
                                                      length: UInt32.stride(_indices.count),
                                                      options: [])
          }
      }
    
    
}

final class CEEmptyMesh: CEGameMesh {
    override func setInstanceCount(_ count: Int) { }
}

final class CETriangleGameMesh: CEGameMesh {
    override func addDefaultVertex() {
        addVertex(position: float3( 0, 1,0), color: float4(1,0,0,1), textureCoordinate: float2(0.5,0.0))
        addVertex(position: float3(-1,-1,0), color: float4(0,1,0,1), textureCoordinate: float2(0.0,1.0))
        addVertex(position: float3( 1,-1,0), color: float4(0,0,1,1), textureCoordinate: float2(1.0,1.0))
    }
}

final class CEQuadGameMesh: CEGameMesh {
    
    override func addDefaultVertex() {
        addVertex(position: float3( 1, 1,0), color: float4(1,0,0,1), textureCoordinate: float2(1,0), normal: float3(0,0,1)) //Top Right
        addVertex(position: float3(-1, 1,0), color: float4(0,1,0,1), textureCoordinate: float2(0,0), normal: float3(0,0,1)) //Top Left
        addVertex(position: float3(-1,-1,0), color: float4(0,0,1,1), textureCoordinate: float2(0,1), normal: float3(0,0,1)) //Bottom Left
        addVertex(position: float3( 1,-1,0), color: float4(1,0,1,1), textureCoordinate: float2(1,1), normal: float3(0,0,1)) //Bottom Right
        
        addSubMesh(CESubMesh(indices: [
            0,1,2,
            0,2,3
        ]))
    }
}

final class CECubeGameMesh: CEGameMesh {
    
    override func addDefaultVertex() {
        //Left
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 0.5, 0.0, 1.0), normal: float3(-1, 0, 0))
        addVertex(position: float3(-1.0,-1.0, 1.0), color: float4(0.0, 1.0, 0.5, 1.0), normal: float3(-1, 0, 0))
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(0.0, 0.5, 1.0, 1.0), normal: float3(-1, 0, 0))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 1.0, 0.0, 1.0), normal: float3(-1, 0, 0))
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0), normal: float3(-1, 0, 0))
        addVertex(position: float3(-1.0, 1.0,-1.0), color: float4(1.0, 0.0, 1.0, 1.0), normal: float3(-1, 0, 0))
        
        //RIGHT
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 0.0, 0.5, 1.0), normal: float3( 1, 0, 0))
        addVertex(position: float3( 1.0,-1.0,-1.0), color: float4(0.0, 1.0, 0.0, 1.0), normal: float3( 1, 0, 0))
        addVertex(position: float3( 1.0, 1.0,-1.0), color: float4(0.0, 0.5, 1.0, 1.0), normal: float3( 1, 0, 0))
        addVertex(position: float3( 1.0,-1.0,-1.0), color: float4(1.0, 1.0, 0.0, 1.0), normal: float3( 1, 0, 0))
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0), normal: float3( 1, 0, 0))
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 0.5, 1.0, 1.0), normal: float3( 1, 0, 0))
        
        //TOP
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 0.0, 0.0, 1.0), normal: float3( 0, 1, 0))
        addVertex(position: float3( 1.0, 1.0,-1.0), color: float4(0.0, 1.0, 0.0, 1.0), normal: float3( 0, 1, 0))
        addVertex(position: float3(-1.0, 1.0,-1.0), color: float4(0.0, 0.0, 1.0, 1.0), normal: float3( 0, 1, 0))
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 1.0, 0.0, 1.0), normal: float3( 0, 1, 0))
        addVertex(position: float3(-1.0, 1.0,-1.0), color: float4(0.5, 1.0, 1.0, 1.0), normal: float3( 0, 1, 0))
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(1.0, 0.0, 1.0, 1.0), normal: float3( 0, 1, 0))
        
        //BOTTOM
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 0.5, 0.0, 1.0), normal: float3( 0,-1, 0))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(0.5, 1.0, 0.0, 1.0), normal: float3( 0,-1, 0))
        addVertex(position: float3( 1.0,-1.0,-1.0), color: float4(0.0, 0.0, 1.0, 1.0), normal: float3( 0,-1, 0))
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 1.0, 0.5, 1.0), normal: float3( 0,-1, 0))
        addVertex(position: float3(-1.0,-1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0), normal: float3( 0,-1, 0))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 0.5, 1.0, 1.0), normal: float3( 0,-1, 0))
        
        //BACK
        addVertex(position: float3( 1.0, 1.0,-1.0), color: float4(1.0, 0.5, 0.0, 1.0), normal: float3( 0, 0,-1))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(0.5, 1.0, 0.0, 1.0), normal: float3( 0, 0,-1))
        addVertex(position: float3(-1.0, 1.0,-1.0), color: float4(0.0, 0.0, 1.0, 1.0), normal: float3( 0, 0,-1))
        addVertex(position: float3( 1.0, 1.0,-1.0), color: float4(1.0, 1.0, 0.0, 1.0), normal: float3( 0, 0,-1))
        addVertex(position: float3( 1.0,-1.0,-1.0), color: float4(0.0, 1.0, 1.0, 1.0), normal: float3( 0, 0,-1))
        addVertex(position: float3(-1.0,-1.0,-1.0), color: float4(1.0, 0.5, 1.0, 1.0), normal: float3( 0, 0,-1))
        
        //FRONT
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(1.0, 0.5, 0.0, 1.0), normal: float3( 0, 0, 1))
        addVertex(position: float3(-1.0,-1.0, 1.0), color: float4(0.0, 1.0, 0.0, 1.0), normal: float3( 0, 0, 1))
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(0.5, 0.0, 1.0, 1.0), normal: float3( 0, 0, 1))
        addVertex(position: float3( 1.0, 1.0, 1.0), color: float4(1.0, 1.0, 0.5, 1.0), normal: float3( 0, 0, 1))
        addVertex(position: float3(-1.0, 1.0, 1.0), color: float4(0.0, 1.0, 1.0, 1.0), normal: float3( 0, 0, 1))
        addVertex(position: float3( 1.0,-1.0, 1.0), color: float4(1.0, 0.0, 1.0, 1.0), normal: float3( 0, 0, 1))
    }
}

public final class CEMeshLibrary: CELibrary<MeshTypes, CEGameMesh>, CEStandardLibrary {

    private var meshes: [MeshTypes: CEGameMesh] = [:]
    
    override subscript(type: MeshTypes) -> CEGameMesh? {
        return Mesh(type)
    }
    
    override func useLibrary() {
        createDefaultMeshes()
    }
    
    public func Mesh(_ meshType: MeshTypes) -> CEGameMesh {
        return meshes[meshType]!
    }
    
    private func createDefaultMeshes() {
        meshes.updateValue(CEEmptyMesh(), forKey: .None)
        meshes.updateValue(CETriangleGameMesh(), forKey: .Triangle)
        meshes.updateValue(CEQuadGameMesh(), forKey: .Quad)
        meshes.updateValue(CECubeGameMesh(), forKey: .Cube)
        meshes.updateValue(CEGameMesh(modelName: "zuk"), forKey: .CarTruck)
        meshes.updateValue(CEGameMesh(modelName: "golf"), forKey: .CarHatch)
        meshes.updateValue(CEGameMesh(modelName: "aston_martin"), forKey: .CarSport)
        meshes.updateValue(CEGameMesh(modelName: "sphere"), forKey: .Sphere)
        meshes.updateValue(CEGameMesh(modelName: "monkeys"), forKey: .Monkeys)
        meshes.updateValue(CEGameMesh(modelName: "Agumon"), forKey: .DigimonAgumon)
        
    }
    
}
