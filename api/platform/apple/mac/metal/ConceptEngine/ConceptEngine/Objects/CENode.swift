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
    
    init(name: String? = nil) {
        self.nodeName = name ?? getDefaultNodeName()
        self.id = UUID().uuidString
    }
    
    var position: float3 = float3(0, 0, 0)
    var scale: float3 = float3(1, 1, 1)
    var rotation: float3 = float3(0, 0, 0)
    
    public var nodeName: String = ""
    var id: String!
    
    var nodeChildren: [CENode] = []
    
    var parentModelMatrix = matrix_identity_float4x4
    
    private var _modelMatrix = matrix_identity_float4x4
     var modelMatrix: matrix_float4x4{
         return matrix_multiply(parentModelMatrix, _modelMatrix)
     }
    
    func updateModelMatrix() {
        _modelMatrix = matrix_identity_float4x4
        _modelMatrix.translate(direction: position)
        _modelMatrix.rotate(angle: rotation.x, axis: X_AXIS)
        _modelMatrix.rotate(angle: rotation.y, axis: Y_AXIS)
        _modelMatrix.rotate(angle: rotation.z, axis: Z_AXIS)
        _modelMatrix.scale(axis: scale)
    }
    
    // Override these when needed
    func afterTranslation() { }
    func afterRotation() { }
    func afterScale() { }

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
    func setPosition(_ position: float3){
        self.position = position
        updateModelMatrix()
        afterTranslation()
    }
    func setPosition(_ x: Float,_ y: Float,_ z: Float) { setPosition(float3(x,y,z)) }
    func setPositionX(_ xPosition: Float) { setPosition(xPosition, getPositionY(), getPositionZ()) }
    func setPositionY(_ yPosition: Float) { setPosition(getPositionX(), yPosition, getPositionZ()) }
    func setPositionZ(_ zPosition: Float) { setPosition(getPositionX(), getPositionY(), zPosition) }
    func move(_ x: Float, _ y: Float, _ z: Float){ setPosition(getPositionX() + x, getPositionY() + y, getPositionZ() + z) }
    func moveX(_ delta: Float){ move(delta, 0, 0) }
    func moveY(_ delta: Float){ move(0, delta, 0) }
    func moveZ(_ delta: Float){ move(0, 0, delta) }
    func getPosition()->float3 { return self.position }
    func getPositionX()->Float { return self.position.x }
    func getPositionY()->Float { return self.position.y }
    func getPositionZ()->Float { return self.position.z }
    
    //Rotating
    func setRotation(_ rotation: float3) {
        self.rotation = rotation
        updateModelMatrix()
        afterRotation()
    }
    func setRotation(_ x: Float,_ y: Float,_ z: Float) { setRotation(float3(x,y,z)) }
    func setRotationX(_ xRotation: Float) { setRotation(xRotation, getRotationY(), getRotationZ()) }
    func setRotationY(_ yRotation: Float) { setRotation(getRotationX(), yRotation, getRotationZ()) }
    func setRotationZ(_ zRotation: Float) { setRotation(getRotationX(), getRotationY(), zRotation) }
    func rotate(_ x: Float, _ y: Float, _ z: Float){ setRotation(getRotationX() + x, getRotationY() + y, getRotationZ() + z)}
    func rotateX(_ delta: Float){ rotate(delta, 0, 0) }
    func rotateY(_ delta: Float){ rotate(0, delta, 0) }
    func rotateZ(_ delta: Float){ rotate(0, 0, delta) }
    func getRotation()->float3 { return self.rotation }
    func getRotationX()->Float { return self.rotation.x }
    func getRotationY()->Float { return self.rotation.y }
    func getRotationZ()->Float { return self.rotation.z }
    
    //Scaling
    func setScale(_ scale: float3){
        self.scale = scale
        updateModelMatrix()
        afterScale()
    }
    func setScale(_ x: Float,_ y: Float,_ z: Float) { setScale(float3(x,y,z)) }
    func setScale(_ scale: Float){ setScale(float3(scale, scale, scale)) }
    func setScaleX(_ scaleX: Float){ setScale(scaleX, getScaleY(), getScaleZ()) }
    func setScaleY(_ scaleY: Float){ setScale(getScaleX(), scaleY, getScaleZ()) }
    func setScaleZ(_ scaleZ: Float){ setScale(getScaleX(), getScaleY(), scaleZ) }
    func scale(_ x: Float, _ y: Float, _ z: Float) { setScale(getScaleX() + x, getScaleY() + y, getScaleZ() + z)}
    func scaleX(_ delta: Float){ scale(delta,0,0) }
    func scaleY(_ delta: Float){ scale(0,delta,0) }
    func scaleZ(_ delta: Float){ scale(0,0,delta) }
    func getScale()->float3 { return self.scale }
    func getScaleX()->Float { return self.scale.x }
    func getScaleY()->Float { return self.scale.y }
    func getScaleZ()->Float { return self.scale.z }
}
