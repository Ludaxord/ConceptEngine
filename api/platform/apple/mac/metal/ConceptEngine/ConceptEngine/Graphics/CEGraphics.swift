//
//  CEGraphics.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGraphics {
    
    init(device: MTLDevice) {
        instanceLibraries(device: device)
    }
    
    private var FragmentShaderLibrary: CEFragmentShaderLibrary!
    private var VertexShaderLibrary: CEVertexShaderLibrary!
    private var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    private var DepthStencilStateLibrary: CEDepthStencilStateLibrary!
    private var RenderPipelineDescriptorLibrary: CERenderPipelineDescriptorLibrary!
    private var RenderPipelineStateLibrary: CERenderPipelineStateLibrary!
    private var MeshLibrary: CEMeshLibrary!
    private var UtilitiesLibrary: CEUtilitiesLibrary!
    
    public var Libraries: [LibraryTypes: CEStandardLibrary] = [:]
    
    private func instanceLibraries(device: MTLDevice) {
        print("gpu: \(device)")
        self.UtilitiesLibrary = CEUtilitiesLibrary()
        Libraries.updateValue(UtilitiesLibrary, forKey: .Utilities)
        
        self.FragmentShaderLibrary = CEFragmentShaderLibrary()
        Libraries.updateValue(FragmentShaderLibrary, forKey: .FragmentShader)
        
        self.VertexShaderLibrary = CEVertexShaderLibrary()
        Libraries.updateValue(VertexShaderLibrary, forKey: .VertexShader)
        
        self.VertexDescriptorLibrary = CEVertexDescriptorLibrary()
        Libraries.updateValue(VertexDescriptorLibrary, forKey: .VertexDescriptor)
        
        self.DepthStencilStateLibrary = CEDepthStencilStateLibrary()
        Libraries.updateValue(DepthStencilStateLibrary, forKey: .DepthStencilState)
        
        self.RenderPipelineDescriptorLibrary = CERenderPipelineDescriptorLibrary(fragmentShaderLibrary: self.FragmentShaderLibrary, vertexShaderLibrary: self.VertexShaderLibrary, vertexDescriptorLibrary: self.VertexDescriptorLibrary)
        Libraries.updateValue(RenderPipelineDescriptorLibrary, forKey: .RenderPipelineDescriptor)
        
        self.RenderPipelineStateLibrary = CERenderPipelineStateLibrary(device: device, renderPipelineDescriptorLibrary: self.RenderPipelineDescriptorLibrary)
        Libraries.updateValue(RenderPipelineStateLibrary, forKey: .RenderPipelineState)
        
        self.MeshLibrary = CEMeshLibrary()
        Libraries.updateValue(MeshLibrary, forKey: .Mesh)
    }
    
}

