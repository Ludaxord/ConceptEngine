//
//  CEDepthStencilStateLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 18/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum DepthStencilStateTypes {
    case Less
}

protocol CEDepthStencilState {
    var depthStencilState: MTLDepthStencilState! { get }
}

public struct LessDepthStencilState: CEDepthStencilState {
    var depthStencilState: MTLDepthStencilState!
    
    init() {
        let depthStencilDescriptor = MTLDepthStencilDescriptor()
        depthStencilDescriptor.isDepthWriteEnabled = true
        depthStencilDescriptor.depthCompareFunction = .less
        depthStencilState = ConceptEngine.GPUDevice.makeDepthStencilState(descriptor: depthStencilDescriptor)
    }
}


public final class CEDepthStencilStateLibrary: CELibrary<DepthStencilStateTypes, MTLDepthStencilState>, CEStandardLibrary {
    
    public static var DefeultDevice: MTLDevice!
    
    private var depthStencilStates: [DepthStencilStateTypes: CEDepthStencilState] = [:]
    
    private static var ShaderLibrary: CEShaderLibrary!
    private static var VertexDescriptorLibrary: CEVertexDescriptorLibrary!
    
    override func useLibrary() {
        createDefaultDepthStencilStates()
    }
    
    override subscript(type: DepthStencilStateTypes) -> MTLDepthStencilState? {
        return DepthStencilState(type)
    }
        
    public func DepthStencilState(_ depthStencilStateType: DepthStencilStateTypes) -> MTLDepthStencilState {
        return depthStencilStates[depthStencilStateType]!.depthStencilState
    }
    
    private func createDefaultDepthStencilStates() {
        depthStencilStates.updateValue(LessDepthStencilState(), forKey: .Less)
    }
}
