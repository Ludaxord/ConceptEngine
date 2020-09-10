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

protocol ConceptRenderPipelineState {
    var stateName: String { get }
    var renderPipelineState: MTLRenderPipelineState { get }
}

public struct BasicRenderPipelineState: ConceptRenderPipelineState {
    var stateName: String = "Basic Render Pipeline State"
    
    var renderPipelineState: MTLRenderPipelineState {
        var renderPipelineState: MTLRenderPipelineState!
        do {
            renderPipelineState = try ConceptRenderPipelineStateLibrary.DefeultDevice.makeRenderPipelineState(descriptor:
                ConceptRenderPipelineStateLibrary.RenderPipelineDescriptorLibrary.RenderDescriptor(.Basic)
            )
        } catch let error as NSError {
            print("ERROR::CREATE::RENDER_PIPELINE_STATE::__\(stateName)__::\(error)")
        }
        return renderPipelineState
    }
}


public class ConceptRenderPipelineStateLibrary {
    
    private var renderPipelineStates: [RenderPipelineStateTypes: ConceptRenderPipelineState] = [:]
    
    public static var DefeultDevice: MTLDevice!
    public static var RenderPipelineDescriptorLibrary: ConceptRenderPipelineDescriptorLibrary!
    
    private var vertexShaders: [VertexShaderTypes: ConceptShader] = [:]
    private var fragmentShaders: [FragmentShaderTypes: ConceptShader] = [:]
    
    required init(device: MTLDevice, renderPipelineDescriptorLibrary: ConceptRenderPipelineDescriptorLibrary) {
        ConceptRenderPipelineStateLibrary.DefeultDevice = device
        ConceptRenderPipelineStateLibrary.RenderPipelineDescriptorLibrary = renderPipelineDescriptorLibrary
        createDefaultRenderPipelineState()
    }
    
    public func PipelineState(_ renderPipelineStateType: RenderPipelineStateTypes) -> MTLRenderPipelineState {
        return (renderPipelineStates[renderPipelineStateType]?.renderPipelineState)!
    }
    
    private func createDefaultRenderPipelineState() {
        renderPipelineStates.updateValue(BasicRenderPipelineState(), forKey: .Basic)
    }
}
