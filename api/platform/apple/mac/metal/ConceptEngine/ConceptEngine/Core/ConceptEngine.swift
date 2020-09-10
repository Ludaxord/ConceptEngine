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
    
    required init(device: MTLDevice) {
        self.Device = device
        self.CommandQueue = device.makeCommandQueue()
        self.ShaderLibrary = ConceptShaderLibrary(device: device)
    }
    
    public static func Initialize(device: MTLDevice) -> ConceptEngine {
        return ConceptEngine(device: device)
    }
}
