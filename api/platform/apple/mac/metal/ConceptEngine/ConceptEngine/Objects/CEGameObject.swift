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
    var mesh: CEMesh!
    
    
    init(meshType: MeshTypes, meshFillMode: MTLTriangleFillMode = .fill, camera: CECamera) {
        self.meshFillMode = meshFillMode
        mesh = (ConceptEngine.getLibrary(.Mesh) as! CEMeshLibrary).Mesh(meshType)
        self.camera = camera
    }
    
    public override func update(deltaTime: Float) {
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
}

extension CEGameObject: CERenderable {
    func doRender(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setVertexBytes(&model, length: CEModelDefaults.stride, index: 2)
        renderCommandEncoder.setTriangleFillMode(meshFillMode)
        renderCommandEncoder.setRenderPipelineState((ConceptEngine.getLibrary(.RenderPipelineState) as! CERenderPipelineStateLibrary).PipelineState(.Basic))
        renderCommandEncoder.setVertexBuffer(mesh.vertexBuffer, offset: 0, index: 0)
        renderCommandEncoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: mesh.vertexCount)
    }
}
