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
}

protocol CERenderPipelineState {
    var stateName: String { get }
    var renderPipelineState: MTLRenderPipelineState! { get }
}

public struct BasicRenderPipelineState: CERenderPipelineState {
    var stateName: String = "Basic Render Pipeline State"
    
    var renderPipelineState: MTLRenderPipelineState!
    init() {
        renderPipelineState = CERenderPipelineStateLibrary.createRenderPipelineState(stateName: stateName)
    }
}


public final class CERenderPipelineStateLibrary: CEStandardLibrary {
    
    private var renderPipelineStates: [RenderPipelineStateTypes: CERenderPipelineState] = [:]
    
    public static var DefeultDevice: MTLDevice!
    public static var RenderPipelineDescriptorLibrary: CERenderPipelineDescriptorLibrary!
    
    private var vertexShaders: [VertexShaderTypes: CEShader] = [:]
    private var fragmentShaders: [FragmentShaderTypes: CEShader] = [:]
    
    required init(device: MTLDevice, renderPipelineDescriptorLibrary: CERenderPipelineDescriptorLibrary) {
        CERenderPipelineStateLibrary.DefeultDevice = device
        CERenderPipelineStateLibrary.RenderPipelineDescriptorLibrary = renderPipelineDescriptorLibrary
        createDefaultRenderPipelineState()
    }
    
    public static func createRenderPipelineState(stateName: String) -> MTLRenderPipelineState {
        var renderPipelineState: MTLRenderPipelineState!
        do {
            renderPipelineState = try CERenderPipelineStateLibrary.DefeultDevice.makeRenderPipelineState(descriptor:
                CERenderPipelineStateLibrary.RenderPipelineDescriptorLibrary.RenderDescriptor(.Basic)
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
    }
}
