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
    case Mesh
    case Utilities
}

public enum ManagerTypes {
    case SceneManager
}

public final class ConceptEngine {
    
    public static var GPUDevice: MTLDevice!
    public static var CommandQueue: MTLCommandQueue!
    
    private var ShaderLibrary: CEShaderLibrary!
    private var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    private var RenderPipelineDescriptorLibrary: CERenderPipelineDescriptorLibrary!
    private var RenderPipelineStateLibrary: CERenderPipelineStateLibrary!
    private var MeshLibrary: CEMeshLibrary!
    private var UtilitiesLibrary: CEUtilitiesLibrary!
    
    private var SceneManager: CESceneManager!
    
    private static var Libraries: [LibraryTypes: CEStandardLibrary] = [:]
    private static var Managers: [ManagerTypes: CEManager] = [:]
    
    required init(device: MTLDevice) {
        ConceptEngine.GPUDevice = device
        ConceptEngine.CommandQueue = device.makeCommandQueue()
        instanceLibraries(device: ConceptEngine.GPUDevice)
        instanceManagers()
        
    }
    
    private func instanceLibraries(device: MTLDevice) {
        print("gpu: \(device)")
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
        self.MeshLibrary = CEMeshLibrary()
        ConceptEngine.Libraries.updateValue(MeshLibrary, forKey: .Mesh)
    }
    
    private func instanceManagers() {
        self.SceneManager = CESceneManager(CEUtilitiesLibrary.DefaultSceneType)
        ConceptEngine.Managers.updateValue(SceneManager, forKey: .SceneManager)
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
    
    static func getManager(_ managerType: ManagerTypes) -> CEManager? {
        return ConceptEngine.Managers[managerType]
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
