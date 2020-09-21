//
//  ColorUtils.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 18/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import simd

class CEColorUtils {
    public static var randomColor: float4 {
        return float4(Float.randomFromZeroToOne, Float.randomFromZeroToOne, Float.randomFromZeroToOne, 1.0)
    }
    
    public static func createColor(r: Float, g: Float, b: Float, alpha: Float = 1.0) -> float4 {
        return float4(r, g, b, 1.0)
    }
}
