//
//  Engine.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class ConceptEngine {
    
    public var Device: MTLDevice!
    public var CommandQueue: MTLCommandQueue!
    public var ShaderLibrary: ConceptShaderLibrary!
    public var VertexDescriptorLibrary: ConceptVertexDescriptorLibrary!
    public var RenderPipelineDescriptorLibrary: ConceptRenderPipelineDescriptorLibrary!
    public var RenderPipelineStateLibrary: ConceptRenderPipelineStateLibrary!
    public var Utils: Utilities!
    
    required init(device: MTLDevice) {
        self.Utils = Utilities()
        self.Device = device
        self.CommandQueue = device.makeCommandQueue()
        self.ShaderLibrary = ConceptShaderLibrary(device: device)
        self.VertexDescriptorLibrary = ConceptVertexDescriptorLibrary()
        self.RenderPipelineDescriptorLibrary = ConceptRenderPipelineDescriptorLibrary(shaderLibrary: ShaderLibrary, vertexDescriptorLibrary: VertexDescriptorLibrary)
        self.RenderPipelineStateLibrary = ConceptRenderPipelineStateLibrary(device: device, renderPipelineDescriptorLibrary: RenderPipelineDescriptorLibrary)
    }
    
    public static func Initialize(device: MTLDevice) -> ConceptEngine {
        return ConceptEngine(device: device)
    }
}
