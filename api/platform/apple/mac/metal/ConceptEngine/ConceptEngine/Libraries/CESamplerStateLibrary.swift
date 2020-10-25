//
//  CESamplerStateLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 14/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum SamplerStateTypes {
    case None
    case Linear
}

protocol CESamplerState {
    var stateName: String { get }
    var samplerState: MTLSamplerState! { get }
}

public struct LinearSamplerState: CESamplerState {
    var stateName: String = "Linear Sampler State"
    
    var samplerState: MTLSamplerState!
    init() {
        let samplerDescriptor = MTLSamplerDescriptor()
        samplerDescriptor.minFilter = .linear
        samplerDescriptor.magFilter = .linear
        samplerDescriptor.mipFilter = .linear
//        samplerDescriptor.lodMinClamp = 4
        samplerDescriptor.label = stateName
        samplerState = ConceptEngine.GPUDevice.makeSamplerState(descriptor: samplerDescriptor)
    }
}


public final class CESamplerStateLibrary: CELibrary<SamplerStateTypes, MTLSamplerState>, CEStandardLibrary {
    
    private var samplerStates: [SamplerStateTypes: CESamplerState] = [:]
    
    override func useLibrary() {
        createDefaultSamplerState()
    }
    
    override subscript(type: SamplerStateTypes) -> MTLSamplerState? {
        return (samplerStates[type]?.samplerState!)!
    }
    
    private func createDefaultSamplerState() {
        samplerStates.updateValue(LinearSamplerState(), forKey: .Linear)
    }
    
    public func SamplerState(_ samplerStatetype: SamplerStateTypes) -> MTLSamplerState {
        return samplerStates[samplerStatetype]!.samplerState
    }
}
