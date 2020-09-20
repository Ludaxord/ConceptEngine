//
//  CEGameObjectInstances.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 20/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjectInstances : CENode {
    private var material = CEMaterial()
    var meshFillMode: MTLTriangleFillMode!
    
    internal var _nodes: [CENode] = []
    
    private var _models: [CEModelDefaults] = []
    private var _modelConstantBuffer: MTLBuffer!
    private var _mesh: CEMesh!
    
    init(meshType: MeshTypes, instanceCount: Int, meshFillMode: MTLTriangleFillMode = .fill) {
        super.init()
        self.meshFillMode = meshFillMode
        self._mesh = (ConceptEngine.getLibrary(.Mesh) as! CEMeshLibrary).Mesh(meshType)
        self._mesh.setInstanceCount(instanceCount)
        self.generateInstances(instanceCount)
        self.createBuffers(instanceCount)
    }
    
    func generateInstances(_ instancesCount: Int) {
        for _ in 0..<instancesCount {
            _nodes.append(CENode())
            _models.append(CEModelDefaults())
        }
    }
    
    func createBuffers(_ instancesCount: Int) {
        _modelConstantBuffer = ConceptEngine.GPUDevice.makeBuffer(length: CEModelDefaults.stride(instancesCount), options: [])
    }
    
    private func updateModelConstantsBuffer() {
        var pointer = _modelConstantBuffer.contents().bindMemory(to: CEModelDefaults.self, capacity: _models.count)
        for node in _nodes {
            pointer.pointee.modelMatrix = matrix_multiply(node.modelMatrix, self.modelMatrix)
            pointer = pointer.advanced(by: 1)
        }
    }
    
    override func update(deltaTime: Float) {
        updateModelConstantsBuffer()
        super.update(deltaTime: deltaTime)
    }
}

extension CEGameObjectInstances: CERenderable {
    func doRender(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setRenderPipelineState((ConceptEngine.getLibrary(.RenderPipelineState) as! CERenderPipelineStateLibrary).PipelineState(.Instanced))
        renderCommandEncoder.setDepthStencilState((ConceptEngine.getLibrary(.DepthStencilState) as! CEDepthStencilStateLibrary).DepthStencilState(.Less))
        
        renderCommandEncoder.setVertexBuffer(_modelConstantBuffer, offset: 0, index: 2)
        
        renderCommandEncoder.setTriangleFillMode(meshFillMode)
        
        renderCommandEncoder.setFragmentBytes(&material, length: CEMaterial.stride, index: 1)
        
        _mesh.drawPrimitives(renderCommandEncoder)
    }
}

extension CEGameObjectInstances {
    func changeMeshFillMode(fillMode: MTLTriangleFillMode) {
        self.meshFillMode = fillMode
    }
    
    public func setColor(_ color: float4) {
        self.material.color = color
        self.material.useMaterialColor = true
    }
}
