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
    private var material = CEMaterial()
    private var _textureType: TextureTypes = .None
    
    var model: CEModelDefaults = CEModelDefaults()
    
    var meshFillMode: MTLTriangleFillMode!
    var mesh: CEMesh!
    
    
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
    
    public func setColor(_ color: float4) {
        self.material.color = color
        self.material.useMaterialColor = true
        self.material.useTexture = false
        self.material.useDefaultTrigonometricTexture = false
    }
    
    public func setTexture(_ textureType: TextureTypes) {
        self._textureType = textureType
        self.material.useMaterialColor = false
        self.material.useTexture = true
        self.material.useDefaultTrigonometricTexture = false
    }
    
    public func setDefaultTrigonometricTexture() {
        self.material.useMaterialColor = false
        self.material.useTexture = false
        self.material.useDefaultTrigonometricTexture = true
    }
}

extension CEGameObject: CERenderable {
    func doRender(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setRenderPipelineState((ConceptEngine.getLibrary(.RenderPipelineState) as! CERenderPipelineStateLibrary).PipelineState(.Basic))
        renderCommandEncoder.setDepthStencilState((ConceptEngine.getLibrary(.DepthStencilState) as! CEDepthStencilStateLibrary).DepthStencilState(.Less))
        renderCommandEncoder.setFragmentSamplerState((ConceptEngine.getLibrary(.SamplerState) as! CESamplerStateLibrary).SamplerState(.Linear), index: 0)
        
        if material.useTexture {
            //Test Car texture
            renderCommandEncoder.setFragmentTexture((ConceptEngine.getLibrary(.Texture) as! CETextureLibrary).Texture(_textureType), index: 0)
        }
        
        renderCommandEncoder.setVertexBytes(&model, length: CEModelDefaults.stride, index: 2)
        renderCommandEncoder.setTriangleFillMode(meshFillMode)
        renderCommandEncoder.setFragmentBytes(&material, length: CEMaterial.stride, index: 1)
        mesh.drawPrimitives(renderCommandEncoder)
    }
}

