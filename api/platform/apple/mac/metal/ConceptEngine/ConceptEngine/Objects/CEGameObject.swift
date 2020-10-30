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
    
    private var material = CEMaterial()
    private var baseColorTextureType: TextureTypes = .None
    
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
    
    public func setMaterialColor(_ color: float4) {
        self.material.color = color
        self.material.useDefaultTrigonometricTexture = false
    }
    
    public func setMaterialColor(_ r: Float,_ g: Float,_ b: Float,_ a: Float) {
        setMaterialColor(float4(r,g,b,a))
    }
    
    public func setTexture(_ textureType: TextureTypes) {
        self.baseColorTextureType = textureType
        self.material.useDefaultTrigonometricTexture = false
    }
    
    public func setDefaultTrigonometricTexture() {
        self.material.useDefaultTrigonometricTexture = true
    }
    
    public func setMaterialIsIlluminated(_ isIlluminated: Bool) { self.material.isIlluminated = isIlluminated }
    public func getMaterialIsIlluminated() -> Bool { return self.material.isIlluminated }
    
    public func setMaterialAmbient(_ ambient: float3) { self.material.ambient = ambient }
    public func setMaterialAmbient(_ r: Float,_ g: Float,_ b: Float) { setMaterialAmbient(float3(r,g,b)) }
    public func setMaterialAmbient(_ ambient: Float) { self.material.ambient = float3(ambient, ambient, ambient) }
    public func addMaterialAmbient(_ value: Float) { self.material.ambient += value }
    public func getMaterialAmbient() -> float3 { return self.material.ambient }
    
    public func setMaterialDiffuse(_ diffuse: float3) { self.material.diffuse = diffuse }
    public func setMaterialDiffuse(_ r: Float,_ g: Float,_ b: Float) { setMaterialDiffuse(float3(r,g,b)) }
    public func setMaterialDiffuse(_ diffuse: Float) { self.material.diffuse = float3(diffuse, diffuse, diffuse) }
    public func addMaterialDiffuse(_ value: Float) { self.material.diffuse += value }
    public func getMaterialDiffuse()->float3 { return self.material.diffuse }
    
    public func setMaterialSpecular(_ specular: float3) { self.material.specular = specular }
    public func setMaterialSpecular(_ r: Float,_ g: Float,_ b: Float) { setMaterialSpecular(float3(r,g,b)) }
    public func setMaterialSpecular(_ specular: Float) { self.material.specular = float3(specular, specular, specular) }
    public func addMaterialSpecular(_ value: Float) { self.material.specular += value }
    public func getMaterialSpecular()->float3 { return self.material.specular }
    
    public func setMaterialShininess(_ shininess: Float) { self.material.shininess = shininess }
    public func getShininess()->Float { return self.material.shininess }
}

extension CEGameObject: CERenderable {
    func doRender(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setRenderPipelineState((ConceptEngine.getLibrary(.RenderPipelineState) as! CERenderPipelineStateLibrary).PipelineState(.Basic))
        
        renderCommandEncoder.setDepthStencilState((ConceptEngine.getLibrary(.DepthStencilState) as! CEDepthStencilStateLibrary).DepthStencilState(.Less))
        
        //Vertex Shader
        renderCommandEncoder.setVertexBytes(&model, length: CEModelDefaults.stride, index: 2)
        
        //Fragment Shader
//        renderCommandEncoder.setFragmentSamplerState((ConceptEngine.getLibrary(.SamplerState) as! CESamplerStateLibrary).SamplerState(.Linear), index: 0)
        renderCommandEncoder.setFragmentBytes(&material, length: CEMaterial.stride, index: 1)
        
        renderCommandEncoder.setTriangleFillMode(meshFillMode)
        
//        if material.useTexture {
//            //Test Car texture
//            renderCommandEncoder.setFragmentTexture((ConceptEngine.getLibrary(.Texture) as! CETextureLibrary).Texture(_textureType), index: 0)
//        }
        mesh.drawPrimitives(renderCommandEncoder, baseColorTextureType: baseColorTextureType)
    }
}

