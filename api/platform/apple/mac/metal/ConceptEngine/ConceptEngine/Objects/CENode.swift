//
//  CENode.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public protocol PCENode {
    func getDefaultNodeName() -> String
}

public class CENode: PCENode {
    
    var position: float3 = float3(0, 0, 0)
    var scale: float3 = float3(1, 1, 1)
    var rotation: float3 = float3(0, 0, 0)
    
    public var nodeName: String = ""
    
    var nodeChildren: [CENode] = []
    
    var modelMatrix: matrix_float4x4 {
        var modelMatrix = matrix_identity_float4x4
        modelMatrix.translate(direction: position)
        modelMatrix.rotate(angle: rotation.x, axis: X_AXIS)
        modelMatrix.rotate(angle: rotation.y, axis: Y_AXIS)
        modelMatrix.rotate(angle: rotation.z, axis: Z_AXIS)
        modelMatrix.scale(axis: scale)
        return modelMatrix
    }
    
    init(name: String? = nil) {
        self.nodeName = name ?? getDefaultNodeName()
    }
    
    public func getDefaultNodeName() -> String {
        return String(describing: self)
    }
    
    public func addNodeChild(_ nodeChild: CENode) {
        nodeChildren.append(nodeChild)
    }
    
    public func update(deltaTime: Float) {
        for child in nodeChildren {
            child.update(deltaTime: deltaTime)
        }
    }
    
    public func render(renderCommandEncoder: MTLRenderCommandEncoder) {
        renderCommandEncoder.pushDebugGroup("Rendering \(self.nodeName)")
        for nodeChild in nodeChildren {
            nodeChild.render(renderCommandEncoder: renderCommandEncoder)
        }
        if let renderable = self as? CERenderable {
            renderable.doRender(renderCommandEncoder)
        }
    }
    
}
