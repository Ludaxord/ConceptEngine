//
//  Types.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 09/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import simd

public typealias float2 = SIMD2<Float>
public typealias float3 = SIMD3<Float>
public typealias float4 = SIMD4<Float>

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

extension Int32: sizeable {}
extension Float: sizeable {}
extension float2: sizeable {}
extension float3: sizeable {}
extension float4: sizeable {}

struct CEVertex: sizeable {
    var position: float3
    var color: float4
    var textureCoordinate: float2
    var normal: float3
}

struct CEModelDefaults: sizeable {
    var modelMatrix = matrix_identity_float4x4
}

struct CESceneDefaults: sizeable {
    var gameTime: Float = 0
    var viewMatrix = matrix_identity_float4x4
    var projectionMatrix = matrix_identity_float4x4
    var cameraPosition = float3(0, 0, 0)
}

struct CEMaterial: sizeable {
    var color: float4 = float4(0.8, 0.8, 0.8, 1.0)
    var useMaterialColor: Bool = false
    var useTexture: Bool = false
    var useDefaultTrigonometricTexture: Bool = false
    var isIlluminated: Bool = true
    var ambient: float3 = float3(0.1, 0.1, 0.1)
    var diffuse: float3 = float3(1, 1, 1)
    var specular: float3 = float3(1, 1, 1)
    var shininess: Float = 1
}

struct CELightData: sizeable {
    var position: float3 = float3(0, 0, 0)
    var color: float3 = float3(1, 1, 1)
    var brightness: Float = 1.0;
    var ambientIntensity: Float = 1.0;
    var diffuseIntensity: Float = 1.0;
    var specularIntensity: Float = 1.0;
}

public struct CEVertexOptions: sizeable {
    
    var positions: [float3]!
    var colors: [float4]!
    var textureCoordinate: [float2]!
    var normals: [float3]!
    var combination: [(float3, float4, float2)]!
    
    
    init(positions: [float3], colors: [float4], textureCoordinate: [float2], normal: [float3]) {
        self.positions = positions
        self.colors = colors
        self.textureCoordinate = textureCoordinate
        self.normals = normal
        (self.positions, self.colors, self.textureCoordinate) = fillInSpacesInArrays(positions: positions, colors: colors, textureCoordinate: textureCoordinate, normal: normal)
        print("ZIPS: \(zip(self.positions, zip(self.colors, zip(self.textureCoordinate, self.normals))).map { ($0.0, $0.1, $0.1.1, $0.1.1.0) })")
        self.combination = zip(self.positions, zip(self.colors, self.textureCoordinate)).map { ( $0.0, $0.1.0, $0.1.1 ) }
//        self.combination = zip(self.positions, zip(self.colors, zip(self.textureCoordinate, self.normals))).map { ($0.0, $0.1, $0.1.1, $0.1.1.0) }
    }
    
    func fillInSpacesInArrays(positions: [float3], colors: [float4], textureCoordinate: [float2], normal: [float3]) -> ([float3], [float4], [float2]) {
        var mainArray = [positions, colors, textureCoordinate] as [Any]
        CEBaseUtils.fillNotEqualArrays(elements: [positions, colors, textureCoordinate]) { (result, indexesArray) in
            if !result {
                for i in indexesArray! {
                    let uniq: Int? = ([positions, colors, textureCoordinate] as [Array<Any>]).map { $0.count }.uniques.max()
                    var indexedArray = [positions, colors, textureCoordinate][i]
                    let f2Array = indexedArray as? Array<float2>
                    let f3Array = indexedArray as? Array<float3>
                    let f4Array = indexedArray as? Array<float4>
                    let f5Array = indexedArray as? Array<float3>
                    if f2Array != nil {
                        indexedArray = CEBaseUtils.fillFromType(of: uniq!, in: f2Array!)
                    } else if f3Array != nil {
                        indexedArray = CEBaseUtils.fillFromType(of: uniq!, in: f3Array!)
                    } else if f4Array != nil {
                        indexedArray = CEBaseUtils.fillFromType(of: uniq!, in: f4Array!)
                    } else if f5Array != nil {
//                        indexedArray = CEBaseUtils.fillFromType(of: uniq!, in: f5Array!)
                    }
                    mainArray[i] = indexedArray
                }
            }
        }
//        return (mainArray[0] as! [float3], mainArray[1] as! [float4], mainArray[2] as! [float2], mainArray[3] as! [float3])
        return (mainArray[0] as! [float3], mainArray[1] as! [float4], mainArray[2] as! [float2])
    }
}

