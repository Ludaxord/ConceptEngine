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
    var id: String!
    
    var nodeChildren: [CENode] = []
    
    var parentModelMatrix = matrix_identity_float4x4
    
    var modelMatrix: matrix_float4x4 {
        var modelMatrix = matrix_identity_float4x4
        modelMatrix.translate(direction: position)
        modelMatrix.rotate(angle: rotation.x, axis: X_AXIS)
        modelMatrix.rotate(angle: rotation.y, axis: Y_AXIS)
        modelMatrix.rotate(angle: rotation.z, axis: Z_AXIS)
        modelMatrix.scale(axis: scale)
        return matrix_multiply(parentModelMatrix, modelMatrix)
    }
    
    init(name: String? = nil) {
        self.nodeName = name ?? getDefaultNodeName()
        self.id = UUID().uuidString
    }
    
    public func getDefaultNodeName() -> String {
        return String(describing: self)
    }
    
    public func addNodeChild(_ nodeChild: CENode) {
        nodeChildren.append(nodeChild)
    }
    
    public func addNodeChildren(_ nodeChildrenCollection: [CENode]) {
        nodeChildren.append(contentsOf: nodeChildrenCollection)
    }
    
    public func update() {
        for child in nodeChildren {
            child.update()
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

extension CENode {
    //Naming
    func setName(_ name: String){ self.nodeName = name }
    func getName()->String{ return nodeName }
    func getID()->String { return id }
    
    //Positioning and Movement
    func setPosition(_ position: float3){ self.position = position }
    func setPositionX(_ xPosition: Float) { self.position.x = xPosition }
    func setPositionY(_ yPosition: Float) { self.position.y = yPosition }
    func setPositionZ(_ zPosition: Float) { self.position.z = zPosition }
    func getPosition()->float3 { return self.position }
    func getPositionX()->Float { return self.position.x }
    func getPositionY()->Float { return self.position.y }
    func getPositionZ()->Float { return self.position.z }
    func move(_ x: Float, _ y: Float, _ z: Float){ self.position += float3(x,y,z) }
    func moveX(_ delta: Float){ self.position.x += delta }
    func moveY(_ delta: Float){ self.position.y += delta }
    func moveZ(_ delta: Float){ self.position.z += delta }
    
    //Rotating
    func setRotation(_ rotation: float3) { self.rotation = rotation }
    func setRotationX(_ xRotation: Float) { self.rotation.x = xRotation }
    func setRotationY(_ yRotation: Float) { self.rotation.y = yRotation }
    func setRotationZ(_ zRotation: Float) { self.rotation.z = zRotation }
    func getRotation()->float3 { return self.rotation }
    func getRotationX()->Float { return self.rotation.x }
    func getRotationY()->Float { return self.rotation.y }
    func getRotationZ()->Float { return self.rotation.z }
    func rotate(_ x: Float, _ y: Float, _ z: Float){ self.rotation += float3(x,y,z) }
    func rotateX(_ delta: Float){ self.rotation.x += delta }
    func rotateY(_ delta: Float){ self.rotation.y += delta }
    func rotateZ(_ delta: Float){ self.rotation.z += delta }
    
    //Scaling
    func setScale(_ scale: float3){ self.scale = scale }
    func setScale(_ scale: Float){setScale(float3(scale, scale, scale))}
    func setScaleX(_ scaleX: Float){ self.scale.x = scaleX }
    func setScaleY(_ scaleY: Float){ self.scale.y = scaleY }
    func setScaleZ(_ scaleZ: Float){ self.scale.z = scaleZ }
    func getScale()->float3 { return self.scale }
    func getScaleX()->Float { return self.scale.x }
    func getScaleY()->Float { return self.scale.y }
    func getScaleZ()->Float { return self.scale.z }
    func scaleX(_ delta: Float){ self.scale.x += delta }
    func scaleY(_ delta: Float){ self.scale.y += delta }
    func scaleZ(_ delta: Float){ self.scale.z += delta }
}
