//
//  CGameObject.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit


public class CEGameObject: CENode {
    var meshFillMode: MTLTriangleFillMode!
    var mesh: CEMesh!
    
    init(meshType: MeshTypes, meshFillMode: MTLTriangleFillMode = .fill) {
        self.meshFillMode = meshFillMode
        mesh = (ConceptEngine.getLibrary(.Mesh) as! CEMeshLibrary).Mesh(meshType)
    }
}


extension CEGameObject {
    func changeMeshFillMode(fillMode: MTLTriangleFillMode) {
        self.meshFillMode = fillMode
    }
}

extension CEGameObject: CERenderable {
    func doRender(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setTriangleFillMode(meshFillMode)
        renderCommandEncoder.setRenderPipelineState((ConceptEngine.getLibrary(.RenderPipelineState) as! CERenderPipelineStateLibrary).PipelineState(.Basic))
        renderCommandEncoder.setVertexBuffer(mesh.vertexBuffer, offset: 0, index: 0)
        renderCommandEncoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: mesh.vertexCount)
    }
}
