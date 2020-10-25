//
//  ConceptRenderPipelineStateLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum RenderPipelineStateTypes {
    case Basic
    case Instanced
}

protocol CERenderPipelineState {
    var stateName: String { get }
    var renderPipelineState: MTLRenderPipelineState! { get }
}

public struct BasicRenderPipelineState: CERenderPipelineState {
    var stateName: String = "Basic Render Pipeline State"
    
    var renderPipelineState: MTLRenderPipelineState!
    init() {
        renderPipelineState = CERenderPipelineStateLibrary.createRenderPipelineState(stateName: stateName, .Basic)
    }
}

public struct InstancedRenderPipelineState: CERenderPipelineState {
    var stateName: String = "Instanced Render Pipeline State"
    
    var renderPipelineState: MTLRenderPipelineState!
    init() {
        renderPipelineState = CERenderPipelineStateLibrary.createRenderPipelineState(stateName: stateName, .Instanced)
    }
}


public final class CERenderPipelineStateLibrary: CELibrary<RenderPipelineStateTypes, MTLRenderPipelineState>, CEStandardLibrary {
    
    private var renderPipelineStates: [RenderPipelineStateTypes: CERenderPipelineState] = [:]
    
    public static var DefeultDevice: MTLDevice!
    public static var RenderPipelineDescriptorLibrary: CERenderPipelineDescriptorLibrary!
    
//    private var vertexShaders: [VertexShaderTypes: CEShader] = [:]
    private var shaders: [ShaderTypes: CEShader] = [:]
    
    override func useLibrary() {
        createDefaultRenderPipelineState()
    }
    
    override subscript(type: RenderPipelineStateTypes) -> MTLRenderPipelineState? {
        return PipelineState(type)
    }
    
    required init(device: MTLDevice, renderPipelineDescriptorLibrary: CERenderPipelineDescriptorLibrary) {
        CERenderPipelineStateLibrary.DefeultDevice = device
        CERenderPipelineStateLibrary.RenderPipelineDescriptorLibrary = renderPipelineDescriptorLibrary
        super.init()
    }
    
    public static func createRenderPipelineState(stateName: String, _ renderPipelineDescriptorType :RenderPipelineDescriptorTypes) -> MTLRenderPipelineState {
        var renderPipelineState: MTLRenderPipelineState!
        do {
            renderPipelineState = try CERenderPipelineStateLibrary.DefeultDevice.makeRenderPipelineState(descriptor:
                CERenderPipelineStateLibrary.RenderPipelineDescriptorLibrary.RenderDescriptor(renderPipelineDescriptorType)
            )
        } catch let error as NSError {
            print("ERROR::CREATE::RENDER_PIPELINE_STATE::__\(stateName)__::\(error)")
        }
        return renderPipelineState
    }
    
    public func PipelineState(_ renderPipelineStateType: RenderPipelineStateTypes) -> MTLRenderPipelineState {
        return (renderPipelineStates[renderPipelineStateType]?.renderPipelineState)!
    }
    
    private func createDefaultRenderPipelineState() {
        renderPipelineStates.updateValue(BasicRenderPipelineState(), forKey: .Basic)
        renderPipelineStates.updateValue(InstancedRenderPipelineState(), forKey: .Instanced)
    }
}
