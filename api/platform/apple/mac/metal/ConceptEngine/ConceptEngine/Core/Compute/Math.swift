//
//  Math.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 13/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//
import MetalKit

class CEMathUtils {
    //Source: https://stackoverflow.com/a/48634199/7927803
    public static func factorial(_ N: Double) -> Double {
        var mult = N
        var retVal: Double = 1.0
        while mult > 0.0 {
            retVal *= mult
            mult -= 1.0
        }
        return retVal
    }
    
    public static func combinationsWithoutRepeats(_ n: Double, _ k: Double) -> Double {
        return (factorial(n)) / (factorial(k) * (factorial(n - k)))
    }
    
    public static func createCombinationsArray(allElements: Array<Int>) -> Array<Array<Int>> {
        let combinations = Int(CEMathUtils.combinationsWithoutRepeats(Double(allElements.count), Double(allElements.count - 1)))
        var tempArr: Array<Array<Int>> = []
        for n in 0...(combinations - 1) {
            for (index, _) in allElements.enumerated() {
                if index != n {
                    if tempArr.isEmpty {
                        tempArr.append([index, n])
                    } else {
                        var exists = false
                        for t in tempArr {
                            if [index, n].containsSameElements(as: t) {
                                exists = true
                            }
                        }
                        if !exists {
                            tempArr.append([index, n])
                        }
                    }
                }
            }
        }
        
        return tempArr
        
    }
}

public var X_AXIS: float3{
    return float3(1,0,0)
}

public var Y_AXIS: float3{
    return float3(0,1,0)
}

public var Z_AXIS: float3{
    return float3(0,0,1)
}

extension Float {
    var toRadians: Float {
        return (self / 180.0) * Float.pi
    }
    
    var toDegrees: Float {
        return self * (180.0 / Float.pi)
    }
    
    static var randomFromZeroToOne: Float {
        return Float(arc4random()) / Float(UINT32_MAX)
    }
}

extension matrix_float4x4{
    
    mutating func translate(direction: float3){
        var result = matrix_identity_float4x4
        
        let x: Float = direction.x
        let y: Float = direction.y
        let z: Float = direction.z
        
        result.columns = (
            float4(1,0,0,0),
            float4(0,1,0,0),
            float4(0,0,1,0),
            float4(x,y,z,1)
        )
        
        self = matrix_multiply(self, result)
    }
    
    mutating func scale(axis: float3){
        var result = matrix_identity_float4x4
        
        let x: Float = axis.x
        let y: Float = axis.y
        let z: Float = axis.z
        
        result.columns = (
            float4(x,0,0,0),
            float4(0,y,0,0),
            float4(0,0,z,0),
            float4(0,0,0,1)
        )
        
        self = matrix_multiply(self, result)
    }
    
    
    mutating func rotate(angle: Float, axis: float3){
        var result = matrix_identity_float4x4
        
        let x: Float = axis.x
        let y: Float = axis.y
        let z: Float = axis.z
        
        let c: Float = cos(angle)
        let s: Float = sin(angle)
        
        let mc: Float = (1 - c)
        
        let r1c1: Float = x * x * mc + c
        let r2c1: Float = x * y * mc + z * s
        let r3c1: Float = x * z * mc - y * s
        let r4c1: Float = 0.0
        
        let r1c2: Float = y * x * mc - z * s
        let r2c2: Float = y * y * mc + c
        let r3c2: Float = y * z * mc + x * s
        let r4c2: Float = 0.0
        
        let r1c3: Float = z * x * mc + y * s
        let r2c3: Float = z * y * mc - x * s
        let r3c3: Float = z * z * mc + c
        let r4c3: Float = 0.0
        
        let r1c4: Float = 0.0
        let r2c4: Float = 0.0
        let r3c4: Float = 0.0
        let r4c4: Float = 1.0
        
        result.columns = (
            float4(r1c1, r2c1, r3c1, r4c1),
            float4(r1c2, r2c2, r3c2, r4c2),
            float4(r1c3, r2c3, r3c3, r4c3),
            float4(r1c4, r2c4, r3c4, r4c4)
        )
        
        self = matrix_multiply(self, result)
    }
    
    //source: https://gamedev.stackexchange.com/questions/120338/what-does-a-perspective-projection-matrix-look-like-in-opengl
    static func perspective(degreesFieldOfView: Float, aspectRatio: Float, near: Float, far: Float) -> matrix_float4x4 {
        let fieldOfView = degreesFieldOfView.toRadians
        
        let tang: Float = tan(fieldOfView / 2)
        
        let x: Float = 1 / (aspectRatio * tang)
        let y: Float = 1 / tang
        let z: Float = -((far + near) / (far - near))
        let w: Float = -((2 * far * near) / (far - near))
        
        var result = matrix_identity_float4x4
        result.columns = (
            float4(x, 0, 0,  0),
            float4(0, y, 0,  0),
            float4(0, 0, z, -1),
            float4(0, 0, w,  0)
        )
        
        return result
    }
    
}
