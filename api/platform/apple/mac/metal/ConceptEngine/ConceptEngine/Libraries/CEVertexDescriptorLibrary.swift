//
//  ConceptVertexDescriptorLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum VertexDescriptorTypes {
    case Basic
}

protocol CEVertexDescriptor {
    var vertexName: String { get }
    var vertexDescriptor: MTLVertexDescriptor! { get }
}

public struct BasicVertexDescriptor: CEVertexDescriptor {
    var vertexName: String = "Basic Vertex Descriptor"
    
    var vertexDescriptor: MTLVertexDescriptor!
    init(){
        vertexDescriptor = CEVertexDescriptorLibrary.createVertexDescriptor()
    }
}

public final class CEVertexDescriptorLibrary: CEStandardLibrary {
    private var vertexDescriptors: [VertexDescriptorTypes: CEVertexDescriptor] = [:]
    
    required init() {
        createDefaultVertexDescriptors()
    }
    
    public static func createVertexDescriptor() -> MTLVertexDescriptor {
        let vertexDescriptor = MTLVertexDescriptor()
        vertexDescriptor.attributes[0].format = .float3
        vertexDescriptor.attributes[0].bufferIndex = 0
        vertexDescriptor.attributes[0].offset = 0
        
        vertexDescriptor.attributes[1].format = .float4
        vertexDescriptor.attributes[1].bufferIndex = 0
        vertexDescriptor.attributes[1].offset = float3.size
        
        vertexDescriptor.layouts[0].stride = CEVertex.stride
        return vertexDescriptor
    }
    
    public func Descriptor(_ vertexDescriptorType: VertexDescriptorTypes) -> MTLVertexDescriptor {
        return vertexDescriptors[vertexDescriptorType]!.vertexDescriptor
    }
     
    private func createDefaultVertexDescriptors() {
        vertexDescriptors.updateValue(BasicVertexDescriptor(), forKey: .Basic)
    }
    
}
