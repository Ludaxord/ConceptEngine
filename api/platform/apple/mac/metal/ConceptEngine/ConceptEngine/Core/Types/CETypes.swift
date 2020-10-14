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

extension Float: sizeable {}
extension float2: sizeable {}
extension float3: sizeable {}
extension float4: sizeable {}

struct CEVertex: sizeable {
    var position: float3
    var color: float4
    var textureCoordinate: float2  = float2(0)
}

struct CEModelDefaults: sizeable {
    var modelMatrix = matrix_identity_float4x4
}

struct CESceneDefaults: sizeable {
    var gameTime: Float = 0
    var viewMatrix = matrix_identity_float4x4
    var projectionMatrix = matrix_identity_float4x4
}

struct CEMaterial: sizeable {
    var color: float4 = float4(0.8, 0.8, 0.8, 1.0)
    var useMaterialColor: Bool = false
    var useTexture: Bool = false
    var useDefaultTrigonometricTexture: Bool = false
}

public struct CEVertexOptions: sizeable {
    
    var positions: [float3]!
    var colors: [float4]!
    var textureCoordinate: [float2]!
    var combination: [(float3, float4, float2)]!
    
    
    init(positions: [float3], colors: [float4], textureCoordinate: [float2]) {
        (self.positions, self.colors, self.textureCoordinate) = fillInSpacesInArrays(positions: positions, colors: colors, textureCoordinate: textureCoordinate)
        self.combination = zip(self.positions, zip(self.colors, self.textureCoordinate)).map { ( $0.0, $0.1.0, $0.1.1 ) }
    }
    
    func fillInSpacesInArrays(positions: [float3], colors: [float4], textureCoordinate: [float2]) -> ([float3], [float4], [float2]) {
        var mainArray = [positions, colors, textureCoordinate] as [Any]
        CEBaseUtils.fillNotEqualArrays(elements: [positions, colors, textureCoordinate]) { (result, indexesArray) in
            if !result {
                for i in indexesArray! {
                    let uniq: Int? = ([positions, colors, textureCoordinate] as [Array<Any>]).map { $0.count }.uniques.max()
                    var indexedArray = [positions, colors, textureCoordinate][i]
                    let f2Array = indexedArray as? Array<float2>
                    let f3Array = indexedArray as? Array<float3>
                    let f4Array = indexedArray as? Array<float4>
                    if f2Array != nil {
                        indexedArray = CEBaseUtils.fillFromType(of: uniq!, in: f2Array!)
                    } else if f3Array != nil {
                        indexedArray = CEBaseUtils.fillFromType(of: uniq!, in: f3Array!)
                    } else if f4Array != nil {
                        indexedArray = CEBaseUtils.fillFromType(of: uniq!, in: f4Array!)
                    }
                    mainArray[i] = indexedArray
                }
            }
        }
        return (mainArray[0] as! [float3], mainArray[1] as! [float4], mainArray[2] as! [float2])
    }
}
