//
//  CGameObject.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit


public class CEGameObject: CENode {
    
    internal var camera: CECamera!
    
    var model: CEModelDefaults = CEModelDefaults()
    
    var meshFillMode: MTLTriangleFillMode!
    var mesh: CEGameMesh!
    
    private var material: CEMaterial? = nil
    private var baseColorTextureType: TextureTypes = .None
    private var normalMapTextureType: TextureTypes = .None
    
    init(meshType: MeshTypes, meshFillMode: MTLTriangleFillMode = .fill, camera: CECamera) {
        self.meshFillMode = meshFillMode
        mesh = (ConceptEngine.getLibrary(.Mesh) as! CEMeshLibrary).Mesh(meshType)
        self.camera = camera
    }
    
    public override func update() {
        updateModel()
    }
    
    private func updateModel() {
        model.modelMatrix = self.modelMatrix
    }
}

extension CEGameObject {
    func changeMeshFillMode(fillMode: MTLTriangleFillMode) {
        self.meshFillMode = fillMode
    }
    
    public func setBaseColorTexture(_ textureType: TextureTypes) {
        self.baseColorTextureType = textureType
    }
    
    public func setNormalMapTexture(_ textureType: TextureTypes) {
        self.normalMapTextureType = textureType
    }
    
    public func setMaterial(_ material: CEMaterial) {
        self.material = material
    }
}

extension CEGameObject: CERenderable {
    func doRender(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setRenderPipelineState((ConceptEngine.getLibrary(.RenderPipelineState) as! CERenderPipelineStateLibrary).PipelineState(.Basic))
        
        renderCommandEncoder.setDepthStencilState((ConceptEngine.getLibrary(.DepthStencilState) as! CEDepthStencilStateLibrary).DepthStencilState(.Less))
        
        renderCommandEncoder.setVertexBytes(&model, length: CEModelDefaults.stride, index: 2)
        
        renderCommandEncoder.setTriangleFillMode(meshFillMode)
        
        mesh.drawPrimitives(renderCommandEncoder, material: material, baseColorTextureType: baseColorTextureType, normalMapTextureType: normalMapTextureType)
    }
}

