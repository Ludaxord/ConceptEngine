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
    
    private var ShaderLibrary: CEShaderLibrary!
    private var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    
    init(shaderLibrary: CEShaderLibrary, vertexDescriptorLibrary: CEVertexDescriptorLibrary) {
        self.ShaderLibrary = shaderLibrary
        self.VertexDescriptorLibrary = vertexDescriptorLibrary
        renderPipelineDescriptor = CERenderPipelineDescriptorLibrary.createRenderDescriptor(.BasicVertex, .BasicFragment, .Basic)
    }
    
    var renderName: String = "Basic Render Pipeline Descriptor"
    
    var renderPipelineDescriptor: MTLRenderPipelineDescriptor!
    
}

public struct InstancedRenderPipelineDescriptor: CEPipelineDescriptor {
    
    private var ShaderLibrary: CEShaderLibrary!
    private var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    
    init(shaderLibrary: CEShaderLibrary, vertexDescriptorLibrary: CEVertexDescriptorLibrary) {
        self.ShaderLibrary = shaderLibrary
        self.VertexDescriptorLibrary = vertexDescriptorLibrary
        renderPipelineDescriptor = CERenderPipelineDescriptorLibrary.createRenderDescriptor(.InstancedVertex, .BasicFragment, .Basic)
    }
    
    var renderName: String = "Instanced Render Pipeline Descriptor"
    
    var renderPipelineDescriptor: MTLRenderPipelineDescriptor!
    
}

public final class CERenderPipelineDescriptorLibrary: CELibrary<RenderPipelineDescriptorTypes, MTLRenderPipelineDescriptor>, CEStandardLibrary {
    private var renderPipelineDescriptors: [RenderPipelineDescriptorTypes: CEPipelineDescriptor] = [:]
    
    private static var ShaderLibrary: CEShaderLibrary!
    private static var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    
    override func useLibrary() {
        createDefaultRenderPipelineDescriptor()
    }
    
    override subscript(type: RenderPipelineDescriptorTypes) -> MTLRenderPipelineDescriptor? {
        return RenderDescriptor(type)
    }
    
    required init(shaderLibrary: CEShaderLibrary, vertexDescriptorLibrary: CEVertexDescriptorLibrary) {
        CERenderPipelineDescriptorLibrary.ShaderLibrary = shaderLibrary
        CERenderPipelineDescriptorLibrary.VertexDescriptorLibrary = vertexDescriptorLibrary
        super.init()
    }
    
    public static func createRenderDescriptor(_ vertexType: ShaderTypes, _ fragmentType: ShaderTypes, _ descriptorType: VertexDescriptorTypes) -> MTLRenderPipelineDescriptor {
        let renderPipelineStateDescriptor = MTLRenderPipelineDescriptor()
        renderPipelineStateDescriptor.colorAttachments[0].pixelFormat = CEUtilitiesLibrary.PixelFormat
        renderPipelineStateDescriptor.depthAttachmentPixelFormat = CEUtilitiesLibrary.DepthPixelFormat
        renderPipelineStateDescriptor.vertexFunction = ShaderLibrary.Shader(vertexType)
        renderPipelineStateDescriptor.fragmentFunction = ShaderLibrary.Shader(fragmentType)
        renderPipelineStateDescriptor.vertexDescriptor = VertexDescriptorLibrary.Descriptor(descriptorType)
        return renderPipelineStateDescriptor
    }
    
    public func RenderDescriptor(_ renderPipelineDescriptorType :RenderPipelineDescriptorTypes) -> MTLRenderPipelineDescriptor {
        return renderPipelineDescriptors[renderPipelineDescriptorType]!.renderPipelineDescriptor
    }
    
    private func createDefaultRenderPipelineDescriptor(shaderLibrary: CEShaderLibrary? = nil, vertexDescriptorLibrary: CEVertexDescriptorLibrary? = nil) {
        renderPipelineDescriptors.updateValue(BasicRenderPipelineDescriptor(shaderLibrary: shaderLibrary ?? CERenderPipelineDescriptorLibrary.ShaderLibrary, vertexDescriptorLibrary: vertexDescriptorLibrary ?? CERenderPipelineDescriptorLibrary.VertexDescriptorLibrary), forKey: .Basic)
        
        renderPipelineDescriptors.updateValue(InstancedRenderPipelineDescriptor(shaderLibrary: shaderLibrary ?? CERenderPipelineDescriptorLibrary.ShaderLibrary, vertexDescriptorLibrary: vertexDescriptorLibrary ?? CERenderPipelineDescriptorLibrary.VertexDescriptorLibrary), forKey: .Instanced)
    }
}
