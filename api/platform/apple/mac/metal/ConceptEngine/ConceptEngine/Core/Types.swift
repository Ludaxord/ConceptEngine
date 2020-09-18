//
//  Types.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 09/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import simd

protocol sizeable {}

extension sizeable {
    static var size: Int {
        return MemoryLayout<Self>.size
    }
    
    static var stride: Int {
        return MemoryLayout<Self>.stride
    }
    
    static func size(_ count: Int) -> Int {
        return MemoryLayout<Self>.size * count
    }
    
    static func stride(_ count: Int) -> Int {
        return MemoryLayout<Self>.stride * count
    }
}

extension float3: sizeable {}
extension Float: sizeable {}

struct CEVertex: sizeable {
    var position: float3
    var color: float4
}

struct CEModelDefaults: sizeable {
    var modelMatrix = matrix_identity_float4x4
}

struct CESceneDefaults: sizeable {
    var viewMatrix = matrix_identity_float4x4
    var projectionMatrix = matrix_identity_float4x4
}
