//
//  Engine.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit


public enum LibraryTypes {
    case FragmentShader
    case VertexShader
    case VertexDescriptor
    case RenderPipelineDescriptor
    case RenderPipelineState
    case Mesh
    case DepthStencilState
    case Utilities
}

public enum ManagerTypes {
    case SceneManager
}

public final class ConceptEngine {
    
    public static var GPUDevice: MTLDevice!
    public static var CommandQueue: MTLCommandQueue!
    public static var DefaultLibrary: MTLLibrary!
    
    private static var Graphics: CEGraphics!

    private var SceneManager: CESceneManager!
    
    private static var Managers: [ManagerTypes: CEManager] = [:]
    
    required init(device: MTLDevice) {
        ConceptEngine.GPUDevice = device
        ConceptEngine.CommandQueue = device.makeCommandQueue()
        ConceptEngine.DefaultLibrary = device.makeDefaultLibrary()
        ConceptEngine.Graphics = CEGraphics(device: device)
        instanceManagers()
        
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
        return Graphics.Libraries[libraryType]
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
        view.depthStencilPixelFormat = CEUtilitiesLibrary.DepthPixelFormat
    }
    
    func injectGpuToView(view: MTKView) {
        view.device = self.getGPUDevice()
    }
}
