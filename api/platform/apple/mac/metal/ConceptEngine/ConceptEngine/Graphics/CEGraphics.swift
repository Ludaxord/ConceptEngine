//
//  CEGraphics.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGraphics {
    
    init(staticLibraries: [LibraryTypes: CEStandardLibrary]) {
        Libraries = staticLibraries
        
    }
    
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
    private var TextureLibrary: CETextureLibrary!
    private var SamplerStateLibrary: CESamplerStateLibrary!
    private var UtilitiesLibrary: CEUtilitiesLibrary!
    
    public var Libraries: [LibraryTypes: CEStandardLibrary] = [:]
    
    private func fillLibraries(staticLibraries: [LibraryTypes: CEStandardLibrary]) {
        Libraries = staticLibraries
        CEGraphics.MainLibraries = staticLibraries
    }
    
    private func instanceLibraries(device: MTLDevice) {
        print("gpu: \(device)")
        self.UtilitiesLibrary = CEUtilitiesLibrary()
        Libraries.updateValue(UtilitiesLibrary, forKey: .Utilities)
        print("UtilitiesLibrary")
        
        self.FragmentShaderLibrary = CEFragmentShaderLibrary()
        Libraries.updateValue(FragmentShaderLibrary, forKey: .FragmentShader)
        print("FragmentShaderLibrary")

        self.VertexShaderLibrary = CEVertexShaderLibrary()
        Libraries.updateValue(VertexShaderLibrary, forKey: .VertexShader)
        print("VertexShaderLibrary")

        self.VertexDescriptorLibrary = CEVertexDescriptorLibrary()
        Libraries.updateValue(VertexDescriptorLibrary, forKey: .VertexDescriptor)
        print("VertexDescriptorLibrary")

        self.DepthStencilStateLibrary = CEDepthStencilStateLibrary()
        Libraries.updateValue(DepthStencilStateLibrary, forKey: .DepthStencilState)
        print("DepthStencilStateLibrary")

        self.RenderPipelineDescriptorLibrary = CERenderPipelineDescriptorLibrary(fragmentShaderLibrary: self.FragmentShaderLibrary, vertexShaderLibrary: self.VertexShaderLibrary, vertexDescriptorLibrary: self.VertexDescriptorLibrary)
        Libraries.updateValue(RenderPipelineDescriptorLibrary, forKey: .RenderPipelineDescriptor)
        print("RenderPipelineDescriptorLibrary")

        self.RenderPipelineStateLibrary = CERenderPipelineStateLibrary(device: device, renderPipelineDescriptorLibrary: self.RenderPipelineDescriptorLibrary)
        Libraries.updateValue(RenderPipelineStateLibrary, forKey: .RenderPipelineState)
        print("RenderPipelineStateLibrary")

        self.MeshLibrary = CEMeshLibrary()
        Libraries.updateValue(MeshLibrary, forKey: .Mesh)
        print("MeshLibrary")

        self.TextureLibrary = CETextureLibrary()
        Libraries.updateValue(TextureLibrary, forKey: .Texture)
        print("TextureLibrary")

        self.SamplerStateLibrary = CESamplerStateLibrary()
        Libraries.updateValue(SamplerStateLibrary, forKey: .SamplerState)
        print("SamplerStateLibrary")
    }
    
}

extension CEGraphics {
    public static var MainLibraries: [LibraryTypes: CEStandardLibrary] = [:]
    
    public static func staticLibraries() {
        MainLibraries.updateValue(CEUtilitiesLibrary(), forKey: .Utilities)
        print("static UtilitiesLibrary")
        
        MainLibraries.updateValue(CEFragmentShaderLibrary(), forKey: .FragmentShader)
        print("static FragmentShaderLibrary")

        MainLibraries.updateValue(CEVertexShaderLibrary(), forKey: .VertexShader)
        print("static VertexShaderLibrary")

        MainLibraries.updateValue(CEVertexDescriptorLibrary(), forKey: .VertexDescriptor)
        print("static VertexDescriptorLibrary")

        MainLibraries.updateValue(CEDepthStencilStateLibrary(), forKey: .DepthStencilState)
        print("static DepthStencilStateLibrary")

        MainLibraries.updateValue(CERenderPipelineDescriptorLibrary(
            fragmentShaderLibrary: MainLibraries[.FragmentShader] as! CEFragmentShaderLibrary,
            vertexShaderLibrary: MainLibraries[.VertexShader] as! CEVertexShaderLibrary,
            vertexDescriptorLibrary: MainLibraries[.VertexDescriptor] as! CEVertexDescriptorLibrary
        ), forKey: .RenderPipelineDescriptor)
        print("static RenderPipelineDescriptorLibrary")

        MainLibraries.updateValue(CERenderPipelineStateLibrary(
            device: ConceptEngine.GPUDevice,
            renderPipelineDescriptorLibrary: MainLibraries[.RenderPipelineDescriptor] as! CERenderPipelineDescriptorLibrary
        ), forKey: .RenderPipelineState)
        print("static RenderPipelineStateLibrary")

        MainLibraries.updateValue(CEMeshLibrary(), forKey: .Mesh)
        print("static MeshLibrary")

        MainLibraries.updateValue(CETextureLibrary(), forKey: .Texture)
        print("static TextureLibrary")

        MainLibraries.updateValue(CESamplerStateLibrary(), forKey: .SamplerState)
        print("static SamplerStateLibrary")
    }
}
