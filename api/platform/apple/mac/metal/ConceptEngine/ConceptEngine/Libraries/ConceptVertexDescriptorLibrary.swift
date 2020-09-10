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

protocol ConceptVertexDescriptor {
    var vertexName: String { get }
    var vertexDescriptor: MTLVertexDescriptor { get }
}

public struct BasicVertexDescriptor: ConceptVertexDescriptor {
    var vertexName: String = "Basic Vertex Descriptor"
    
    var vertexDescriptor: MTLVertexDescriptor {
        let vertexDescriptor = MTLVertexDescriptor()
        vertexDescriptor.attributes[0].format = .float3
        vertexDescriptor.attributes[0].bufferIndex = 0
        vertexDescriptor.attributes[0].offset = 0
        
        vertexDescriptor.attributes[1].format = .float4
        vertexDescriptor.attributes[1].bufferIndex = 0
        vertexDescriptor.attributes[1].offset = float3.size
        
        vertexDescriptor.layouts[0].stride = Vertex.stride
        
        return vertexDescriptor
    }
}

public class ConceptVertexDescriptorLibrary {
    private var vertexDescriptors: [VertexDescriptorTypes: ConceptVertexDescriptor] = [:]
    
    required init() {
        createDefaultVertexDescriptors()
    }
    
    public func Descriptor(_ vertexDescriptorType: VertexDescriptorTypes) -> MTLVertexDescriptor {
        return vertexDescriptors[vertexDescriptorType]!.vertexDescriptor
    }
     
    private func createDefaultVertexDescriptors() {
        vertexDescriptors.updateValue(BasicVertexDescriptor(), forKey: .Basic)
    }
    
}
