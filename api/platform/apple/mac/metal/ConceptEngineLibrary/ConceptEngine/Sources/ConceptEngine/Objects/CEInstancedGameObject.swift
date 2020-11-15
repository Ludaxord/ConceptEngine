//
//  CEInstancedGameObject.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/11/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEInstancedGameObject : CENode {
    private var _material = CEMaterial()
    private var _mesh: CEGameMesh!
    
    internal var _nodes: [CENode] = []
    private var _modelConstantBuffer: MTLBuffer!
    
    init(meshType: MeshTypes, instanceCount: Int) {
        super.init(name: "Instanced Game Object")
        self._mesh = (ConceptEngine.getLibrary(.Mesh) as! CEMeshLibrary).Mesh(meshType)
        self._mesh.setInstanceCount(instanceCount)
        self.generateInstances(instanceCount)
        self.createBuffers(instanceCount)
    }
    
    func updateNodes(_ updateNodeFunction: (CENode, Int)->()) {
        for (index, node) in _nodes.enumerated() {
            updateNodeFunction(node, index)
        }
    }
    
    func generateInstances(_ instanceCount: Int){
        for _ in 0..<instanceCount {
            _nodes.append(CENode(name: "\(getName())_InstancedNode"))
        }
    }
    
    func createBuffers(_ instanceCount: Int) {
        _modelConstantBuffer = ConceptEngine.GPUDevice.makeBuffer(length: CEModelDefaults.stride(instanceCount), options: [])
    }
    
    override func update() {
        var pointer = _modelConstantBuffer.contents().bindMemory(to: CEModelDefaults.self, capacity: _nodes.count)
        for node in _nodes {
            pointer.pointee.modelMatrix = matrix_multiply(self.modelMatrix, node.modelMatrix)
            pointer = pointer.advanced(by: 1)
        }
        
        super.update()
    }
}

extension CEInstancedGameObject: CERenderable {
    func doRender(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.setRenderPipelineState((ConceptEngine.getLibrary(.RenderPipelineState) as! CERenderPipelineStateLibrary).PipelineState(.Instanced))
        renderCommandEncoder.setDepthStencilState((ConceptEngine.getLibrary(.DepthStencilState) as! CEDepthStencilStateLibrary).DepthStencilState(.Less))
        
        //Vertex Shader
        renderCommandEncoder.setVertexBuffer(_modelConstantBuffer, offset: 0, index: 2)
        
        //Fragment Shader
        renderCommandEncoder.setFragmentBytes(&_material, length: CEMaterial.stride, index: 1)
        
        _mesh.drawPrimitives(renderCommandEncoder)
    }
}

//Material Properties
extension CEInstancedGameObject {
    public func setColor(_ color: float4){
        self._material.color = color
    }
    
    public func setColor(_ r: Float,_ g: Float,_ b: Float,_ a: Float) {
        setColor(float4(r,g,b,a))
    }
}

