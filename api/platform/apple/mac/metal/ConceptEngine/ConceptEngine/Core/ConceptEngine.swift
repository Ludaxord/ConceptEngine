//
//  Engine.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit


public enum LibraryTypes {
    case Shader
    case VertexDescriptor
    case RenderPipelineDescriptor
    case RenderPipelineState
    case Utilities
}

public final class ConceptEngine {
    
    public static var GPUDevice: MTLDevice!
    public static var CommandQueue: MTLCommandQueue!
    
    private var ShaderLibrary: CEShaderLibrary!
    private var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    private var RenderPipelineDescriptorLibrary: CERenderPipelineDescriptorLibrary!
    private var RenderPipelineStateLibrary: CERenderPipelineStateLibrary!
    private var UtilitiesLibrary: CEUtilitiesLibrary!
    
    private static var Libraries: [LibraryTypes: CEStandardLibrary] = [:]
    
    required init(device: MTLDevice) {
        ConceptEngine.GPUDevice = device
        ConceptEngine.CommandQueue = device.makeCommandQueue()
        instanceLibraries(device: ConceptEngine.GPUDevice)
        
    }
    
    private func instanceLibraries(device: MTLDevice) {
        self.UtilitiesLibrary = CEUtilitiesLibrary()
        ConceptEngine.Libraries.updateValue(UtilitiesLibrary, forKey: .Utilities)
        self.ShaderLibrary = CEShaderLibrary(device: device)
        ConceptEngine.Libraries.updateValue(ShaderLibrary, forKey: .Shader)
        self.VertexDescriptorLibrary = CEVertexDescriptorLibrary()
        ConceptEngine.Libraries.updateValue(VertexDescriptorLibrary, forKey: .VertexDescriptor)
        self.RenderPipelineDescriptorLibrary = CERenderPipelineDescriptorLibrary(shaderLibrary: self.ShaderLibrary, vertexDescriptorLibrary: self.VertexDescriptorLibrary)
        ConceptEngine.Libraries.updateValue(RenderPipelineDescriptorLibrary, forKey: .RenderPipelineDescriptor)
        self.RenderPipelineStateLibrary = CERenderPipelineStateLibrary(device: device, renderPipelineDescriptorLibrary: self.RenderPipelineDescriptorLibrary)
        ConceptEngine.Libraries.updateValue(RenderPipelineStateLibrary, forKey: .RenderPipelineState)
    }
    
    public static func Initialize(device: MTLDevice? = nil) -> ConceptEngine {
        var gpuDevice = device
        if gpuDevice == nil {
            gpuDevice = MTLCreateSystemDefaultDevice()
        }
        return ConceptEngine(device: gpuDevice!)
    }
}


extension ConceptEngine {
    static func getLibrary(_ libraryType: LibraryTypes) -> CEStandardLibrary? {
        return ConceptEngine.Libraries[libraryType]
    }
    
    func getGPUDevice() -> MTLDevice {
        return ConceptEngine.GPUDevice
    }
    
    func injectDefaultColorsToView(view: MTKView) {
        view.clearColor = CEUtilitiesLibrary.ClearColor
        view.colorPixelFormat = CEUtilitiesLibrary.PixelFormat
    }
    
    func injectGpuToView(view: MTKView) {
        view.device = self.getGPUDevice()
    }
}
