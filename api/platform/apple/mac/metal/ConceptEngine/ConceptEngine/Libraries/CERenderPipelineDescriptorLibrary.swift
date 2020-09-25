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
    case Instanced
}

protocol CEPipelineDescriptor {
    var renderName: String { get }
    var renderPipelineDescriptor: MTLRenderPipelineDescriptor! { get }
}

public struct BasicRenderPipelineDescriptor: CEPipelineDescriptor {
    
    private var ShaderLibrary: CEFragmentShaderLibrary!
    private var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    
    init(shaderLibrary: CEFragmentShaderLibrary, vertexDescriptorLibrary: CEVertexDescriptorLibrary) {
        self.ShaderLibrary = shaderLibrary
        self.VertexDescriptorLibrary = vertexDescriptorLibrary
        renderPipelineDescriptor = CERenderPipelineDescriptorLibrary.createRenderDescriptor(.Basic, .Basic, .Basic)
    }
    
    var renderName: String = "Basic Render Pipeline Descriptor"
    
    var renderPipelineDescriptor: MTLRenderPipelineDescriptor!
    
}

public struct InstancedRenderPipelineDescriptor: CEPipelineDescriptor {
    
    private var ShaderLibrary: CEFragmentShaderLibrary!
    private var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    
    init(shaderLibrary: CEFragmentShaderLibrary, vertexDescriptorLibrary: CEVertexDescriptorLibrary) {
        self.ShaderLibrary = shaderLibrary
        self.VertexDescriptorLibrary = vertexDescriptorLibrary
        renderPipelineDescriptor = CERenderPipelineDescriptorLibrary.createRenderDescriptor(.Instanced, .Basic, .Basic)
    }
    
    var renderName: String = "Instanced Render Pipeline Descriptor"
    
    var renderPipelineDescriptor: MTLRenderPipelineDescriptor!
    
}

public final class CERenderPipelineDescriptorLibrary: CELibrary<RenderPipelineDescriptorTypes, MTLRenderPipelineDescriptor>, CEStandardLibrary {
    private var renderPipelineDescriptors: [RenderPipelineDescriptorTypes: CEPipelineDescriptor] = [:]
    
    private static var FragmentShaderLibrary: CEFragmentShaderLibrary!
    private static var VertexShaderLibrary: CEVertexShaderLibrary!
    private static var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    
    override func useLibrary() {
        createDefaultRenderPipelineDescriptor()
    }
    
    override subscript(type: RenderPipelineDescriptorTypes) -> MTLRenderPipelineDescriptor? {
        return RenderDescriptor(type)
    }
    
    required init(fragmentShaderLibrary: CEFragmentShaderLibrary, vertexShaderLibrary: CEVertexShaderLibrary, vertexDescriptorLibrary: CEVertexDescriptorLibrary) {
        CERenderPipelineDescriptorLibrary.FragmentShaderLibrary = fragmentShaderLibrary
        CERenderPipelineDescriptorLibrary.VertexShaderLibrary = vertexShaderLibrary
        CERenderPipelineDescriptorLibrary.VertexDescriptorLibrary = vertexDescriptorLibrary
        super.init()
    }
    
    public static func createRenderDescriptor(_ vertexType: VertexShaderTypes, _ fragmentType: FragmentShaderTypes, _ descriptorType: VertexDescriptorTypes) -> MTLRenderPipelineDescriptor {
        let renderPipelineStateDescriptor = MTLRenderPipelineDescriptor()
        renderPipelineStateDescriptor.colorAttachments[0].pixelFormat = CEUtilitiesLibrary.PixelFormat
        renderPipelineStateDescriptor.depthAttachmentPixelFormat = CEUtilitiesLibrary.DepthPixelFormat
        renderPipelineStateDescriptor.vertexFunction = VertexShaderLibrary.Vertex(vertexType)
        renderPipelineStateDescriptor.fragmentFunction = FragmentShaderLibrary.Fragment(fragmentType)
        renderPipelineStateDescriptor.vertexDescriptor = VertexDescriptorLibrary.Descriptor(descriptorType)
        return renderPipelineStateDescriptor
    }
    
    public func RenderDescriptor(_ renderPipelineDescriptorType :RenderPipelineDescriptorTypes) -> MTLRenderPipelineDescriptor {
        return renderPipelineDescriptors[renderPipelineDescriptorType]!.renderPipelineDescriptor
    }
    
    private func createDefaultRenderPipelineDescriptor(shaderLibrary: CEFragmentShaderLibrary? = nil, vertexDescriptorLibrary: CEVertexDescriptorLibrary? = nil) {
        renderPipelineDescriptors.updateValue(BasicRenderPipelineDescriptor(shaderLibrary: shaderLibrary ?? CERenderPipelineDescriptorLibrary.FragmentShaderLibrary, vertexDescriptorLibrary: vertexDescriptorLibrary ?? CERenderPipelineDescriptorLibrary.VertexDescriptorLibrary), forKey: .Basic)
        
        renderPipelineDescriptors.updateValue(InstancedRenderPipelineDescriptor(shaderLibrary: shaderLibrary ?? CERenderPipelineDescriptorLibrary.FragmentShaderLibrary, vertexDescriptorLibrary: vertexDescriptorLibrary ?? CERenderPipelineDescriptorLibrary.VertexDescriptorLibrary), forKey: .Instanced)
    }
}
