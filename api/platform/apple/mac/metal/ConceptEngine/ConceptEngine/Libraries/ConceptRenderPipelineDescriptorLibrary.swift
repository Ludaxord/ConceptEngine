//
//  ConceptRenderPipelineDescriptorLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum RenderPipelineDescriptorTypes {
    case Basic
}

protocol ConceptRenderPipelineDescriptor {
    var renderName: String { get }
    var renderPipelineDescriptor: MTLRenderPipelineDescriptor { get }
}

public struct BasicRenderPipelineDescriptor: ConceptRenderPipelineDescriptor {
    
    private var ShaderLibrary: ConceptShaderLibrary!
    private var VertexDescriptorLibrary: ConceptVertexDescriptorLibrary!
    
    init(shaderLibrary: ConceptShaderLibrary, vertexDescriptorLibrary: ConceptVertexDescriptorLibrary) {
        self.ShaderLibrary = shaderLibrary
        self.VertexDescriptorLibrary = vertexDescriptorLibrary
    }
    
    var renderName: String = "Basic Render Pipeline Descriptor"
    
    var renderPipelineDescriptor: MTLRenderPipelineDescriptor {
        return ConceptRenderPipelineDescriptorLibrary.createRenderDescriptor(.Basic, .Basic, .Basic)
    }
    
    
}

public class ConceptRenderPipelineDescriptorLibrary {
    private var renderPipelineDescriptors: [RenderPipelineDescriptorTypes: ConceptRenderPipelineDescriptor] = [:]
    
    private static var ShaderLibrary: ConceptShaderLibrary!
    private static var VertexDescriptorLibrary: ConceptVertexDescriptorLibrary!
    
    required init(shaderLibrary: ConceptShaderLibrary, vertexDescriptorLibrary: ConceptVertexDescriptorLibrary) {
        ConceptRenderPipelineDescriptorLibrary.ShaderLibrary = shaderLibrary
        ConceptRenderPipelineDescriptorLibrary.VertexDescriptorLibrary = vertexDescriptorLibrary
        createDefaultRenderPipelineDescriptor()
    }
    
    public static func createRenderDescriptor(_ vertexType: VertexShaderTypes, _ fragmentType: FragmentShaderTypes, _ descriptorType: VertexDescriptorTypes) -> MTLRenderPipelineDescriptor {
        let renderPipelineStateDescriptor = MTLRenderPipelineDescriptor()
        renderPipelineStateDescriptor.colorAttachments[0].pixelFormat = Utilities.PixelFormat
        renderPipelineStateDescriptor.vertexFunction = ShaderLibrary.Vertex(vertexType)
        renderPipelineStateDescriptor.fragmentFunction = ShaderLibrary.Fragment(fragmentType)
        renderPipelineStateDescriptor.vertexDescriptor = VertexDescriptorLibrary.Descriptor(descriptorType)
        return renderPipelineStateDescriptor
    }
    
    public func RenderDescriptor(_ renderPipelineDescriptorType :RenderPipelineDescriptorTypes) -> MTLRenderPipelineDescriptor {
        return renderPipelineDescriptors[renderPipelineDescriptorType]!.renderPipelineDescriptor
    }
    
    private func createDefaultRenderPipelineDescriptor(shaderLibrary: ConceptShaderLibrary? = nil, vertexDescriptorLibrary: ConceptVertexDescriptorLibrary? = nil) {
        renderPipelineDescriptors.updateValue(BasicRenderPipelineDescriptor(shaderLibrary: shaderLibrary ?? ConceptRenderPipelineDescriptorLibrary.ShaderLibrary, vertexDescriptorLibrary: vertexDescriptorLibrary ?? ConceptRenderPipelineDescriptorLibrary.VertexDescriptorLibrary), forKey: .Basic)
    }
}
